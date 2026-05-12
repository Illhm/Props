#!/system/bin/sh

# Generic App State Reset Script
# This script forcefully stops a target application and cleans its associated data and cache directories.
# It also attempts to clean common hidden directories on the internal storage based on the package name.

if [ -z "$1" ]; then
    echo "Usage: $0 <package_name>"
    echo "Example: $0 com.example.app"
    exit 1
fi

PACKAGE_NAME=$1

echo "[*] Targeting package: $PACKAGE_NAME"

# 1. Force stop the application
echo "[*] Force stopping $PACKAGE_NAME..."
am force-stop "$PACKAGE_NAME"
sleep 1

# 2. Clear standard app data and cache (Requires root/system privileges)
echo "[*] Clearing standard data for $PACKAGE_NAME..."
pm clear "$PACKAGE_NAME"

# 3. Aggressively remove residual directories in /data (Root only)
echo "[*] Removing residual data directories..."
rm -rf "/data/data/$PACKAGE_NAME"
rm -rf "/data/user/0/$PACKAGE_NAME"
rm -rf "/data/user_de/0/$PACKAGE_NAME"

# 4. Clean standard external storage paths
echo "[*] Cleaning external storage paths..."
rm -rf "/sdcard/Android/data/$PACKAGE_NAME"
rm -rf "/sdcard/Android/media/$PACKAGE_NAME"
rm -rf "/sdcard/Android/obb/$PACKAGE_NAME"

# 5. Clean common hidden/dot directories on sdcard
# Extracts the last part of the package name to look for loose hidden directories
# e.g., com.example.app -> .app
SHORT_NAME=$(echo "$PACKAGE_NAME" | awk -F. '{print $NF}')

echo "[*] Searching for potential hidden directories on internal storage..."
if [ -n "$SHORT_NAME" ]; then
    # Look for specific hidden directories related to the package
    if [ -d "/sdcard/.$SHORT_NAME" ]; then
        echo "[!] Found hidden directory: /sdcard/.$SHORT_NAME"
        rm -rf "/sdcard/.$SHORT_NAME"
    fi

    # Also look for the full package name as a hidden directory
    if [ -d "/sdcard/.$PACKAGE_NAME" ]; then
        echo "[!] Found hidden directory: /sdcard/.$PACKAGE_NAME"
        rm -rf "/sdcard/.$PACKAGE_NAME"
    fi
fi

echo "[*] Cleanup process for $PACKAGE_NAME completed."
