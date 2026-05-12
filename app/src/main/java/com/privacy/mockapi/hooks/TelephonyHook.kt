package com.privacy.mockapi.hooks

import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.XSharedPreferences

object TelephonyHook {

    fun applyHook(classLoader: ClassLoader, prefs: XSharedPreferences?) {
        val mockedImei = prefs?.getString("IMEI", "351234567890123") ?: "351234567890123"
        val mockedMeid = prefs?.getString("MEID", "99000000000000") ?: "99000000000000"
        val mockedImsi = prefs?.getString("IMSI", "310120123456789") ?: "310120123456789"

        val telephonyManagerClass = XposedHelpers.findClass("android.telephony.TelephonyManager", classLoader)

        try {
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass, "getImei",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) { param.result = mockedImei }
                }
            )
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass, "getImei", Int::class.java,
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) { param.result = mockedImei }
                }
            )
        } catch (e: NoSuchMethodError) {}

        try {
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass, "getDeviceId",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) { param.result = mockedImei }
                }
            )
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass, "getDeviceId", Int::class.java,
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) { param.result = mockedImei }
                }
            )
        } catch (e: NoSuchMethodError) {}

        try {
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass, "getMeid",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) { param.result = mockedMeid }
                }
            )
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass, "getMeid", Int::class.java,
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) { param.result = mockedMeid }
                }
            )
        } catch (e: NoSuchMethodError) {}

        try {
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass, "getSubscriberId",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) { param.result = mockedImsi }
                }
            )
        } catch (e: NoSuchMethodError) {}
    }
}
