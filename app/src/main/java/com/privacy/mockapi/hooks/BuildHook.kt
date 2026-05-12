package com.privacy.mockapi.hooks

import de.robv.android.xposed.XposedHelpers

object BuildHook {

    private val mockedConstants = mapOf(
        "BRAND" to "MockedBrand",
        "MODEL" to "MockedModel",
        "DEVICE" to "MockedDevice",
        "FINGERPRINT" to "Mocked/Fingerprint/String:user/release-keys",
        "HARDWARE" to "MockedHardware",
        "MANUFACTURER" to "MockedManufacturer",
        "PRODUCT" to "MockedProduct"
    )

    fun applyHook(classLoader: ClassLoader) {
        val buildClass = XposedHelpers.findClass("android.os.Build", classLoader)

        for ((field, value) in mockedConstants) {
            try {
                XposedHelpers.setStaticObjectField(buildClass, field, value)
            } catch (e: Exception) {
                // Ignore if specific field cannot be hooked
            }
        }
    }
}
