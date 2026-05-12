package com.privacy.mockapi.utils

import kotlin.random.Random

object MockDataGenerator {

    fun generateRandomHex(length: Int): String {
        val charPool = "0123456789abcdef"
        return (1..length)
            .map { Random.nextInt(0, charPool.length) }
            .map(charPool::get)
            .joinToString("")
    }

    fun generateAndroidId(): String {
        return generateRandomHex(16)
    }

    fun generateNumericString(length: Int): String {
        return (1..length)
            .map { Random.nextInt(0, 10).toString() }
            .joinToString("")
    }

    fun generateImei(): String {
        // IMEI is usually 15 digits long
        return generateNumericString(15)
    }

    fun generateMeid(): String {
        // MEID is usually 14 hex characters
        return generateRandomHex(14)
    }

    fun generateSubscriberId(): String {
        // IMSI is usually 15 digits long
        return generateNumericString(15)
    }
}
