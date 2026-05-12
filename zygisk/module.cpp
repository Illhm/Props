#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <string.h>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/sysmacros.h>

#include "zygisk.hpp"

#define LOG_TAG "ZygiskMockProps"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

// Tipe untuk original function
typedef int (*system_property_get_t)(const char*, char*);
typedef void (*system_property_read_callback_t)(const prop_info*, void (*)(void*, const char*, const char*, uint32_t), void*);
typedef const prop_info* (*system_property_find_t)(const char*);

static system_property_get_t orig_system_property_get = nullptr;
static system_property_read_callback_t orig_system_property_read_callback = nullptr;
static system_property_find_t orig_system_property_find = nullptr;

static std::map<std::string, std::string> custom_props;
// Mapping memori palsu untuk prop_info pointer agar bisa diidentifikasi di read_callback
static std::map<const prop_info*, std::string> fake_prop_info_map;
// Alokasi memori dummy statis untuk properti yang tidak ada
static std::map<std::string, char> dummy_allocs;

// Baca string dari file descriptor (IPC) dengan penanganan partial read
static std::string readStringFd(int fd) {
    size_t length;
    if (read(fd, &length, sizeof(length)) != sizeof(length)) return "";
    if (length == 0) return "";

    std::string str(length, '\0');
    size_t total_read = 0;
    while (total_read < length) {
        ssize_t bytes_read = read(fd, &str[total_read], length - total_read);
        if (bytes_read <= 0) break; // Error or EOF
        total_read += bytes_read;
    }

    if (total_read != length) return "";
    return str;
}

// Tulis string ke file descriptor (IPC)
static void writeStringFd(int fd, const std::string& str) {
    size_t length = str.length();
    write(fd, &length, sizeof(length));
    if (length > 0) {
        write(fd, str.c_str(), length);
    }
}

// ==========================================
// Zygisk Companion Process (Berjalan sebagai root/su)
// ==========================================
// Cache payload agar tidak membaca disk setiap kali aplikasi startup
static std::string cached_payload = "";
static bool is_cached = false;

static void companion_handler(int client_fd) {
    if (!is_cached) {
        LOGD("Companion handler reading from disk...");
        // Gunakan direktori instalasi modul Magisk yang benar
        std::ifstream file("/data/adb/modules/xkatrina_snstv_prps/custom_props.txt");
        if (!file.is_open()) {
            LOGE("Companion gagal membaca /data/adb/modules/xkatrina_snstv_prps/custom_props.txt");
            writeStringFd(client_fd, "ERROR");
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::istringstream iss(line);
            std::string cmd, key, val, word;
            iss >> cmd >> key;

            if (cmd == "check_resetprop" && !key.empty()) {
                val = "";
                while (iss >> word) {
                    if (!val.empty()) val += " ";
                    if (!word.empty() && word.front() == '"') word.erase(0, 1);
                    if (!word.empty() && word.back() == '"') word.pop_back();
                    val += word;
                }
                if (!val.empty()) {
                    cached_payload += key + "=" + val + "\n";
                }
            }
        }
        is_cached = true;
    }

    // Kirim seluruh payload ke proses aplikasi
    writeStringFd(client_fd, cached_payload);
}

REGISTER_ZYGISK_COMPANION(companion_handler)

// ==========================================
// Proses Hooking
// ==========================================

// Hook __system_property_get
int my_system_property_get(const char* name, char* value) {
    if (name != nullptr) {
        auto it = custom_props.find(name);
        if (it != custom_props.end()) {
            strncpy(value, it->second.c_str(), PROP_VALUE_MAX);
            value[PROP_VALUE_MAX - 1] = '\0';
            // LOGD("Spoofed __system_property_get: %s -> %s", name, value);
            return strlen(value);
        }
    }
    if (orig_system_property_get) {
        return orig_system_property_get(name, value);
    }
    return 0; // Seharusnya tidak pernah dicapai jika hook berjalan benar
}

