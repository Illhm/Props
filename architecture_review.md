# Zygisk Module Architecture Review

## 1. Migrasi ke Native Zygisk Module (C/C++)
### Global `resetprop` (Shell) vs In-Memory Injection (Zygisk)

**Masalah pada Global Shell Script (`resetprop` via `post-fs-data.sh`):**
- **Race Conditions:** Skrip shell dieksekusi secara sekuensial. Aplikasi target dapat mulai berjalan dan membaca properti asli (menggunakan `__system_property_get` dll) sebelum skrip `resetprop` selesai mengubahnya di level sistem.
- **Deteksi Kesalahan:** Beberapa tools deteksi fraud dan anti-cheat dapat mendeteksi perubahan nilai properti di level global yang diakibatkan oleh `resetprop`.
- **Tidak Terisolasi:** Perubahan bersifat global. Semua aplikasi dan layanan sistem melihat nilai yang dipalsukan, yang dapat mengganggu fungsionalitas sistem (misal: OTA updates, fitur spesifik vendor).

**Keuntungan Zygisk (In-Memory Per-App Injection):**
- **Tanpa Race Condition:** Zygisk melakukan injeksi ke dalam *zygote* (induk dari semua proses aplikasi) tepat sebelum aplikasi di-fork. Module dapat memuat kode native ke dalam memori aplikasi *sebelum* kode aplikasi apa pun dieksekusi.
- **Isolasi (Per-App):** Modifikasi nilai properti hanya terjadi di dalam memori proses aplikasi yang ditargetkan (misalnya melalui teknik GOT/PLT hooking atau inline hooking pada libc `__system_property_get`). Aplikasi lain dan sistem tetap melihat nilai asli.
- **Sulit Dideteksi:** Karena file statis `/default.prop` atau ruang properti Android di kernel/init tidak benar-benar diubah, metode deteksi tradisional yang memeriksa inkonsistensi sistem akan gagal mendeteksi pemalsuan.

## 2. Integrasi Shell & Zygisk Companion
### Arsitektur Terbaik

1. **Penyiapan Data (Shell):** Skrip utilitas seperti `system/bin/newprops` (dieksekusi manual via `su`) digunakan untuk melakukan pengacakan profil (FINGERPRINT, dll), menghasilkan file konfigurasi yang mudah diparsing (seperti `props.json` atau `key=value` di `custom_props.txt`), dan menyiapkan `new_android_id.txt` untuk `service.sh`.
2. **Boot Scripts (Shell):**
   - `post-fs-data.sh`: Hanya menangani perizinan (sepolicy) dan persiapan lingkungan minimum.
   - `service.sh`: Menangani sinkronisasi tambahan (`settings put` untuk Android ID, timezone, pembersihan cache) setelah *boot completed*.
3. **Zygisk Module (C/C++):**
   - **Main Zygisk API:** Mengimplementasikan `zygisk::ModuleBase`. Menggunakan `preAppSpecialize` untuk membaca konfigurasi `custom_props.txt` dari file descriptor yang disediakan oleh companion, dan `postAppSpecialize` untuk mengeksekusi hook di level *libc*.
   - **Companion Process:** Proses daemon root-level kecil yang diluncurkan oleh Zygisk. Tugas utamanya adalah membaca `custom_props.txt` dari disk (karena proses aplikasi tidak memiliki akses read ke direktori root/modul karena SELinux/UID restriction) dan mengirimkan nilainya via IPC ke Zygisk module di dalam aplikasi target.

### Struktur Direktori Best Practice (Magisk/KernelSU/APatch Zygisk Module)

```text
xkatrina_snstv_prps/
├── module.prop           # Metadata modul (harus mencantumkan zygisk=yes jika menggunakan fitur bawaan zygisk)
├── post-fs-data.sh       # Script dieksekusi di fase post-fs-data (persiapan minimal)
├── service.sh            # Script dieksekusi setelah late_start (settings put, cache cleanup)
├── system/
│   └── bin/
│       └── newprops      # Skrip utilitas CLI untuk generate props
├── zygisk/               # Folder untuk binari Zygisk
│   ├── arm64-v8a.so      # Native module untuk arm64
│   ├── armeabi-v7a.so    # Native module untuk arm32
│   ├── x86_64.so         # Native module untuk x86_64
│   └── x86.so            # Native module untuk x86
├── jni/                  # Folder source code (tidak disertakan di zip rilis, hanya untuk build)
│   ├── Android.mk        # Build script ndk-build
│   ├── Application.mk    # Config NDK
│   └── module.cpp        # Kode sumber C++ Zygisk module dan Companion
├── custom_props.txt      # File konfigurasi hasil generate newprops (format key=value)
└── props_list.txt        # Daftar profil perangkat sumber
```