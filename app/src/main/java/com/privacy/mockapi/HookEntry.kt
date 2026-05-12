package com.privacy.mockapi

import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.callbacks.XC_LoadPackage.LoadPackageParam
import de.robv.android.xposed.XSharedPreferences
import com.privacy.mockapi.hooks.BuildHook
import com.privacy.mockapi.hooks.SettingsHook
import com.privacy.mockapi.hooks.TelephonyHook

class HookEntry : IXposedHookLoadPackage {

    private val targetPackages = listOf("com.example.targetapp", "flar2.devcheck")

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

        try {
            // For flar2.devcheck which uses libpairipcore.so, modifying Build fields directly via setStaticObjectField causes a crash.
            // We pass the package name so BuildHook can decide whether to use setStaticObjectField or only SystemProperties.
            BuildHook.applyHook(lpparam.classLoader, prefs, lpparam.packageName)
        } catch (e: Exception) {
            XposedBridge.log("MockAPI: Failed to hook android.os.Build: ${e.message}")
        }

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
    }
}
