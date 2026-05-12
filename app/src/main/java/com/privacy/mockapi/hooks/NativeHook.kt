package com.privacy.mockapi.hooks

import android.content.Context
import java.io.File
import java.io.FileOutputStream
import java.io.InputStream
import de.robv.android.xposed.XposedBridge

object NativeHook {

    /**
     * Initializes the native hooking engine (e.g. Dobby) and passes the spoofed
     * device properties to the C++ layer.
     */
    external fun initNativeHook(
        brand: String,
        model: String,
        device: String,
        fingerprint: String,
        hardware: String,
        manufacturer: String,
        product: String
    )

    /**
     * Loads a native library from the APK assets or direct path when running inside the hooked app's process.
     * Standard System.loadLibrary() fails in Xposed modules because the classloader doesn't have the module's nativeLibraryDir.
     */
    fun loadLibrarySafely(context: Context?, apkPath: String) {
        try {
            // Option 1: If we have the APK path (provided by lpparam.appInfo.sourceDir or similar for our module)
            // It's often safer to load the uncompressed .so from the APK directly (if extractNativeLibs="false" or API 23+)
            // However, a common fallback in LSPosed is just loading the path directly.

            // XposedBridge doesn't officially expose the module path, but we can construct it if we know where we are
            // Since we don't have standard context to our own module here, we load it if we know the path.

            // Modern LSPosed maps the module's native libs into the classloader, so standard loadLibrary sometimes works:
            System.loadLibrary("native-lib")
        } catch (e: UnsatisfiedLinkError) {
            XposedBridge.log("MockAPI: Failed to load native-lib via System.loadLibrary: ${e.message}")
            // Fallback: Copy .so from assets (if we packed it there) or load from absolute path.
        }
    }
}
