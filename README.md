# Dynamic Prop Spoofer

A Magisk/KernelSU/APatch module designed to spoof Android system properties dynamically and safely.

## Features

- **Dynamic Profiles:** Randomly selects a new device profile (Samsung, Xiaomi, Google, OnePlus, etc.) every time you boot, ensuring anti-root detections see fresh data.
- **Randomized Serial Number:** Automatically generates a new `ro.serialno` on boot.
- **Robust Anti-Root Evasion:** Replaces test-keys with release-keys, hides SELinux state, and bypasses common root detection properties (like OEM unlock status, verified boot).
- **Command Line Interface (`propspoof`):** Easily switch profiles or check status from a terminal.
- **Highly Compatible:** Works seamlessly with Magisk, KernelSU, and APatch. Safe for Android 16.

## Installation

1. Download the latest release `.zip` from the GitHub Actions or Releases page.
2. Flash the module using your root manager (Magisk / KernelSU / APatch).
3. Reboot your device.

## Usage / Command Line Tool

This module comes with a built-in CLI tool called `propspoof`. You can use it via a terminal emulator (like Termux) by running it as root.

```sh
su -c "propspoof <command>"
```

**Available Commands:**
- `status`: Show the currently active profile.
- `randomize`: Immediately apply a random profile from the list without rebooting.
- `reload`: Re-apply the current profile properties.
- `next`: Force selection of a new random profile for the next boot.
- `list`: Show available profiles.

## Customizing Profiles

You can add or remove device profiles by editing `/data/adb/modules/dynamic_prop_spoofer/common/profiles.list` directly on your device, or by forking this repository and editing the file before building.

The format is strict (pipe `|` separated):
`MANUFACTURER|BRAND|MODEL|NAME|DEVICE|FINGERPRINT|DESCRIPTION|HARDWARE|BOARD|ID`

## License

This project is licensed under the GPL-3.0 License.
