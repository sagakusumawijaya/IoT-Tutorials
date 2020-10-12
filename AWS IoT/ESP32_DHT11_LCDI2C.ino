#include<WiFi.h>
#include<DHT.h>
#include<AWS_IOT.h>
#include<Wire.h>
#include<LiquidCrystal_I2C.h>

#define DHT_PIN 4 // pin dht11 
#define DHT_TYPE DHT11  // Type of the DHT Sensor, DHT11/DHT22

#define WIFI_SSID "wifissid" // SSID WiFi
#define WIFI_PASSWD "wifipassword" // Password WiFi

#define CLIENT_ID "your_client_id"// thing ID, harus unik/berbeda dari setiap thing yang terkoneksi ke AWS account
#define MQTT_TOPIC "your_mqtt_topic" // topik untuk MQTT data
#define AWS_HOST "xxxxxxxxxxxxxxxxxxxxxxxxxxxx.amazonaws.com" // host tujuan upload data

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHT_PIN, DHT_TYPE);
AWS_IOT aws;

const byte KARAKTER_DERAJAT = 0;
byte derajat[] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
int hum = 0, temp = 0;

void setup() {
  Serial.begin(9600);
  Serial.print("\nINISIALISASI POST DATA KE AWS IOT \n");

  Serial.print("\n  INISIALISASI WIFI: MENGKONEKSIKAN KE ");  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  Serial.print("  ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n  SUDAH TERKONEKSI\n");

  Serial.print("\n  INISIALISASI DHT11...");
  dht.begin();
  Serial.println("  TELAH SELESAI INISIALISASI DHT11");

  Serial.println("\n  INISIALISASI KONEKSI KE AWS....");
  if (aws.connect(AWS_HOST, CLIENT_ID) == 0) { // MENGKONEKSIKAN KE HOST & returns 0 JIKA SUKSES TERKONEKSI
    Serial.println("  SUDAH TERKONEKSI KE AWS\n");
  }
  else {
    Serial.println("  GAGAL TERKONEKSI!\n PASTIKAN LAGI SUBSCRIPTION KE MQTT DI HALAMAN TEST (TEST PAGE)");
  }
  Serial.println("  READY");

  lcd.init();
  lcd.createChar(KARAKTER_DERAJAT, derajat);
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("DHT 11 AWS IOT");
  lcd.setCursor(0, 1);
  lcd.print("TEMPERATUR :");
  lcd.setCursor(0, 2);
  lcd.print("HUMIDITY   :");
}

void loop() {
  // pembacaan temperatur dan humidity
  temp = dht.readTemperature(); // return temperatur dalam satuan °C
  hum = dht.readHumidity();// return humidity (kelembaban) dalam %

  // Memeriksa apakah pembacaan data DHT11 berhasil atau tidak
  if (temp == NAN || hum == NAN) { // NAN : mengindikasi tidak adanya data
    Serial.println("PEMBACAAN DHT11 GAGAL");
  }
  else {
    //create string payload untuk publishing
    String temp_humidity = "Temperature: ";
    temp_humidity += String(temp);
    temp_humidity += "°C Humidity: ";
    temp_humidity += String(hum);
    temp_humidity += " %";


    char payload[40];
    temp_humidity.toCharArray(payload, 40);

    Serial.println("Publishing:- ");
    Serial.println(payload);
    if (aws.publish(MQTT_TOPIC, payload) == 0) { // MEM-PUBLISH payload DAN returns 0 JIKA SUKSES
      Serial.println("SUKSES\n");
    }
    else {
      Serial.println("GAGAL!\n");
    }

    lcd.setCursor(13, 1);
    lcd.print(temp);
    lcd.setCursor(13, 2);
    lcd.print(hum);

    if (temp < 10) {
      lcd.setCursor(14, 1);
    } else {
      lcd.setCursor(15, 1);
    }
    lcd.write(KARAKTER_DERAJAT);

    if (temp < 10) {
      lcd.setCursor(15, 1);
    } else {
      lcd.setCursor(16, 1);
    }
    lcd.print("C");

    if (hum < 10) {
      lcd.setCursor(14, 2);
    } else {
      lcd.setCursor(15, 2);
    }
    lcd.print("%");
  }

  delay(2000);
}