// Hook __system_property_find
const prop_info* my_system_property_find(const char* name) {
    if (name != nullptr) {
        auto it = custom_props.find(name);
        if (it != custom_props.end()) {
            // Karena mapping fake_prop_info_map sudah di-pre-populate secara single-threaded
            // di postAppSpecialize, kita tinggal iterasi untuk mencari match.
            for (const auto& pair : fake_prop_info_map) {
                if (pair.second == name) {
                    return pair.first;
                }
            }
        }
    }
    if (orig_system_property_find) {
        return orig_system_property_find(name);
    }
    return nullptr;
}

// Hook __system_property_read_callback (Digunakan Android 8+ dan java.os.Build)
void my_system_property_read_callback(const prop_info* pi,
                                      void (*callback)(void* cookie, const char* name, const char* value, uint32_t serial),
                                      void* cookie) {
    if (pi != nullptr && callback != nullptr) {
        auto it = fake_prop_info_map.find(pi);
        if (it != fake_prop_info_map.end()) {
            std::string prop_name = it->second;
            auto prop_val_it = custom_props.find(prop_name);
            if (prop_val_it != custom_props.end()) {
                // Panggil callback langsung secara synchronous tanpa wrapper kompleks
                callback(cookie, prop_name.c_str(), prop_val_it->second.c_str(), 0);
                return;
            }
        }
    }

    if (orig_system_property_read_callback) {
        orig_system_property_read_callback(pi, callback, cookie);
    }
}

class MockPropsModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        // Ambil nama proses dengan aman (nice_name bisa null)
        const char *process = nullptr;
        if (args->nice_name != nullptr) {
            process = env->GetStringUTFChars(args->nice_name, nullptr);
            LOGD("preAppSpecialize: %s", process);
        } else {
            LOGD("preAppSpecialize: <unknown>");
        }

        // Panggil Companion untuk mengambil properti
        int fd = api->connectCompanion();
        if (fd >= 0) {
            std::string payload = readStringFd(fd);
            close(fd);

            if (payload != "ERROR" && !payload.empty()) {
                std::istringstream iss(payload);
                std::string line;
                while (std::getline(iss, line)) {
                    size_t pos = line.find('=');
                    if (pos != std::string::npos) {
                        std::string key = line.substr(0, pos);
                        std::string val = line.substr(pos + 1);
                        custom_props[key] = val;
                    }
                }
                LOGD("Loaded %zu properties via Companion", custom_props.size());
            }
        } else {
            LOGE("Gagal terhubung ke Companion");
        }

        if (process != nullptr) {
            env->ReleaseStringUTFChars(args->nice_name, process);
        }
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        if (custom_props.empty()) return;

        LOGD("Registering PLT Hooks...");

        // Dapatkan original find function terlebih dahulu untuk pre-populate map
        void* libc = dlopen("libc.so", RTLD_LAZY);
        if (libc) {
            orig_system_property_find = reinterpret_cast<system_property_find_t>(dlsym(libc, "__system_property_find"));
            dlclose(libc);
        }

        // Pre-populate mapping untuk menghindari modifikasi map di multi-threaded runtime
        for (const auto& pair : custom_props) {
            const char* name = pair.first.c_str();
            const prop_info* real_pi = nullptr;

            if (orig_system_property_find) {
                real_pi = orig_system_property_find(name);
            }

            if (real_pi == nullptr) {
                // Alokasi pointer dummy statis jika tidak ada
                real_pi = reinterpret_cast<const prop_info*>(&dummy_allocs[pair.first]);
            }

            fake_prop_info_map[real_pi] = pair.first;
        }

        // Parse /proc/self/maps untuk mencari dev dan inode dari library target
        // Kita targetkan semua library yang diload oleh aplikasi (/data/app) dan library system terkait
        FILE* fp = fopen("/proc/self/maps", "r");
        if (fp) {
            char line[512];
            while (fgets(line, sizeof(line), fp)) {
                // Hook system libs dan semua third party app libs (misal SDK anti-fraud)
                if (strstr(line, "/libc.so") ||
                    strstr(line, "/libbase.so") ||
                    strstr(line, "/libandroid_runtime.so") ||
                    strstr(line, "/data/app/")) {

                    unsigned long start, end, offset, inode;
                    unsigned int dev_major, dev_minor;
                    char perms[5];

                    if (sscanf(line, "%lx-%lx %4s %lx %x:%x %lu",
                               &start, &end, perms, &offset, &dev_major, &dev_minor, &inode) == 7) {

                        if (inode != 0 && strchr(perms, 'x') != nullptr) { // Executable map
                            dev_t dev = makedev(dev_major, dev_minor);

                            // Register the hooks using Zygisk API
                            api->pltHookRegister(dev, inode, "__system_property_get",
                                                 (void*)my_system_property_get,
                                                 (void**)&orig_system_property_get);

                            api->pltHookRegister(dev, inode, "__system_property_find",
                                                 (void*)my_system_property_find,
                                                 (void**)&orig_system_property_find);

                            api->pltHookRegister(dev, inode, "__system_property_read_callback",
                                                 (void*)my_system_property_read_callback,
                                                 (void**)&orig_system_property_read_callback);
                        }
                    }
                }
            }
            fclose(fp);
        }

        // Commit hooks
        if (api->pltHookCommit()) {
            LOGD("Hooks committed successfully");
        } else {
            LOGE("Failed to commit hooks");
        }

        // Android Java properties (android.os.Build) di-cache secara statis saat Zygote startup.
        // PLT hooks di atas hanya bekerja untuk C/C++ native calls.
        // Agar aplikasi Java melihat spoofing, kita harus overwrite field static via JNI Reflection.
        jclass build_class = env->FindClass("android/os/Build");
        if (build_class) {
            auto setStringField = [&](const char* fieldName, const std::string& propKey) {
                auto it = custom_props.find(propKey);
                if (it != custom_props.end()) {
                    jfieldID fieldId = env->GetStaticFieldID(build_class, fieldName, "Ljava/lang/String;");
                    if (fieldId) {
                        jstring newStr = env->NewStringUTF(it->second.c_str());
                        env->SetStaticObjectField(build_class, fieldId, newStr);
                        env->DeleteLocalRef(newStr);
                    }
                }
            };

            setStringField("BRAND", "ro.product.brand");
            setStringField("MODEL", "ro.product.model");
            setStringField("DEVICE", "ro.product.device");
            setStringField("FINGERPRINT", "ro.build.fingerprint");
            setStringField("HARDWARE", "ro.hardware");
            setStringField("PRODUCT", "ro.build.product");
            setStringField("BOARD", "ro.product.board");
            setStringField("MANUFACTURER", "ro.product.manufacturer");

            env->DeleteLocalRef(build_class);
            LOGD("Java android.os.Build fields updated via JNI");
        }

        // Update nested class android.os.Build$VERSION
        jclass version_class = env->FindClass("android/os/Build$VERSION");
        if (version_class) {
            auto setStringFieldVersion = [&](const char* fieldName, const std::string& propKey) {
                auto it = custom_props.find(propKey);
                if (it != custom_props.end()) {
                    jfieldID fieldId = env->GetStaticFieldID(version_class, fieldName, "Ljava/lang/String;");
                    if (fieldId) {
                        jstring newStr = env->NewStringUTF(it->second.c_str());
                        env->SetStaticObjectField(version_class, fieldId, newStr);
                        env->DeleteLocalRef(newStr);
                    }
                }
            };

            setStringFieldVersion("RELEASE", "ro.build.version.release");
            setStringFieldVersion("INCREMENTAL", "ro.build.version.incremental");

            env->DeleteLocalRef(version_class);
            LOGD("Java android.os.Build$VERSION fields updated via JNI");
        }
    }

private:
    Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MockPropsModule)