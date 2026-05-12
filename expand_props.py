import random
import os

# Base Samsung templates for Android 16
# Format: BRAND|MODEL|DEVICE|FINGERPRINT
# FINGERPRINT: Brand/Name/Device:16/BP1A.241025.001/ID:user/release-keys

templates = [
    # S25 series
    ("Samsung", "Galaxy S25 Ultra", "e3q", "samsung/e3qsqw/e3q:16/BP1A.241025.001/S938BXXU1AXA1:user/release-keys"),
    ("Samsung", "Galaxy S25+", "e3s", "samsung/e3ssqw/e3s:16/BP1A.241025.001/S936BXXU1AXA1:user/release-keys"),
    ("Samsung", "Galaxy S25", "e3", "samsung/e3sqw/e3:16/BP1A.241025.001/S931BXXU1AXA1:user/release-keys"),
    # S24 series
    ("Samsung", "Galaxy S24 Ultra", "e2q", "samsung/e2qsqw/e2q:16/BP1A.241025.001/S928BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy S24+", "e2s", "samsung/e2ssqw/e2s:16/BP1A.241025.001/S926BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy S24", "e2", "samsung/e2sqw/e2:16/BP1A.241025.001/S921BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy S24 FE", "r12q", "samsung/r12qsqw/r12q:16/BP1A.241025.001/S721BXXS3AXC4:user/release-keys"),
    # S23 series
    ("Samsung", "Galaxy S23 Ultra", "dm3q", "samsung/dm3qsqw/dm3q:16/BP1A.241025.001/S918BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy S23+", "dm2", "samsung/dm2sqw/dm2:16/BP1A.241025.001/S916BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy S23", "dm1", "samsung/dm1sqw/dm1:16/BP1A.241025.001/S911BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy S23 FE", "r11q", "samsung/r11qsqw/r11q:16/BP1A.241025.001/S711BXXS3AXC4:user/release-keys"),
    # Z Fold/Flip series
    ("Samsung", "Galaxy Z Fold6", "q6", "samsung/q6sqw/q6:16/BP1A.241025.001/F956BXXU1AXA1:user/release-keys"),
    ("Samsung", "Galaxy Z Flip6", "b6", "samsung/b6sqw/b6:16/BP1A.241025.001/F741BXXU1AXA1:user/release-keys"),
    ("Samsung", "Galaxy Z Fold5", "q5", "samsung/q5sqw/q5:16/BP1A.241025.001/F946BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy Z Flip5", "b5", "samsung/b5sqw/b5:16/BP1A.241025.001/F731BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy Z Fold4", "q4", "samsung/q4sqw/q4:16/BP1A.241025.001/F936BXXS3AXC4:user/release-keys"),
    ("Samsung", "Galaxy Z Flip4", "b4", "samsung/b4sqw/b4:16/BP1A.241025.001/F721BXXS3AXC4:user/release-keys"),
    # A series
    ("Samsung", "Galaxy A55 5G", "a55x", "samsung/a55xsq/a55x:16/BP1A.241025.001/A556BXXS4AXD1:user/release-keys"),
    ("Samsung", "Galaxy A54 5G", "a54x", "samsung/a54xsq/a54x:16/BP1A.241025.001/A546BXXS4AXD1:user/release-keys"),
    ("Samsung", "Galaxy A35 5G", "a35x", "samsung/a35xsq/a35x:16/BP1A.241025.001/A356BXXS4AXD1:user/release-keys"),
    ("Samsung", "Galaxy A34 5G", "a34x", "samsung/a34xsq/a34x:16/BP1A.241025.001/A346BXXS4AXD1:user/release-keys"),
    ("Samsung", "Galaxy A25 5G", "a25x", "samsung/a25xsq/a25x:16/BP1A.241025.001/A256BXXS4AXD1:user/release-keys"),
    ("Samsung", "Galaxy A15 5G", "a15x", "samsung/a15xsq/a15x:16/BP1A.241025.001/A156BXXS4AXD1:user/release-keys"),
]

# We want only Samsung devices, and we want around 1000 of them.
new_lines = set()

# Random string generation helpers for variants
def rand_id():
    chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
    return "".join(random.choice(chars) for _ in range(8))

# Regional/Carrier codes common for Samsung (B=Global, U=US, N=Korea, 0=China)
model_suffixes = ["B", "U", "N", "0", "E", "F", "W"]

# We will clear the existing file completely to ensure ONLY Samsung is there
count_needed = 1000

for i in range(count_needed * 3): # Loop enough times to handle duplicates
    if len(new_lines) >= count_needed:
        break

    t = random.choice(templates)
    brand, model, device, fingerprint_base = t

    # Generate realistic variations of the model number and firmware versions
    suffix = random.choice(model_suffixes)
    # Extract the base baseband (e.g. S938B from S938BXXU1AXA1)
    baseband_part = fingerprint_base.split('/')[-2]

    # Let's create a dynamic Samsung-like baseband
    # Example: S928BXXS3AXC4 -> Model(S928) + Region(B) + XX(Country/CSC) + S(Revision) + 3(Bootloader) + AXC4(Datecode)

    fake_model_num = f"{baseband_part[:4]}{suffix}" # e.g. S938U
    fake_bootloader = str(random.randint(1, 9))
    fake_datecode = f"A{random.choice('XYZ')}{random.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZ')}{random.randint(1,9)}"

    new_firmware = f"{fake_model_num}XXS{fake_bootloader}{fake_datecode}"

    # Modify model name slightly
    new_model = f"{model} ({suffix})"

    # Modify fingerprint
    parts = fingerprint_base.split('/')
    # Format: samsung/e3qsqw/e3q:16/BP1A.241025.001/S938BXXU1AXA1:user/release-keys
    new_fp = f"samsung/{parts[1]}/{device}:16/BP1A.241025.001/{new_firmware}:user/release-keys"

    line = f"{brand}|{new_model}|{device}|{new_fp}"
    new_lines.add(line)

# Write to file directly (overwriting anything that was there)
with open('props_list.txt', 'w') as f:
    for line in sorted(list(new_lines)):
        f.write(line + '\n')

print(f"Total Samsung lines generated: {len(new_lines)}")
