package com.privacy.mockapi.hooks

import android.content.ContentResolver
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.XSharedPreferences
import android.provider.Settings

object SettingsHook {

    fun applyHook(classLoader: ClassLoader, prefs: XSharedPreferences?) {
        val mockedAndroidId = prefs?.getString("ANDROID_ID", "1234567890abcdef") ?: "1234567890abcdef"

        val settingsSecureClass = XposedHelpers.findClass("android.provider.Settings\$Secure", classLoader)

        XposedHelpers.findAndHookMethod(
            settingsSecureClass,
            "getString",
            ContentResolver::class.java,
            String::class.java,
            object : XC_MethodHook() {
                override fun beforeHookedMethod(param: MethodHookParam) {
                    val name = param.args[1] as? String
                    if (name == Settings.Secure.ANDROID_ID) {
                        param.result = mockedAndroidId
                    }
                }
            }
        )
    }
}
