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

static system_property_get_t orig_system_property_get = nullptr;
static system_property_read_callback_t orig_system_property_read_callback = nullptr;

static std::map<std::string, std::string> custom_props;

// Baca string dari file descriptor (IPC)
static std::string readStringFd(int fd) {
    size_t length;
    if (read(fd, &length, sizeof(length)) != sizeof(length)) return "";
    if (length == 0) return "";
    std::string str(length, '\0');
    if (read(fd, &str[0], length) != (ssize_t)length) return "";
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
static void companion_handler(int client_fd) {
    LOGD("Companion handler started");
    // Gunakan direktori instalasi modul Magisk yang benar
    std::ifstream file("/data/adb/modules/xkatrina_snstv_prps/custom_props.txt");
    if (!file.is_open()) {
        LOGE("Companion gagal membaca /data/adb/modules/xkatrina_snstv_prps/custom_props.txt");
        writeStringFd(client_fd, "ERROR");
        return;
    }

    std::string payload = "";
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
                payload += key + "=" + val + "\n";
            }
        }
    }

    // Kirim seluruh payload ke proses aplikasi
    writeStringFd(client_fd, payload);
    LOGD("Companion finished sending payload");
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

class MockPropsModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        // Ambil nama proses
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        LOGD("preAppSpecialize: %s", process);

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

        env->ReleaseStringUTFChars(args->nice_name, process);
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        if (custom_props.empty()) return;

        LOGD("Registering PLT Hooks...");

        // Register the hooks using Zygisk API
        // Catatan: Ini PLT hook, mungkin tidak menangkap pemanggilan dlsym dinamis.
        // Kita gunakan ".*" untuk mencakup library aplikasi, bukan libc.so
        api->pltHookRegister(".*", "__system_property_get",
                             (void*)my_system_property_get,
                             (void**)&orig_system_property_get);

        // Commit hooks
        if (api->pltHookCommit()) {
            LOGD("Hooks committed successfully");
        } else {
            LOGE("Failed to commit hooks");
        }
    }

private:
    Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MockPropsModule)