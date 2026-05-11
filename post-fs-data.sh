#!/system/bin/sh

MODDIR="${0%/*}"
MODNAME="${MODDIR##*/}"
MAGISKTMP="$(magisk --path)" || MAGISKTMP=/sbin

if [ "$(magisk -V)" -lt 26302 ] || [ "$(/data/adb/ksud -V)" -lt 10818 ]; then
  touch "$MODDIR/disable"
fi

if [ ! -e "$MAGISKTMP/.magisk/mirror/sepolicy.rules/$MODNAME/sepolicy.rule" ] && [ ! -e "$MAGISKTMP/.magisk/sepolicy.rules/$MODNAME/sepolicy.rule" ]; then
    magiskpolicy --live --apply "$MODDIR/sepolicy.rule"
    ksud sepolicy apply "$MODDIR/sepolicy.rule"
fi

ksud sepolicy apply "$MODDIR/sepolicy.rule"

. "$MODDIR/resetprop.sh"

# these props must be set in post-fs-data
# clear out lineage and aosp words
# replace:
#    userdebug -> user
#    test-keys -> release-keys


#XKatrina
#
check_resetprop ro.bootimage.build.date Thu Oct 12 09:38:23 KST 2023
check_resetprop ro.bootimage.build.date.utc 1697071103
check_resetprop ro.bootimage.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
check_resetprop ro.bootimage.build.id RP1A.200720.012
check_resetprop ro.bootimage.build.version.incremental G991BXXU3AUE1
check_resetprop ro.bootimage.build.version.release 16
check_resetprop ro.bootloader G991BXXU3AUE1
check_resetprop ro.build.date Thu Oct 12 09:38:23 KST 2023
check_resetprop ro.build.date.utc 1697071103
check_resetprop ro.build.description o1sxxx-user 11 RP1A.200720.012G991BXXU3AUE1 release-keys
check_resetprop ro.build.displayid RP1A.200720.012.G991BXXU3AUE1
check_resetprop ro.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
#
check_resetprop ro.build.host D3wSRz95
check_resetprop ro.build.id RP1A.200720.012
check_resetprop ro.build.product qiZU
check_resetprop ro.build.user dmGTw
check_resetprop ro.build.version.incremental G991BXXU3AUE1
check_resetprop ro.build.version.release 16
check_resetprop ro.hardware LsJFdI
check_resetprop ro.lineage.device wDrX1ajkf
check_resetprop ro.matrixx.device wDrX1ajkf
check_resetprop ro.odm.build.date Thu Oct 12 09:38:23 KST 2023
check_resetprop ro.odm.build.date.utc 1697071103
check_resetprop ro.odm.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
check_resetprop ro.odm.build.id RP1A.200720.012
check_resetprop ro.odm.build.version.incremental G991BXXU3AUE1
check_resetprop ro.odm.build.version.release 16
check_resetprop ro.odm_dlkm.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
check_resetprop ro.product.board JQLvxCeYW
check_resetprop ro.product.bootimage.brand samsung
check_resetprop ro.product.bootimage.device wDrX1ajkf
check_resetprop ro.product.bootimage.manufacturer Samsung
check_resetprop ro.product.bootimage.model SMG991B
check_resetprop ro.product.bootimage.name o1sxxx
check_resetprop ro.product.brand samsung
check_resetprop ro.product.build.date Thu Oct 12 09:38:23 KST 2023
check_resetprop ro.product.build.date.utc 1697071103
check_resetprop ro.product.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
check_resetprop ro.product.build.id RP1A.200720.012
check_resetprop ro.product.build.version.incremental G991BXXU3AUE1
check_resetprop ro.product.build.version.release 16
check_resetprop ro.product.device wDrX1ajkf
check_resetprop ro.product.manufacturer Samsung
check_resetprop ro.product.model SMG991B
check_resetprop ro.product.name o1sxxx
check_resetprop ro.product.odm.brand samsung
check_resetprop ro.product.odm.device wDrX1ajkf
check_resetprop ro.product.odm.manufacturer Samsung
check_resetprop ro.product.odm.model SMG991B
check_resetprop ro.product.odm.name o1sxxx
check_resetprop ro.product.odm_dlkm.brand samsung
check_resetprop ro.product.odm_dlkm.device wDrX1ajkf
check_resetprop ro.product.odm_dlkm.model SMG991B
check_resetprop ro.product.product.brand samsung
check_resetprop ro.product.product.device wDrX1ajkf
check_resetprop ro.product.product.manufacturer Samsung
check_resetprop ro.product.product.model SMG991B
check_resetprop ro.product.product.name o1sxxx
check_resetprop ro.product.system.brand samsung
check_resetprop ro.product.system.device wDrX1ajkf
check_resetprop ro.product.system.manufacturer Samsung
check_resetprop ro.product.system.model SMG991B
check_resetprop ro.product.system.name o1sxxx
check_resetprop ro.product.system_ext.brand samsung
check_resetprop ro.product.system_ext.device wDrX1ajkf
check_resetprop ro.product.system_ext.manufacturer Samsung
check_resetprop ro.product.system_ext.model SMG991B
check_resetprop ro.product.system_ext.name o1sxxx
check_resetprop ro.product.vendor.brand samsung
check_resetprop ro.product.vendor.device wDrX1ajkf
check_resetprop ro.product.vendor.manufacturer Samsung
check_resetprop ro.product.vendor.model SMG991B
check_resetprop ro.product.vendor.name o1sxxx
check_resetprop ro.product.vendor_dlkm.brand samsung
check_resetprop ro.product.vendor_dlkm.device wDrX1ajkf
check_resetprop ro.product.vendor_dlkm.manufacturer Samsung
check_resetprop ro.product.vendor_dlkm.model SMG991B
check_resetprop ro.product.vendor_dlkm.name o1sxxx
check_resetprop ro.system.build.date Thu Oct 12 09:38:23 KST 2023
check_resetprop ro.system.build.date.utc 1697071103
check_resetprop ro.system.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
check_resetprop ro.system.build.id RP1A.200720.012
check_resetprop ro.system.build.version.incremental G991BXXU3AUE1
check_resetprop ro.system.build.version.release 16
check_resetprop ro.system_ext.build.date Thu Oct 12 09:38:23 KST 2023
check_resetprop ro.system_ext.build.date.utc 1697071103
check_resetprop ro.system_ext.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
check_resetprop ro.system_ext.build.id RP1A.200720.012
check_resetprop ro.system_ext.build.version.incremental G991BXXU3AUE1
check_resetprop ro.system_ext.build.version.release 16
check_resetprop ro.vendor.build.date Thu Oct 12 09:38:23 KST 2023
check_resetprop ro.vendor.build.date.utc 1697071103
check_resetprop ro.vendor.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
check_resetprop ro.vendor.build.id RP1A.200720.012
check_resetprop ro.vendor.build.version.incremental G991BXXU3AUE1
check_resetprop ro.vendor.build.version.release 16
check_resetprop ro.vendor_dlkm.build.date Thu Oct 12 09:38:23 KST 2023
check_resetprop ro.vendor_dlkm.build.date.utc 1697071103
check_resetprop ro.vendor_dlkm.build.fingerprint samsung/o1sxxx/o1s:11/RP1A.200720.012/G991BXXU3AUE1:user/release-keys
check_resetprop ro.vendor_dlkm.build.id RP1A.200720.012
check_resetprop ro.vendor_dlkm.build.version.incremental G991BXXU3AUE1
check_resetprop ro.vendor_dlkm.build.version.release 16

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

