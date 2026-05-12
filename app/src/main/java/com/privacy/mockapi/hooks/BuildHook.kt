package com.privacy.mockapi.hooks

import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.XSharedPreferences

object BuildHook {

    fun applyHook(classLoader: ClassLoader, prefs: XSharedPreferences?, packageName: String) {
        val brand = prefs?.getString("BRAND", "Samsung") ?: "Samsung"
        val model = prefs?.getString("MODEL", "SM-G998B") ?: "SM-G998B"
        val device = prefs?.getString("DEVICE", "p3s") ?: "p3s"
        val fingerprint = prefs?.getString("FINGERPRINT", "samsung/p3sxx/p3s:13/TP1A.220624.014/G998BXXU9EWG1:user/release-keys") ?: "samsung/p3sxx/p3s:13/TP1A.220624.014/G998BXXU9EWG1:user/release-keys"
        val hardware = prefs?.getString("HARDWARE", "exynos2100") ?: "exynos2100"
        val manufacturer = prefs?.getString("MANUFACTURER", "samsung") ?: "samsung"
        val product = prefs?.getString("PRODUCT", "p3sxx") ?: "p3sxx"

        val mockedConstants = mapOf(
            "BRAND" to brand,
            "MODEL" to model,
            "DEVICE" to device,
            "FINGERPRINT" to fingerprint,
            "HARDWARE" to hardware,
            "MANUFACTURER" to manufacturer,
            "PRODUCT" to product
        )

        // Hook SystemProperties to fake the underlying values native code might query
        try {
            val systemPropertiesClass = XposedHelpers.findClass("android.os.SystemProperties", classLoader)

            XposedHelpers.findAndHookMethod(
                systemPropertiesClass,
                "get",
                String::class.java,
                object : XC_MethodHook() {
                    override fun afterHookedMethod(param: MethodHookParam) {
                        val key = param.args[0] as String
                        when (key) {
                            "ro.product.brand" -> param.result = mockedConstants["BRAND"]
                            "ro.product.model" -> param.result = mockedConstants["MODEL"]
                            "ro.product.device" -> param.result = mockedConstants["DEVICE"]
                            "ro.build.fingerprint" -> param.result = mockedConstants["FINGERPRINT"]
                            "ro.hardware" -> param.result = mockedConstants["HARDWARE"]
                            "ro.product.manufacturer" -> param.result = mockedConstants["MANUFACTURER"]
                            "ro.product.name" -> param.result = mockedConstants["PRODUCT"]
                        }
                    }
                }
            )

            XposedHelpers.findAndHookMethod(
                systemPropertiesClass,
                "get",
                String::class.java,
                String::class.java,
                object : XC_MethodHook() {
                    override fun afterHookedMethod(param: MethodHookParam) {
                        val key = param.args[0] as String
                        when (key) {
                            "ro.product.brand" -> param.result = mockedConstants["BRAND"]
                            "ro.product.model" -> param.result = mockedConstants["MODEL"]
                            "ro.product.device" -> param.result = mockedConstants["DEVICE"]
                            "ro.build.fingerprint" -> param.result = mockedConstants["FINGERPRINT"]
                            "ro.hardware" -> param.result = mockedConstants["HARDWARE"]
                            "ro.product.manufacturer" -> param.result = mockedConstants["MANUFACTURER"]
                            "ro.product.name" -> param.result = mockedConstants["PRODUCT"]
                        }
                    }
                }
            )

            // Only update Build fields directly if it's not a known PairIP/protected app
            // DevCheck uses libpairipcore which crashes with SEGV_MAPERR if we use setStaticObjectField on Build
            if (packageName != "flar2.devcheck" && !packageName.contains("shopee")) {
                val buildClass = XposedHelpers.findClass("android.os.Build", classLoader)
                for ((field, value) in mockedConstants) {
                    try {
                        XposedHelpers.setStaticObjectField(buildClass, field, value)
                    } catch (e: Throwable) {
                        // Ignore
                    }
                }
            }

        } catch (e: Exception) {
            // Fallback
        }
    }
}
