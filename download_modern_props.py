import urllib.request
import json
import re

# We will fetch a newer repository of fingerprints for PIF
# https://github.com/daboynb/PlayIntegrityNEXT/tree/main/FINGERPRINTS has many
# But it's easier to hit the openapi or known json dumps of device catalogues.

# Let's use the Google Play Console Device Catalog dump if possible, or another PIF repo.
url = "https://raw.githubusercontent.com/Programming4life/PlayIntegrityFix/main/pif.json"

try:
    req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
    with urllib.request.urlopen(req) as response:
        content = json.loads(response.read().decode('utf-8'))
        print(content)
except Exception as e:
    print(e)

url2 = "https://raw.githubusercontent.com/TheScarcastic/play-integrity-fix/main/pif.json"
try:
    req = urllib.request.Request(url2, headers={'User-Agent': 'Mozilla/5.0'})
    with urllib.request.urlopen(req) as response:
        content = json.loads(response.read().decode('utf-8'))
        print(content)
except Exception as e:
    print(e)