for prefix in system vendor system_ext product oem odm vendor_dlkm odm_dlkm bootimage; do
    check_resetprop ro.${prefix}.build.tags release-keys
    check_resetprop ro.${prefix}.build.type user
    replace_value_resetprop ro.${prefix}.build.description test-keys release-keys
    replace_value_resetprop ro.${prefix}.build.description userdebug user
    replace_value_resetprop ro.${prefix}.build.fingerprint test-keys release-keys
    replace_value_resetprop ro.${prefix}.build.fingerprint userdebug user
    replace_value_resetprop ro.${prefix}.build.description "aosp_" ""
    replace_value_resetprop ro.${prefix}.build.fingerprint "aosp_" ""
    replace_value_resetprop ro.product.${prefix}.name "aosp_" ""
    replace_value_resetprop ro.${prefix}.build.description "lineage_" ""
    replace_value_resetprop ro.${prefix}.build.fingerprint "lineage_" ""
    replace_value_resetprop ro.product.${prefix}.name "lineage_" ""
  # check_resetprop ro.${prefix}.build.date.utc $(date +"%s")
done

# check_resetprop ro.build.date.utc $(date +"%s")
# check_resetprop ro.build.version.security_patch $(date +2023-%m-%d)
# check_resetprop ro.vendor.build.security_patch $(date +2023-%m-%d)