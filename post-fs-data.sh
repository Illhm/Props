#!/system/bin/sh

MODDIR="${0%/*}"
MODNAME="${MODDIR##*/}"
MAGISKTMP="$(magisk --path)" || MAGISKTMP=/sbin

if [ "$(magisk -V)" -lt 26302 ] && [ -z "$(/data/adb/ksud -V)" ] && [ -z "$(/data/adb/apatch -V)" ]; then
  touch "$MODDIR/disable"
fi

if [ ! -e "$MAGISKTMP/.magisk/mirror/sepolicy.rules/$MODNAME/sepolicy.rule" ] && [ ! -e "$MAGISKTMP/.magisk/sepolicy.rules/$MODNAME/sepolicy.rule" ]; then
    magiskpolicy --live --apply "$MODDIR/sepolicy.rule"
    ksud sepolicy apply "$MODDIR/sepolicy.rule" 2>/dev/null
    apatch sepolicy apply "$MODDIR/sepolicy.rule" 2>/dev/null
fi

. "$MODDIR/resetprop.sh"

PROFILE_FILE="$MODDIR/current_profile.txt"
PROFILES_LIST="$MODDIR/common/profiles.list"

# Generate a random profile if none exists or if it's empty
if [ ! -f "$PROFILE_FILE" ] || [ ! -s "$PROFILE_FILE" ]; then
    grep -v "^#" "$PROFILES_LIST" | shuf -n 1 > "$PROFILE_FILE"
fi

IFS='|' read -r MANUFACTURER BRAND MODEL NAME DEVICE FINGERPRINT DESCRIPTION HARDWARE BOARD ID < "$PROFILE_FILE"

# Random Serial Number Generation
NEW_SERIAL=$(cat /dev/urandom | tr -dc 'A-Z0-9' | fold -w 12 | head -n 1)
check_resetprop ro.serialno "$NEW_SERIAL"
check_resetprop ro.boot.serialno "$NEW_SERIAL"

# Spoof core properties across all partitions
for prefix in "" bootimage. odm. odm_dlkm. product. system. system_ext. vendor. vendor_dlkm.; do
    check_resetprop "ro.${prefix}build.fingerprint" "$FINGERPRINT"
    check_resetprop "ro.${prefix}build.id" "$ID"
    check_resetprop "ro.${prefix}build.tags" "release-keys"
    check_resetprop "ro.${prefix}build.type" "user"
    replace_value_resetprop "ro.${prefix}build.description" "test-keys" "release-keys"
    replace_value_resetprop "ro.${prefix}build.description" "userdebug" "user"
done

# Device specific props
for prefix in "" bootimage. odm. odm_dlkm. product. system. system_ext. vendor. vendor_dlkm.; do
    check_resetprop "ro.product.${prefix}brand" "$BRAND"
    check_resetprop "ro.product.${prefix}device" "$DEVICE"
    check_resetprop "ro.product.${prefix}manufacturer" "$MANUFACTURER"
    check_resetprop "ro.product.${prefix}model" "$MODEL"
    check_resetprop "ro.product.${prefix}name" "$NAME"
done

check_resetprop ro.build.description "$DESCRIPTION"
check_resetprop ro.build.display.id "$ID"
check_resetprop ro.build.host "builder"
check_resetprop ro.build.user "builder"
check_resetprop ro.hardware "$HARDWARE"
check_resetprop ro.product.board "$BOARD"

# Fix Lineage and Debugging props
replace_value_resetprop ro.build.description "aosp_" ""
replace_value_resetprop ro.build.fingerprint "aosp_" ""
replace_value_resetprop ro.build.flavor "aosp_" ""
replace_value_resetprop ro.product.bootimage.name "aosp_" ""
replace_value_resetprop ro.product.name "aosp_" ""

replace_value_resetprop ro.build.description "lineage_" ""
replace_value_resetprop ro.build.fingerprint "lineage_" ""
replace_value_resetprop ro.build.flavor "lineage_" ""
replace_value_resetprop ro.product.bootimage.name "lineage_" ""
replace_value_resetprop ro.product.name "lineage_" ""

replace_value_resetprop ro.build.description test-keys release-keys
replace_value_resetprop ro.build.description userdebug user
replace_value_resetprop ro.build.fingerprint test-keys release-keys
replace_value_resetprop ro.build.fingerprint userdebug user
replace_value_resetprop ro.build.flavor userdebug user
