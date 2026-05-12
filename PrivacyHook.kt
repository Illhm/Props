package com.example.privacyhook

import android.telephony.TelephonyManager
import android.net.wifi.WifiInfo
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage

class PrivacyHook : IXposedHookLoadPackage {

    // Define the specific package you want to target, or leave empty/modify logic to target all
    private val TARGET_PACKAGE = "com.example.targetapp"

    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam) {

        // 1. Restrict hooking to the target package
        if (lpparam.packageName != TARGET_PACKAGE) {
            return
        }

        XposedBridge.log("PrivacyHook: Injected into ${lpparam.packageName}")

        try {
            // 2. Hook TelephonyManager to mock IMEI
            XposedHelpers.findAndHookMethod(
                TelephonyManager::class.java.name,
                lpparam.classLoader,
                "getImei",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        // Return a mocked/empty IMEI
                        param.result = "000000000000000" // Or null, depending on desired behavior
                        XposedBridge.log("PrivacyHook: Mocked TelephonyManager.getImei()")
                    }
                }
            )

            // Also hook getDeviceId which is often used interchangeably or on older APIs
            XposedHelpers.findAndHookMethod(
                TelephonyManager::class.java.name,
                lpparam.classLoader,
                "getDeviceId",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = "000000000000000"
                        XposedBridge.log("PrivacyHook: Mocked TelephonyManager.getDeviceId()")
                    }
                }
            )

        } catch (e: Throwable) {
            XposedBridge.log("PrivacyHook: Error hooking TelephonyManager - ${e.message}")
        }

        try {
            // 3. Hook WifiInfo to mock MAC Address
            XposedHelpers.findAndHookMethod(
                WifiInfo::class.java.name,
                lpparam.classLoader,
                "getMacAddress",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        // Return a mocked MAC address
                        param.result = "02:00:00:00:00:00"
                        XposedBridge.log("PrivacyHook: Mocked WifiInfo.getMacAddress()")
                    }
                }
            )
        } catch (e: Throwable) {
            XposedBridge.log("PrivacyHook: Error hooking WifiInfo - ${e.message}")
        }
    }
}
