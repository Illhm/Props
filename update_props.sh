#!/system/bin/sh
# System configuration update automation tool for Android build properties.

# 5. Disable SELinux enforcement at the start
echo "[INFO] Disabling SELinux enforcement..."
setenforce 0

# Function to restore SELinux state on exit
cleanup() {
    echo "[INFO] Re-enabling SELinux enforcement..."
    setenforce 1
    echo "[INFO] Execution complete."
}
trap cleanup EXIT

# Validate arguments
TARGET_PROPS_LIST="$1"
if [ -z "$TARGET_PROPS_LIST" ]; then
    echo "[ERROR] Usage: $0 <target_properties_list_file>"
    exit 1
fi

if [ ! -f "$TARGET_PROPS_LIST" ]; then
    echo "[ERROR] Target properties list file '$TARGET_PROPS_LIST' not found."
    exit 1
fi

BUILD_PROP_URL="https://raw.githubusercontent.com/gu1dry/StockBuildProp/master/Samsung/P5110ZCDMH1%20build.prop"
BUILD_PROP_FILE="build.prop"
PROPS_LIST_TXT="props_list.txt"
CUSTOM_PROPS_TXT="custom_props.txt"

# 1. Fetch Samsung P5110ZCDMH1 build.prop
echo "[INFO] Fetching build.prop from $BUILD_PROP_URL..."
curl -fsSL "$BUILD_PROP_URL" -o "$BUILD_PROP_FILE"
if [ $? -ne 0 ] || [ ! -s "$BUILD_PROP_FILE" ]; then
    echo "[ERROR] Failed to download build.prop"
    exit 1
fi

# 2. Bulk update properties
echo "[INFO] Applying properties from $TARGET_PROPS_LIST to $BUILD_PROP_FILE..."

# Process the properties list
while IFS='=' read -r key value || [ -n "$key" ]; do
    # Skip empty lines and comments
    if [ -z "$key" ] || echo "$key" | grep -q "^#"; then
        continue
    fi

    # Trim leading/trailing whitespace without xargs
    key=$(printf '%s' "$key" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')
    value=$(printf '%s' "$value" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')

    if [ -z "$key" ]; then
        continue
    fi

    # Update build.prop file
    if grep -q "^${key}=" "$BUILD_PROP_FILE"; then
        esc_value=$(printf '%s' "$value" | sed -e 's/[&\|]/\&/g')
        sed -i "s|^${key}=.*|${key}=${esc_value}|" "$BUILD_PROP_FILE"
        echo "[UPDATE] Updated $key=$value in $BUILD_PROP_FILE"
    else
        echo "${key}=${value}" >> "$BUILD_PROP_FILE"
        echo "[ADD] Added $key=$value to $BUILD_PROP_FILE"
    fi

    # Try to apply using resetprop if available, but don't fail if not
    if command -v ./resetprop >/dev/null 2>&1; then
        ./resetprop -n "$key" "$value" >/dev/null 2>&1
        echo "[SYSTEM] Applied $key to system via resetprop"
    fi

done < "$TARGET_PROPS_LIST"

echo "[INFO] Build properties update complete."

# 3. Update props_list.txt
# Extract necessary values from updated build.prop
BRAND=$(grep "^ro.product.brand=" "$BUILD_PROP_FILE" | cut -d'=' -f2)
MODEL=$(grep "^ro.product.model=" "$BUILD_PROP_FILE" | cut -d'=' -f2)
DEVICE=$(grep "^ro.product.device=" "$BUILD_PROP_FILE" | cut -d'=' -f2)
FINGERPRINT=$(grep "^ro.build.fingerprint=" "$BUILD_PROP_FILE" | cut -d'=' -f2)

if [ -n "$BRAND" ] && [ -n "$MODEL" ] && [ -n "$DEVICE" ] && [ -n "$FINGERPRINT" ]; then
    NEW_ENTRY="${BRAND}|${MODEL}|${DEVICE}|${FINGERPRINT}"

    echo "[INFO] Generated new props_list.txt entry: $NEW_ENTRY"

    touch "$PROPS_LIST_TXT"
    if ! grep -qxF "$NEW_ENTRY" "$PROPS_LIST_TXT"; then
        echo "$NEW_ENTRY" >> "$PROPS_LIST_TXT"
        echo "[SUCCESS] Appended new entry to $PROPS_LIST_TXT"
    else
        echo "[INFO] Entry already exists in $PROPS_LIST_TXT"
    fi
else
    echo "[WARNING] Could not extract all required fields for props_list.txt from $BUILD_PROP_FILE"
fi

# 4. Update pprops configuration (custom_props.txt)
echo "[INFO] Updating pprops configuration ($CUSTOM_PROPS_TXT)..."

# Create or overwrite custom_props.txt with resetprop commands based on target props
echo "# Updated via update_props.sh" > "$CUSTOM_PROPS_TXT"
echo "# Source: $TARGET_PROPS_LIST" >> "$CUSTOM_PROPS_TXT"

while IFS='=' read -r key value || [ -n "$key" ]; do
    # Skip empty lines and comments
    if [ -z "$key" ] || echo "$key" | grep -q "^#"; then
        continue
    fi

    # Trim leading/trailing whitespace without xargs
    key=$(printf '%s' "$key" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')
    value=$(printf '%s' "$value" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')

    if [ -n "$key" ]; then
        echo "check_resetprop $key \"$value\"" >> "$CUSTOM_PROPS_TXT"
    fi
done < "$TARGET_PROPS_LIST"

echo "[SUCCESS] Updated $CUSTOM_PROPS_TXT"

# trap will trigger cleanup() on exit
