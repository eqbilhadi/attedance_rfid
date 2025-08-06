
# 📘 RFID Reader dengan ESP32 dan Layar OLED

Sistem ini menggunakan ESP32 sebagai mikrokontroler utama untuk membaca kartu RFID (RC522), menampilkan data di layar OLED, dan memberikan umpan balik suara melalui buzzer.



## 1. Komponen yang Dibutuhkan
- ESP32 Devkit
- RFID RC522
- OLED SSD1306 (I2C) 0.96"
- BUZZER Aktif
- Breadboard
- Kabel Jumper Male to Male


## 2. Wiring Diagram

#### 📟 OLED SSD1306 (I2C) ke ESP32

| OLED Pin | Fungsi | ESP32 Pin / Breadboard |
|----------|--------|-------------------------|
| VCC      | Power  | ke rail merah (+)       |
| GND      | Ground | ke rail biru (-)        |
| SCL      | Clock  | GPIO 5 / D4             |
| SDA      | Data   | GPIO 4 / D5             |

#### 📡 RFID RC522 (SPI) ke ESP32

| RC522 Pin | Fungsi         | ESP32 Pin / Breadboard |
|-----------|----------------|-------------------------|
| SDA       | SS (Slave Sel) | GPIO 21 / D21           |
| SCK       | SPI Clock      | GPIO 18 / D18           |
| MOSI      | SPI MOSI       | GPIO 23 / D23           |
| MISO      | SPI MISO       | GPIO 19 / D19           |
| GND       | Ground         | ke rail biru (-)        |
| RST       | Reset          | GPIO 22 / D22           |
| 3.3V      | Power          | ke rail merah (+)       |

#### 🔔 BUZZER Aktif ke ESP32

| Buzzer Pin | Fungsi | ESP32 Pin / Breadboard |
|------------|--------|-------------------------|
| + (VCC)    | Power  | GPIO 15 / D15           |
| - (GND)    | Ground | ke rail biru (-)        |

---

## 🧰 Jalur Power dan Ground pada Breadboard

#### 🔵 Jalur Ground (GND)
- Hubungkan salah satu **GND dari ESP32** ke **rail biru (-)** pada breadboard.
- Sambungkan GND dari **OLED**, **RFID**, dan **Buzzer** ke rail biru ini.

#### 🔴 Jalur Power (VCC 3.3V)
- Hubungkan **3.3V dari ESP32** ke **rail merah (+)** pada breadboard.
- Sambungkan VCC dari **OLED** dan **RFID** ke rail merah ini.


## ⚙️ Alur Kerja Sistem

    1. Perangkat menyala, OLED menampilkan status "Ready".
    2. Pengguna menempelkan kartu RFID.
    3. RFID membaca UID dan mengirim ke ESP32.
    4. OLED menampilkan UID atau status hadir.
    5. Buzzer berbunyi untuk konfirmasi scan berhasil.
    6. (Opsional) Data dapat dikirim ke server atau disimpan lokal.


## ⚠️ Catatan Penting
    - Semua komponen bekerja pada **3.3V**, **jangan gunakan 5V**.
    - Gunakan library Arduino yang sesuai (lihat di bawah).
    - Pastikan koneksi kabel kuat, terutama SDA/SCL dan SPI.


## 📦 Library yang Dibutuhkan
    - MFRC522 by Miguel Balboa
    - Adafruit SSD1306
    - Adafruit GFX Library

    Cara install: Arduino IDE → Library Manager → cari & install library di atas.

## 🖥️ Pengaturan Arduino IDE
    - Board: ESP32 Dev Module
    - Port: COM sesuai ESP32
    - Upload Speed: 115200


## 📚 Referensi
- https://randomnerdtutorials.com
- https://github.com/miguelbalboa/rfid

---

![ESP32](images/ESP32-GPIO-Pins.webp)
