#pragma once
#include <jni.h>

#define EXPORT __attribute__((visibility("default")))

namespace zygisk {
struct AppSpecializeArgs {
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jobjectArray &rlimits;
    jint &mount_external;
    jstring &se_info;
    jstring &nice_name;
    jintArray &instruction_set;
    jstring &app_data_dir;
};

struct ServerSpecializeArgs {
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jlong &permitted_capabilities;
    jlong &effective_capabilities;
};

class Api {
public:
    virtual bool connectCompanion() = 0;
    virtual int getModuleDir() = 0;
    virtual void setOption(int option) = 0;
    virtual void setOption(int option, bool value) = 0;
    virtual void pltHookRegister(dev_t dev, ino_t inode, const char *symbol, void *new_func, void **old_func) = 0;
    virtual bool pltHookCommit() = 0;
};

class ModuleBase {
public:
    virtual ~ModuleBase() = default;
    virtual void onLoad(Api *api, JNIEnv *env) {}
    virtual void preAppSpecialize(AppSpecializeArgs *args) {}
    virtual void postAppSpecialize(const AppSpecializeArgs *args) {}
    virtual void preServerSpecialize(ServerSpecializeArgs *args) {}
    virtual void postServerSpecialize(const ServerSpecializeArgs *args) {}
};

} // namespace zygisk

#define REGISTER_ZYGISK_MODULE(clazz) \
    extern "C" EXPORT void zygisk_module_entry(zygisk::Api *api, JNIEnv *env) { \
        auto *module = new clazz(); \
        module->onLoad(api, env); \
    }

#define REGISTER_ZYGISK_COMPANION(func) \
    extern "C" EXPORT void zygisk_companion_entry(int client_fd) { \
        func(client_fd); \
    }
