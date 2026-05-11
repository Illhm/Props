#!/system/bin/sh

MAGISKTMP="$(magisk --path)" || MAGISKTMP=/sbin
MODPATH="${0%/*}"

[ -d "$MAGISKTMP/.magisk/mirror/early-mount/initrc.d" ] && cp -Tf "$MODPATH/oem.rc" "$MAGISKTMP/.magisk/mirror/early-mount/initrc.d/oem.rc"

. "$MODPATH/resetprop.sh"

# Hide SELinux state
if [ "$(toybox cat /sys/fs/selinux/enforce 2>/dev/null)" == "0" ]; then
    chmod 640 /sys/fs/selinux/enforce
    chmod 440 /sys/fs/selinux/policy
fi

while [ "$(getprop sys.boot_completed)" != 1 ]; do
    sleep 1
done

# Prepare next profile for next boot
PROFILES_LIST="$MODPATH/common/profiles.list"
CURRENT_PROFILE_FILE="$MODPATH/current_profile.txt"

if [ -f "$CURRENT_PROFILE_FILE" ]; then
    CURRENT_DATA=$(cat "$CURRENT_PROFILE_FILE")
    # Get a random profile that is NOT the current one
    NEXT_PROFILE=$(grep -v "^#" "$PROFILES_LIST" | grep -vF "$CURRENT_DATA" | shuf -n 1)
    if [ -n "$NEXT_PROFILE" ]; then
        echo "$NEXT_PROFILE" > "$CURRENT_PROFILE_FILE"
    fi
fi

# Anti-Root / Security Properties Spoofing
check_resetprop ro.boot.flash.locked 1
check_resetprop ro.boot.vbmeta.device_state locked
check_resetprop vendor.boot.verifiedbootstate green
check_resetprop ro.boot.verifiedbootstate green
check_resetprop ro.boot.veritymode enforcing
check_resetprop vendor.boot.vbmeta.device_state locked

check_resetprop ro.boot.warranty_bit 0
check_resetprop ro.warranty_bit 0
check_resetprop ro.vendor.boot.warranty_bit 0
check_resetprop ro.vendor.warranty_bit 0
check_resetprop ro.is_ever_orange 0
    
check_resetprop ro.debuggable 0
check_resetprop ro.secure 1
check_resetprop ro.adb.secure 1

check_resetprop ro.secureboot.devicelock 1
check_resetprop ro.secureboot.lockstate locked

check_resetprop ro.build.type user
check_resetprop ro.build.keys release-keys
check_resetprop ro.build.tags release-keys

check_resetprop sys.oem_unlock_allowed 0
check_resetprop ro.oem_unlock_supported 0

check_resetprop init.svc.flash_recovery stopped
check_resetprop ro.boot.realmebootstate green
check_resetprop ro.boot.realme.lockstate 1

check_resetprop ro.crypto.state encrypted

resetprop -n persist.log.tag.LSPosed S
resetprop -n persist.log.tag.LSPosed-Bridge S

maybe_resetprop ro.bootmode recovery unknown
maybe_resetprop ro.boot.bootmode recovery unknown
maybe_resetprop ro.boot.mode recovery unknown
maybe_resetprop vendor.bootmode recovery unknown
maybe_resetprop vendor.boot.bootmode recovery unknown
maybe_resetprop vendor.boot.mode recovery unknown

if [ -n "$(resetprop ro.build.selinux)" ]; then
    resetprop --delete ro.build.selinux
fi

chmod 0440 /proc/cmdline
chmod 0440 /proc/net/unix
chmod 0750 /system/addon.d

settings delete global hidden_api_policy >/dev/null 2>&1
settings delete global hidden_api_policy_pre_p_apps >/dev/null 2>&1
settings delete global hidden_api_policy_p_apps >/dev/null 2>&1
