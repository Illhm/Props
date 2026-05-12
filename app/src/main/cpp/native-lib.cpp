#include <jni.h>
#include <string>
#include <android/log.h>
#include <sys/system_properties.h>

#define LOG_TAG "MockAPI_Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Mock implementations for Dobby resolver to pass compilation since we don't have Dobby linked here.
// In a real environment, you'd statically link libdobby.a or dynamically link libdobby.so.
extern "C" {
    void* DobbySymbolResolver(const char* image_name, const char* symbol_name) {
        return nullptr; // Placeholder
    }
    int DobbyHook(void* address, void* replace_call, void** origin_call) {
        return 0; // Placeholder
    }
}

// Spoofed values
std::string spoofed_brand;
std::string spoofed_model;
std::string spoofed_device;
std::string spoofed_fingerprint;
std::string spoofed_hardware;
std::string spoofed_manufacturer;
std::string spoofed_product;

// Original function pointers
static int (*orig_system_property_get)(const char *name, char *value) = nullptr;
static void (*orig_system_property_read_callback)(const prop_info *pi, void (*callback)(void *cookie, const char *name, const char *value, uint32_t serial), void *cookie) = nullptr;

// Hook for __system_property_get
int my_system_property_get(const char *name, char *value) {
    if (!name) return orig_system_property_get ? orig_system_property_get(name, value) : 0;

    std::string key(name);
    std::string spoofed_val;

    if (key == "ro.product.brand") spoofed_val = spoofed_brand;
    else if (key == "ro.product.model") spoofed_val = spoofed_model;
    else if (key == "ro.product.device") spoofed_val = spoofed_device;
    else if (key == "ro.build.fingerprint") spoofed_val = spoofed_fingerprint;
    else if (key == "ro.hardware") spoofed_val = spoofed_hardware;
    else if (key == "ro.product.manufacturer") spoofed_val = spoofed_manufacturer;
    else if (key == "ro.product.name") spoofed_val = spoofed_product;

    if (!spoofed_val.empty()) {
        LOGI("Spoofing __system_property_get for %s -> %s", name, spoofed_val.c_str());
        strcpy(value, spoofed_val.c_str());
        return spoofed_val.length();
    }

    return orig_system_property_get ? orig_system_property_get(name, value) : 0;
}

// Struct to pass original callback and cookie
struct CallbackBaton {
    void (*orig_callback)(void *cookie, const char *name, const char *value, uint32_t serial);
    void *orig_cookie;
};

// Our interceptor callback
void my_read_callback(void *cookie, const char *name, const char *value, uint32_t serial) {
    CallbackBaton *baton = static_cast<CallbackBaton*>(cookie);
    if (!name) {
        baton->orig_callback(baton->orig_cookie, name, value, serial);
        return;
    }

    std::string key(name);
    std::string spoofed_val;

    if (key == "ro.product.brand") spoofed_val = spoofed_brand;
    else if (key == "ro.product.model") spoofed_val = spoofed_model;
    else if (key == "ro.product.device") spoofed_val = spoofed_device;
    else if (key == "ro.build.fingerprint") spoofed_val = spoofed_fingerprint;
    else if (key == "ro.hardware") spoofed_val = spoofed_hardware;
    else if (key == "ro.product.manufacturer") spoofed_val = spoofed_manufacturer;
    else if (key == "ro.product.name") spoofed_val = spoofed_product;

    if (!spoofed_val.empty()) {
        LOGI("Spoofing __system_property_read_callback for %s -> %s", name, spoofed_val.c_str());
        baton->orig_callback(baton->orig_cookie, name, spoofed_val.c_str(), serial);
    } else {
        baton->orig_callback(baton->orig_cookie, name, value, serial);
    }
}

// Hook for __system_property_read_callback
void my_system_property_read_callback(const prop_info *pi, void (*callback)(void *cookie, const char *name, const char *value, uint32_t serial), void *cookie) {
    CallbackBaton baton = {callback, cookie};
    if (orig_system_property_read_callback) {
        orig_system_property_read_callback(pi, my_read_callback, &baton);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_privacy_mockapi_hooks_NativeHook_initNativeHook(
        JNIEnv* env,
        jobject /* this */,
        jstring brand,
        jstring model,
        jstring device,
        jstring fingerprint,
        jstring hardware,
        jstring manufacturer,
        jstring product) {

    const char* c_brand = env->GetStringUTFChars(brand, nullptr);
    spoofed_brand = c_brand;
    env->ReleaseStringUTFChars(brand, c_brand);

    const char* c_model = env->GetStringUTFChars(model, nullptr);
    spoofed_model = c_model;
    env->ReleaseStringUTFChars(model, c_model);

    const char* c_device = env->GetStringUTFChars(device, nullptr);
    spoofed_device = c_device;
    env->ReleaseStringUTFChars(device, c_device);

    const char* c_fingerprint = env->GetStringUTFChars(fingerprint, nullptr);
    spoofed_fingerprint = c_fingerprint;
    env->ReleaseStringUTFChars(fingerprint, c_fingerprint);

    const char* c_hardware = env->GetStringUTFChars(hardware, nullptr);
    spoofed_hardware = c_hardware;
    env->ReleaseStringUTFChars(hardware, c_hardware);

    const char* c_manufacturer = env->GetStringUTFChars(manufacturer, nullptr);
    spoofed_manufacturer = c_manufacturer;
    env->ReleaseStringUTFChars(manufacturer, c_manufacturer);

    const char* c_product = env->GetStringUTFChars(product, nullptr);
    spoofed_product = c_product;
    env->ReleaseStringUTFChars(product, c_product);

    LOGI("Initializing Dobby Native Hooks for system properties...");

    // Hook __system_property_get
    void *sys_prop_get = DobbySymbolResolver("libc.so", "__system_property_get");
    if (sys_prop_get) {
        DobbyHook(sys_prop_get, (void *)my_system_property_get, (void **)&orig_system_property_get);
        LOGI("Hooked __system_property_get successfully.");
    }

    // Hook __system_property_read_callback (Android 8.0+)
    void *sys_prop_read_cb = DobbySymbolResolver("libc.so", "__system_property_read_callback");
    if (sys_prop_read_cb) {
        DobbyHook(sys_prop_read_cb, (void *)my_system_property_read_callback, (void **)&orig_system_property_read_callback);
        LOGI("Hooked __system_property_read_callback successfully.");
    }
}
