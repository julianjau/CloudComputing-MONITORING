#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C bisa berbeda, sesuaikan jika perlu

const int lm35Pin = A0;
const int ldrPin = A1;
const int buzzerPin = 8;
const float voltageReference = 3.0; // Tegangan referensi 3V

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  lcd.begin(16, 2); // Inisialisasi LCD
  lcd.backlight(); // Mengaktifkan lampu belakang LCD
  pinMode(lm35Pin, INPUT);
  pinMode(ldrPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}
                                                                                                                                                                                                                                                                   
float calculateMembership(float value, float start, float end) {
  if (value <= start) return 0;
  if (value >= end) return 1;
  return (value - start) / (end - start);
}
    
void fuzzifyTemperature(float temperature, float &sejuk, float &hangat) {
  sejuk = calculateMembership(temperature, 10, 25);
  hangat = calculateMembership(temperature, 20, 35);
}

void fuzzifyLightIntensity(float light, float &redup, float &terang) {
  redup = calculateMembership(light, 20, 50);
  terang = calculateMembership(light, 45, 75);
}

int defuzzifyOutput(float sejuk, float hangat, float redup, float terang, float temperatureC) {
  if (temperatureC > 24) {
    return 1;
  }
  return 0;
}

void loop() {
  // Membaca suhu dari sensor LM35
  int lm35Value = analogRead(lm35Pin);
  float voltage = lm35Value * (voltageReference / 1024.0);
  float temperatureC = voltage * 100.0;

  // Membaca intensitas cahaya dari sensor LDR
  int ldrValue = analogRead(ldrPin);
  float lightIntensity = map(ldrValue, 0, 1023, 0, 100); // Mengonversi ke persentase

  // Fuzzyfikasi
  float sejuk, hangat, redup, terang;
  fuzzifyTemperature(temperatureC, sejuk, hangat);
  fuzzifyLightIntensity(lightIntensity, redup, terang);

  // Defuzzifikasi
  int buzzerOutput = defuzzifyOutput(sejuk, hangat, redup, terang, temperatureC);
  digitalWrite(buzzerPin, buzzerOutput);

  // Menampilkan suhu dan intensitas cahaya pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatureC);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Light: ");
  lcd.print(lightIntensity);
  lcd.print(" %");

  // Mengirim data ke serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.print(" C, Light Intensity: ");
  Serial.print(lightIntensity);
  Serial.println(" %");

  delay(2000); // Tunggu 2 detik sebelum membaca lagi
}