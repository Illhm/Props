#include <jni.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dlfcn.h>
#include <map>
#include <string>
#include <vector>
#include <sys/system_properties.h>

#include "zygisk.hpp"

// Placeholder for hook library like xhook, Dobby, or LSPlant
// In real implementation, this would use a robust hooking framework
void* DobbyHook(void* address, void* replace_call, void** origin_call) {
    // Mock hooking logic
    return nullptr;
}

// Global map to store mock properties fetched from companion
std::map<std::string, std::string> mock_props;

// Original function pointers
static int (*orig_system_property_get)(const char *name, char *value) = nullptr;
static void (*orig_system_property_read_callback)(const prop_info *pi,
                                                  void (*callback)(void *cookie, const char *name,
                                                                   const char *value, uint32_t serial),
                                                  void *cookie) = nullptr;

// Hook for __system_property_get
int my_system_property_get(const char *name, char *value) {
    if (name != nullptr) {
        auto it = mock_props.find(name);
        if (it != mock_props.end()) {
            strcpy(value, it->second.c_str());
            return it->second.length();
        }
    }
    if (orig_system_property_get) {
        return orig_system_property_get(name, value);
    }
    return 0; // Fallback
}

// Hook for __system_property_read_callback
void my_system_property_read_callback(const prop_info *pi,
                                      void (*callback)(void *cookie, const char *name, const char *value, uint32_t serial),
                                      void *cookie) {
    // In a full implementation, you would resolve pi to name first.
    // For this template, we assume standard structure or redirect completely.
    // Note: implementing pi to name resolution safely requires parsing prop_info struct.

    // Call original for now (skeleton template)
    if (orig_system_property_read_callback) {
        orig_system_property_read_callback(pi, callback, cookie);
    }
}

// Setup hooking
void ApplyHooks() {
    void* handle = dlopen("libc.so", RTLD_NOW);
    if (handle) {
        void* sym_get = dlsym(handle, "__system_property_get");
        if (sym_get) {
            DobbyHook(sym_get, (void*)my_system_property_get, (void**)&orig_system_property_get);
        }

        void* sym_read = dlsym(handle, "__system_property_read_callback");
        if (sym_read) {
            DobbyHook(sym_read, (void*)my_system_property_read_callback, (void**)&orig_system_property_read_callback);
        }
        dlclose(handle);
    }
}

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Request companion to read custom_props.txt
        int fd = api->connectCompanion();
        if (fd >= 0) {
            std::string content;
            char buffer[4096];
            ssize_t bytesRead;
            while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytesRead] = '\0';
                content += buffer;
            }
            if (!content.empty()) {
                ParseProps(content.c_str());
            }
            close(fd);
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Only apply hooks if we have props to mock and process is not isolated
        if (!mock_props.empty() && (args->uid % 100000 >= 10000)) {
            ApplyHooks();
        }
    }

private:
    zygisk::Api *api;
    JNIEnv *env;

    void ParseProps(const char* data) {
        std::string content(data);
        size_t start = 0;
        size_t end = content.find('\n');

        while (end != std::string::npos) {
            std::string line = content.substr(start, end - start);
            if (!line.empty() && line[0] != '#') {
                size_t delim = line.find('=');
                if (delim != std::string::npos) {
                    std::string key = line.substr(0, delim);
                    std::string val = line.substr(delim + 1);
                    mock_props[key] = val;
                }
            }
            start = end + 1;
            end = content.find('\n', start);
        }
    }
};

// Companion process runs as root
static void companion_handler(int client_fd) {
    int fd = open("/data/adb/modules/xkatrina_snstv_prps/custom_props.txt", O_RDONLY);
    if (fd >= 0) {
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
            write(client_fd, buffer, bytesRead);
        }
        close(fd);
    }
    close(client_fd);
}

REGISTER_ZYGISK_MODULE(MyModule)
REGISTER_ZYGISK_COMPANION(companion_handler)
