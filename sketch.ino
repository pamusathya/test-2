#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <DHT.h>
#include <RTClib.h>

// =============================
// DHT22
// =============================
#define DHT_PIN PA0
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

// =============================
// SD CARD
// =============================
#define SD_CS PA4

// =============================
// RTC
// =============================
RTC_DS1307 rtc;

// =============================
// LED
// =============================
#define LED_PIN PC13

// =============================
// FILE
// =============================
File dataFile;

// =============================
// SETUP
// =============================
void setup()
{
  Serial.begin(115200);

  delay(1000);

  pinMode(LED_PIN, OUTPUT);

  Serial.println();
  Serial.println("==============================");
  Serial.println(" STM32 EMBEDDED DATA LOGGER ");
  Serial.println("==============================");

  // -----------------------------
  // DHT22
  // -----------------------------
  dht.begin();

  Serial.println("DHT22 initialized");

  // -----------------------------
  // I2C
  // STM32F103C8:
  // PB7 = SDA
  // PB6 = SCL
  // -----------------------------
  Wire.begin();

  // -----------------------------
  // RTC
  // -----------------------------
  if (!rtc.begin())
  {
    Serial.println("RTC ERROR!");
  }
  else
  {
    Serial.println("RTC OK");

    if (!rtc.isrunning())
    {
      Serial.println("RTC not running");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }

  // -----------------------------
  // SD CARD
  // -----------------------------
  Serial.println("Initializing SD Card...");

  if (!SD.begin(SD_CS))
  {
    Serial.println("SD CARD ERROR!");
  }
  else
  {
    Serial.println("SD CARD OK");

    dataFile = SD.open("datalog.csv", FILE_WRITE);

    if (dataFile)
    {
      dataFile.println(
        "Date,Time,Temperature_C,Humidity_percent"
      );

      dataFile.close();

      Serial.println("datalog.csv created");
    }
    else
    {
      Serial.println("File creation ERROR");
    }
  }

  Serial.println();
  Serial.println("LOGGER READY");
  Serial.println();
}

// =============================
// LOOP
// =============================
void loop()
{
  // -----------------------------
  // Read DHT22
  // -----------------------------
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("DHT22 READ ERROR");

    delay(2000);
    return;
  }

  // -----------------------------
  // Read RTC
  // -----------------------------
  DateTime now = rtc.now();

  // -----------------------------
  // Serial Monitor
  // -----------------------------
  Serial.print("Date: ");

  Serial.print(now.day());
  Serial.print("/");
  Serial.print(now.month());
  Serial.print("/");
  Serial.print(now.year());

  Serial.print("  Time: ");

  if (now.hour() < 10)
    Serial.print("0");

  Serial.print(now.hour());
  Serial.print(":");

  if (now.minute() < 10)
    Serial.print("0");

  Serial.print(now.minute());
  Serial.print(":");

  if (now.second() < 10)
    Serial.print("0");

  Serial.print(now.second());

  Serial.print("  Temp: ");

  Serial.print(temperature, 1);

  Serial.print(" C  Humidity: ");

  Serial.print(humidity, 1);

  Serial.println(" %");

  // -----------------------------
  // Save to SD
  // -----------------------------
  dataFile = SD.open("datalog.csv", FILE_WRITE);

  if (dataFile)
  {
    dataFile.print(now.day());
    dataFile.print("/");
    dataFile.print(now.month());
    dataFile.print("/");
    dataFile.print(now.year());

    dataFile.print(",");

    dataFile.print(now.hour());
    dataFile.print(":");

    dataFile.print(now.minute());
    dataFile.print(":");

    dataFile.print(now.second());

    dataFile.print(",");

    dataFile.print(temperature, 1);

    dataFile.print(",");

    dataFile.println(humidity, 1);

    dataFile.close();

    Serial.println("DATA SAVED TO SD CARD");

    // LED indication
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    Serial.println("SD FILE ERROR");
  }

  Serial.println("------------------------------");

  // DHT22 needs approximately 2 seconds
  delay(5000);
}
