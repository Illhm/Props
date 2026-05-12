package com.privacy.mockapi

import android.content.Context
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.privacy.mockapi.utils.MockDataGenerator
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var tvCurrentProps: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        tvCurrentProps = findViewById(R.id.tvCurrentProps)

        findViewById<Button>(R.id.btnGenerateSamsung).setOnClickListener {
            generateSamsungProps()
        }

        findViewById<Button>(R.id.btnGenerateRandomAll).setOnClickListener {
            generateRandomProps()
        }

        findViewById<Button>(R.id.btnGenerateDeviceInfo).setOnClickListener {
            generateDeviceInfo()
        }

        updateDisplay()
    }

    // Making SharedPreferences world-readable is restricted in modern Android.
    // Instead, LSPosed injects and redirects the prefs for us, but it requires
    // us to create a world-readable file or use MODE_WORLD_READABLE (deprecated).
    // Using Context.MODE_WORLD_READABLE may throw SecurityException in API >= 24,
    // so we set read/write permissions directly on the file if needed.
    private fun getSharedPrefs() = getSharedPreferences("MockApiPrefs", Context.MODE_PRIVATE)

    private fun fixPrefsPermissions() {
        try {
            val file = File(applicationContext.applicationInfo.dataDir, "shared_prefs/MockApiPrefs.xml")
            if (file.exists()) {
                file.setReadable(true, false)
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    private fun generateSamsungProps() {
        val prefs = getSharedPrefs().edit()

        // Randomly pick a realistic Samsung profile
        val profiles = listOf(
            mapOf("BRAND" to "samsung", "MODEL" to "SM-G998B", "DEVICE" to "p3s", "PRODUCT" to "p3sxx", "HARDWARE" to "exynos2100", "MANUFACTURER" to "samsung", "FINGERPRINT" to "samsung/p3sxx/p3s:13/TP1A.220624.014/G998BXXU9EWG1:user/release-keys"),
            mapOf("BRAND" to "samsung", "MODEL" to "SM-S918B", "DEVICE" to "dm3q", "PRODUCT" to "dm3qxxx", "HARDWARE" to "qcom", "MANUFACTURER" to "samsung", "FINGERPRINT" to "samsung/dm3qxxx/dm3q:14/UP1A.231005.007/S918BXXS3BWK5:user/release-keys"),
            mapOf("BRAND" to "samsung", "MODEL" to "SM-A546E", "DEVICE" to "a54x", "PRODUCT" to "a54xedx", "HARDWARE" to "s5e8835", "MANUFACTURER" to "samsung", "FINGERPRINT" to "samsung/a54xedx/a54x:13/TP1A.220624.014/A546EXXU5AWGJ:user/release-keys")
        )

        val profile = profiles.random()

        for ((k, v) in profile) {
            prefs.putString(k, v)
        }
        prefs.apply()
        fixPrefsPermissions()
        Toast.makeText(this, "Samsung Props Generated", Toast.LENGTH_SHORT).show()
        updateDisplay()
    }

    private fun generateRandomProps() {
        val prefs = getSharedPrefs().edit()

        // Generic random devices
        val profiles = listOf(
            mapOf("BRAND" to "google", "MODEL" to "Pixel 7 Pro", "DEVICE" to "cheetah", "PRODUCT" to "cheetah", "HARDWARE" to "gs201", "MANUFACTURER" to "Google", "FINGERPRINT" to "google/cheetah/cheetah:13/TQ3A.230901.001/10750268:user/release-keys"),
            mapOf("BRAND" to "OnePlus", "MODEL" to "KB2003", "DEVICE" to "kebab", "PRODUCT" to "OnePlus8T_EEA", "HARDWARE" to "qcom", "MANUFACTURER" to "OnePlus", "FINGERPRINT" to "OnePlus/OnePlus8T_EEA/kebab:11/RP1A.201005.001/2011132215:user/release-keys")
        )

        val profile = profiles.random()

        for ((k, v) in profile) {
            prefs.putString(k, v)
        }
        prefs.apply()
        fixPrefsPermissions()
        Toast.makeText(this, "Random Device Props Generated", Toast.LENGTH_SHORT).show()
        updateDisplay()
    }

    private fun generateDeviceInfo() {
        val prefs = getSharedPrefs().edit()
        prefs.putString("ANDROID_ID", MockDataGenerator.generateAndroidId())
        prefs.putString("IMEI", MockDataGenerator.generateImei())
        prefs.putString("MEID", MockDataGenerator.generateMeid())
        prefs.putString("IMSI", MockDataGenerator.generateSubscriberId())
        prefs.apply()
        fixPrefsPermissions()
        Toast.makeText(this, "Device Info Generated", Toast.LENGTH_SHORT).show()
        updateDisplay()
    }

    private fun updateDisplay() {
        val prefs = getSharedPrefs()
        val sb = StringBuilder()
        sb.append("--- BUILD PROPS ---\n")
        sb.append("BRAND: ${prefs.getString("BRAND", "Not Set")}\n")
        sb.append("MODEL: ${prefs.getString("MODEL", "Not Set")}\n")
        sb.append("DEVICE: ${prefs.getString("DEVICE", "Not Set")}\n")
        sb.append("PRODUCT: ${prefs.getString("PRODUCT", "Not Set")}\n")
        sb.append("HARDWARE: ${prefs.getString("HARDWARE", "Not Set")}\n")
        sb.append("MANUFACTURER: ${prefs.getString("MANUFACTURER", "Not Set")}\n")
        sb.append("FINGERPRINT: ${prefs.getString("FINGERPRINT", "Not Set")}\n")

        sb.append("\n--- DEVICE INFO ---\n")
        sb.append("ANDROID_ID: ${prefs.getString("ANDROID_ID", "Not Set")}\n")
        sb.append("IMEI: ${prefs.getString("IMEI", "Not Set")}\n")
        sb.append("MEID: ${prefs.getString("MEID", "Not Set")}\n")
        sb.append("IMSI: ${prefs.getString("IMSI", "Not Set")}\n")

        tvCurrentProps.text = sb.toString()
    }
}
