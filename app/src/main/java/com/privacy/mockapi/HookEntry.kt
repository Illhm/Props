package com.privacy.mockapi

import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.callbacks.XC_LoadPackage.LoadPackageParam
import de.robv.android.xposed.XSharedPreferences
import com.privacy.mockapi.hooks.NativeHook
import com.privacy.mockapi.hooks.SettingsHook
import com.privacy.mockapi.hooks.TelephonyHook

class HookEntry : IXposedHookLoadPackage {

    override fun handleLoadPackage(lpparam: LoadPackageParam) {
        // Since we want this to be a privacy module for any selected app, we shouldn't hardcode target packages.
        // LSPosed takes care of the scope for us. The xposedscope array in manifest is just the default.
        // But to prevent issues with system apps, we can filter out android.
        if (lpparam.packageName == "android" || lpparam.packageName == "com.privacy.mockapi") {
            return
        }

        XposedBridge.log("MockAPI: Hooking into targeted app: ${lpparam.packageName}")

        val prefs = XSharedPreferences("com.privacy.mockapi", "MockApiPrefs")
        prefs.makeWorldReadable()

        // 1. NATIVE HOOK: Universal untuk semua aplikasi (Bypass SystemProperties)
        try {
            NativeHook.loadLibrarySafely(null, lpparam.appInfo.sourceDir)
            NativeHook.initNativeHook(
                prefs.getString("BRAND", "Samsung") ?: "Samsung",
                prefs.getString("MODEL", "SM-G998B") ?: "SM-G998B",
                prefs.getString("DEVICE", "p3s") ?: "p3s",
                prefs.getString("FINGERPRINT", "samsung/p3sxx/p3s:13/TP1A.220624.014/G998BXXU9EWG1:user/release-keys") ?: "samsung/p3sxx/p3s:13/TP1A.220624.014/G998BXXU9EWG1:user/release-keys",
                prefs.getString("HARDWARE", "exynos2100") ?: "exynos2100",
                prefs.getString("MANUFACTURER", "samsung") ?: "samsung",
                prefs.getString("PRODUCT", "p3sxx") ?: "p3sxx"
            )
        } catch (e: Exception) {
            XposedBridge.log("MockAPI: Failed to initialize NativeHook: ${e.message}")
        }

        // 2. JAVA HOOK: Skip khusus flar2.devcheck agar tidak memicu libpairipcore.so (SEGV_ACCERR)
        if (lpparam.packageName != "flar2.devcheck") {
            try {
                SettingsHook.applyHook(lpparam.classLoader, prefs)
            } catch (e: Exception) {
                XposedBridge.log("MockAPI: Failed to hook Settings.Secure: ${e.message}")
            }

            try {
                TelephonyHook.applyHook(lpparam.classLoader, prefs)
            } catch (e: Exception) {
                XposedBridge.log("MockAPI: Failed to hook TelephonyManager: ${e.message}")
            }
        } else {
            XposedBridge.log("MockAPI: Skipping Settings and Telephony hooks for ${lpparam.packageName} to prevent libpairipcore crash.")
        }
    }
}
