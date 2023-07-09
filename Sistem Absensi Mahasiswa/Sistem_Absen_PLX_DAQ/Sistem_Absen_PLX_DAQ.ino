//                     KELOMPOK 9
//SISTEM ABSENSI MAHASISWA MENGGUNAKAN KTM BERBASIS 
//            ARDUINO UNO DAN RFID READER

#include <SPI.h>
#include <MFRC522.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SS_PIN 10   // Pin SDA ARDUINO
#define RST_PIN 9   // RST ARDUINO
#define LED_HIJAU 7 // PIN 7 ARDUINO
#define LED_MERAH 6 // PIN 6 ARDUINO

LiquidCrystal_I2C lcd(0x27, 16, 2); // Ukuran LCD dan Tipe LCD
unsigned long lastModeChangeTime = 0; //Waktu Terkahir Perubahan

int const buzzer = 5; // Pin 5
int pb_mode = A0;     // Pin A0
int pb_state = HIGH; 
int prev_pb_state = HIGH;

byte readCard[5]; // Jumalah Byte max yang terbaca
// Menambahkan Identitas Kartu
int cards1[5] = {83, 118, 188, 160, 57}; // Nilai UID Kartu
int cards2[5] = {243, 67, 104, 14, 214}; // Nilai UID Kartu

int ID;
String nama;
String nim;
String jurusan;
bool status_kartu = false;
bool sudah_absen = false;
boolean mode_pulang = false;

//Inisialisasi RTC
RTC_DS3231 rtc;
//Inisialisasi MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};

void setup() {
  lcd.setBacklight(255);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, LOW);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  pinMode(pb_mode, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (!rtc.begin()) {
    Serial.println("RTC tidak terhubung, Cek kembali wiring!");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC tidak bekerja, Setel ulang waktu!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println("CLEARSHEET");
  Serial.println("LABEL,ID,Date,Name,NIM, Card ID,Jurusan,Waktu Masuk,Waktu Keluar");

  lcd.setCursor(1, 0);
  lcd.print("System Absensi");
  lcd.setCursor(0, 1);
  lcd.print("   Kelompok 9");

  delay(1500);
  lcd.clear();
  delay(50);
}

void loop() {
  DateTime now = rtc.now();
  pb_state = digitalRead(pb_mode);

  if (pb_state != prev_pb_state) {
    delay(20);
    if (pb_state != prev_pb_state) {
      // Cek waktu terakhir perubahan mode
      unsigned long currentTime = millis();
      if (currentTime - lastModeChangeTime >= 1000) {
        // Jika sudah berlalu 1 detik sejak perubahan mode terakhir,
        // maka ubah mode
        mode_pulang = !mode_pulang;
        Serial.println("mode");
        if (mode_pulang) {
          digitalWrite(LED_HIJAU, LOW);
          digitalWrite(LED_MERAH, LOW);
        }
        lastModeChangeTime = currentTime;
      }
    }
  }

  prev_pb_state = pb_state;


  lcd.setCursor(4, 1);
  printposisilcd(now.hour());
  lcd.print(":");
  printposisilcd(now.minute());
  lcd.print(":");
  printposisilcd(now.second());

  if (mode_pulang == true) {
    lcd.setCursor(2, 0);
    lcd.print("Absen Pulang");
  } else {
    lcd.setCursor(2, 0);
    lcd.print("Absen Masuk ");
  }

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  if (mfrc522.uid.uidByte[0] != readCard[0] ||
      mfrc522.uid.uidByte[1] != readCard[1] ||
      mfrc522.uid.uidByte[2] != readCard[2] ||
      mfrc522.uid.uidByte[3] != readCard[3]) {

    Serial.println("");
    Serial.print("UID : ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      readCard[i] = mfrc522.uid.uidByte[i];
      Serial.print(readCard[i]);
      if (i < mfrc522.uid.size - 1) {
        Serial.print(" ,");
      } else {
        Serial.println("");
      }

      sudah_absen = false;
      status_kartu = true;

      if (readCard[i] == cards1[i]) {
        ID = 1;
        nama = "Farel Putra Albana";
        nim = "2101020057";
        jurusan = "Teknik Informatika";
      } else if (readCard[i] == cards2[i]) {
        ID = 2;
        nama = "Mohd. Fikri Raekhal";
        nim = "2101020097";
        jurusan = "Teknik Informatikan";
      } else {
        status_kartu = false;
      }
    }
  } else {
    sudah_absen = true;
    Serial.println("sudah absen");
    lcd.setCursor(2, 1);
    lcd.print("Sudah Absen");
    delay(2000);
  }

    if (status_kartu == true && sudah_absen == false) {
     if (mode_pulang == false) {
      Serial.print("DATA,");
      Serial.print(ID);
      Serial.print(",");
      printtanggal();
      Serial.print(",");
      Serial.print(nama);
      Serial.print(",");
      Serial.print(nim);
      Serial.print(",");
      printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
      Serial.print(",");
      Serial.print(jurusan);
      Serial.print(",");
      printwaktu();
      Serial.print(",");
      Serial.println("");
      lcd.setCursor(2, 1);
      digitalWrite(LED_HIJAU, HIGH); // Nyalakan LED hijau
      digitalWrite(buzzer, HIGH); // Menghidupkan buzzer
      delay(300);
      digitalWrite(LED_HIJAU, LOW); // Matikan LED hijau
      digitalWrite(buzzer, LOW); // Matikan buzzer
      lcd.print("Terima Kasih");
      delay(2000);
    }

    if (mode_pulang == true) {
      Serial.print("DATA,");
      Serial.print(ID);
      Serial.print(",");
      printtanggal();
      Serial.print(",");
      Serial.print(nama);
      Serial.print(",");
      Serial.print(nim);
      Serial.print(",");
      printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
      Serial.print(",");
      Serial.print(jurusan);
      Serial.print(",");
      Serial.print("");
      Serial.print(",");
      printwaktu();
      Serial.print(",");
      Serial.println("");
      lcd.setCursor(2, 1);
      digitalWrite(LED_MERAH, HIGH); // Nyalakan LED merah
      digitalWrite(buzzer, HIGH); // Menghidupkan buzzer
      delay(300);
      digitalWrite(LED_MERAH, LOW); // Matikan LED merah
      digitalWrite(buzzer, LOW); // Matikan buzzer
      lcd.print("Terima Kasih");
      delay(2000);
    }
  }

  lcd.clear();
  delay(50);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void printtanggal() {
  DateTime now = rtc.now();
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print('/');
  Serial.print(now.day());
  Serial.print('/');
  Serial.print(now.month());
  Serial.print('/');
  Serial.print(now.year());
}

void printwaktu() {
  DateTime now = rtc.now();
  printposisi(now.hour());
  Serial.print(':');
  printposisi(now.minute());
  Serial.print(':');
  printposisi(now.second());
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printposisi(int digits) {
  if (digits < 10)
    Serial.print("0");
    Serial.print(digits);
}

void printposisilcd(int digits) {
  if (digits < 10)
    lcd.print("0");
    lcd.print(digits);
}