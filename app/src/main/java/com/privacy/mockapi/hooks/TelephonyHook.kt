package com.privacy.mockapi.hooks

import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedHelpers
import com.privacy.mockapi.utils.MockDataGenerator

object TelephonyHook {

    fun applyHook(classLoader: ClassLoader) {
        val telephonyManagerClass = XposedHelpers.findClass("android.telephony.TelephonyManager", classLoader)

        // Hook getImei
        try {
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass,
                "getImei",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = MockDataGenerator.generateImei()
                    }
                }
            )

            XposedHelpers.findAndHookMethod(
                telephonyManagerClass,
                "getImei",
                Int::class.java,
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = MockDataGenerator.generateImei()
                    }
                }
            )
        } catch (e: NoSuchMethodError) {
            // Ignore if method is not present in the current API level
        }

        // Hook getDeviceId
        try {
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass,
                "getDeviceId",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = MockDataGenerator.generateImei() // Usually IMEI
                    }
                }
            )

            XposedHelpers.findAndHookMethod(
                telephonyManagerClass,
                "getDeviceId",
                Int::class.java,
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = MockDataGenerator.generateImei()
                    }
                }
            )
        } catch (e: NoSuchMethodError) {
            // Ignore
        }

        // Hook getMeid
        try {
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass,
                "getMeid",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = MockDataGenerator.generateMeid()
                    }
                }
            )

            XposedHelpers.findAndHookMethod(
                telephonyManagerClass,
                "getMeid",
                Int::class.java,
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = MockDataGenerator.generateMeid()
                    }
                }
            )
        } catch (e: NoSuchMethodError) {
            // Ignore
        }

        // Hook getSubscriberId (IMSI)
        try {
            XposedHelpers.findAndHookMethod(
                telephonyManagerClass,
                "getSubscriberId",
                object : XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = MockDataGenerator.generateSubscriberId()
                    }
                }
            )
        } catch (e: NoSuchMethodError) {
            // Ignore
        }
    }
}
