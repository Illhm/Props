package com.privacy.mockapi

import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.callbacks.XC_LoadPackage.LoadPackageParam
import com.privacy.mockapi.hooks.BuildHook
import com.privacy.mockapi.hooks.SettingsHook
import com.privacy.mockapi.hooks.TelephonyHook

class HookEntry : IXposedHookLoadPackage {

    private val targetPackages = listOf("com.example.targetapp")

    override fun handleLoadPackage(lpparam: LoadPackageParam) {
        if (!targetPackages.contains(lpparam.packageName)) {
            return
        }

        XposedBridge.log("MockAPI: Hooking into targeted app: ${lpparam.packageName}")

        try {
            BuildHook.applyHook(lpparam.classLoader)
        } catch (e: Exception) {
            XposedBridge.log("MockAPI: Failed to hook android.os.Build: ${e.message}")
        }

        try {
            SettingsHook.applyHook(lpparam.classLoader)
        } catch (e: Exception) {
            XposedBridge.log("MockAPI: Failed to hook Settings.Secure: ${e.message}")
        }

        try {
            TelephonyHook.applyHook(lpparam.classLoader)
        } catch (e: Exception) {
            XposedBridge.log("MockAPI: Failed to hook TelephonyManager: ${e.message}")
        }
    }
}
