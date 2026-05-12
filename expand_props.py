import random
from datetime import datetime, timedelta

# OS Versions and their realistic Build ID prefixes
os_versions = [
    ("12", "SP1A"), # Android 12
    ("13", "TP1A"), # Android 13
    ("14", "UP1A"), # Android 14
    ("15", "AP1A", "AP2A", "AP3A"), # Android 15 has multiple branches
    ("16", "BP1A")  # Android 16
]

# A massive list of diverse Samsung device templates
# Format: (Model Name, Codename, Base Model Number)
devices = [
    # Note Series
    ("Galaxy Note20 Ultra", "c2q", "N986"),
    ("Galaxy Note20", "c1", "N981"),
    # S21 Series
    ("Galaxy S21 Ultra 5G", "p3", "G998"),
    ("Galaxy S21+ 5G", "t2", "G996"),
    ("Galaxy S21 5G", "o1", "G991"),
    ("Galaxy S21 FE 5G", "r9", "G990"),
    # S22 Series
    ("Galaxy S22 Ultra", "b0q", "S908"),
    ("Galaxy S22+", "g0", "S906"),
    ("Galaxy S22", "r0", "S901"),
    # S23 Series
    ("Galaxy S23 Ultra", "dm3q", "S918"),
    ("Galaxy S23+", "dm2", "S916"),
    ("Galaxy S23", "dm1", "S911"),
    ("Galaxy S23 FE", "r11q", "S711"),
    # S24 Series
    ("Galaxy S24 Ultra", "e2q", "S928"),
    ("Galaxy S24+", "e2s", "S926"),
    ("Galaxy S24", "e2", "S921"),
    ("Galaxy S24 FE", "r12q", "S721"),
    # S25 Series
    ("Galaxy S25 Ultra", "e3q", "S938"),
    ("Galaxy S25+", "e3s", "S936"),
    ("Galaxy S25", "e3", "S931"),
    # Z Fold
    ("Galaxy Z Fold3 5G", "q2q", "F926"),
    ("Galaxy Z Fold4", "q4", "F936"),
    ("Galaxy Z Fold5", "q5", "F946"),
    ("Galaxy Z Fold6", "q6", "F956"),
    # Z Flip
    ("Galaxy Z Flip3 5G", "b2q", "F711"),
    ("Galaxy Z Flip4", "b4", "F721"),
    ("Galaxy Z Flip5", "b5", "F731"),
    ("Galaxy Z Flip6", "b6", "F741"),
    # A Series
    ("Galaxy A52 5G", "a52xq", "A526"),
    ("Galaxy A52s 5G", "a52sxq", "A528"),
    ("Galaxy A53 5G", "a53x", "A536"),
    ("Galaxy A54 5G", "a54x", "A546"),
    ("Galaxy A55 5G", "a55x", "A556"),
    ("Galaxy A73 5G", "a73x", "A736"),
    ("Galaxy A33 5G", "a33x", "A336"),
    ("Galaxy A34 5G", "a34x", "A346"),
    ("Galaxy A35 5G", "a35x", "A356"),
    ("Galaxy A23 5G", "a23x", "A236"),
    ("Galaxy A24", "a24", "A245"),
    ("Galaxy A25 5G", "a25x", "A256"),
    ("Galaxy A14 5G", "a14x", "A146"),
    ("Galaxy A15 5G", "a15x", "A156"),
    # M Series
    ("Galaxy M53 5G", "m53x", "M536"),
    ("Galaxy M54 5G", "m54x", "M546"),
    ("Galaxy M33 5G", "m33x", "M336"),
    ("Galaxy M34 5G", "m34x", "M346"),
    ("Galaxy M14 5G", "m14x", "M146"),
]

regions = ["B", "U", "N", "E", "0", "W"]
carriers = ["XX", "OYN", "U1", "N0", "EU"]

def generate_random_date():
    # Random date within the last ~3 years
    start_date = datetime.now() - timedelta(days=365 * 3)
    random_days = random.randint(0, 365 * 3)
    dt = start_date + timedelta(days=random_days)
    return dt.strftime("%y%m%d")

new_lines = set()
count_needed = 1000

for i in range(count_needed * 5):
    if len(new_lines) >= count_needed:
        break

    # 1. Pick a random device
    model_name, codename, base_model = random.choice(devices)

    # 2. Pick a random OS version and Build ID base
    os_tuple = random.choice(os_versions)
    os_ver = os_tuple[0]
    # some OS versions have multiple prefixes, pick one
    build_id_prefix = random.choice(os_tuple[1:])

    # 3. Create a realistic Build ID: e.g. UP1A.231005.007
    random_date = generate_random_date()
    build_id = f"{build_id_prefix}.{random_date}.{random.randint(1, 999):03d}"

    # 4. Create a realistic Firmware baseband
    region = random.choice(regions)
    carrier = random.choice(carriers)
    revision = random.choice(["S", "U"]) # S = Security update, U = Feature update
    bootloader_bit = random.randint(1, 9)
    # Firmware date code (Year/Month mapping logic simplified for randomness)
    fw_date_code = f"{random.choice('ABCD')}{random.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZ')}{random.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZ')}{random.randint(1,9)}"

    firmware = f"{base_model}{region}{carrier}{revision}{bootloader_bit}{fw_date_code}"

    # Format: BRAND|MODEL|DEVICE|FINGERPRINT
    # FINGERPRINT: samsung/codename_region/codename:OS_VER/BUILD_ID/FIRMWARE:user/release-keys

    # some variation in the device string to make it seem like different regional variants
    codename_variant = f"{codename}{region.lower()}"

    fingerprint = f"samsung/{codename_variant}/{codename}:{os_ver}/{build_id}/{firmware}:user/release-keys"

    # Use official Samsung internal model code for ro.product.model to bypass strict anti-fraud checks
    official_model = f"SM-{base_model}{region}"

    line = f"Samsung|{official_model}|{codename}|{fingerprint}"
    new_lines.add(line)

with open('props_list.txt', 'w') as f:
    for line in sorted(list(new_lines)):
        f.write(line + '\n')

print(f"Total diverse Samsung lines generated: {len(new_lines)}")
