//Author: Arjan Pals

//ESP-32 based P1 port read
//With OTA self update
//With SD card logging

#include <Arduino.h>
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

String CURRENT_FIRMWARE_VERSION = "12.90";

#define P1_RX_PIN 16
uint32_t BAUDRATE = 9600;

#define OTA
#include <ESP32WebServer.h>  //https://github.com/Pedroalbuquerque/ESP32WebServer
ESP32WebServer server(80);


#include "EEPROM.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#define SD_CS 5



#ifdef OTA  //If OTA update is defined include necessary libraries
#include <WiFi.h>

#include <HTTPClient.h>
#include <Update.h>
HTTPClient client;

//WiFi credentials
String ssid = "TEST";
String password = "TEST";

int totalLength;        //total size of firmware
int currentLength = 0;  //current size of written firmware
#endif


char inputBuffer[100];
uint8_t currentBufferPosition = 0;
double consumptionLowRate = 0;
double consumptionHighRate = 0;
double redeliveryHighRate = 0;
double redeliveryLowRate = 0;
double currentRedeliveryPower = 0;
double voltage;
double currentPower = 0;
int YEAR, MONTH, DAY, HOUR, MINUTE, SECOND;

unsigned long lastTimeCharsReceived;
unsigned long lastTimeCharsDecoded;

unsigned long sdLogInterval = 10000;
bool SD_ERROR = true;


void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);  //Start connection for Serial Monitor @ 115200 b/s
  Serial.println("Program Started");

  EEPROM.begin(1000);



#ifdef OTA
  // Start WiFi connection

  WiFi.mode(WIFI_MODE_STA);

  int address = 0;
  ssid = EEPROM.readString(address);
  address += ssid.length() + 1;
  password = EEPROM.readString(address);

  Serial.print("Connecting to SSID: \"");
  Serial.print(ssid);
  Serial.println("\"");
  Serial.print("Password: \"");
  Serial.print(password);
  Serial.println("\"");


  WiFi.begin(ssid.c_str(), password.c_str());

//After uploading the sketch to the ESP32 the value will be 4294967
//So this if-statement checks if that is the case, and if so it puts it to 10 seconds
  sdLogInterval = EEPROM.readULong(800);
  if (sdLogInterval == 4294967) {
    EEPROM.writeULong(800, 10000);
    EEPROM.commit();
    sdLogInterval = 10000;
  }

#endif

  char serialNumber[24];
  snprintf(serialNumber, 23, "%llX", ESP.getEfuseMac());

  u8g2.begin();  //Initialise OLED

  //Print boot-up screen
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "P1 Smart Meter");
  u8g2.setCursor(0, 20);
  u8g2.print("Baudrate: " + String(BAUDRATE));
  u8g2.setCursor(0, 30);
  u8g2.print("Firmware: " + CURRENT_FIRMWARE_VERSION);
  u8g2.setCursor(0, 40);
  u8g2.print("SN:");
  u8g2.print(serialNumber);
  u8g2.setCursor(0, 60);
  u8g2.print("Copyright Arjan Pals");
  u8g2.sendBuffer();

  //Open Serial connection
  Serial2.begin(BAUDRATE, SERIAL_7E1, P1_RX_PIN, 17, true);

  delay(5000);

#ifdef OTA
  while (millis() < 10000) {}


  if (WiFi.status() == WL_CONNECTED) {

    if (!checkAndInstallUpdate()) {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Error occured during");
      u8g2.drawStr(0, 20, "Firmware Update");
      u8g2.sendBuffer();

    } else {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Newest Version already");
      u8g2.drawStr(0, 20, "installed");
      u8g2.sendBuffer();
    }
    delay(2000);
  }

#endif

  initSDCard();

  //Initialise webserver
  server.on("/", HomePage);
  server.on("/downloadLogFile", downloadLogFile);
  server.on("/downloadLogFileBackup", downloadLogFileBackup);
  server.on("/deleteLogFile", deleteLogFile);
  server.on("/firmwareUpdate", webUpdate);

  //Start the server
  server.begin();
}

#define PRINT_HUMAN_READABLE_SENTENCES

void loop() {
  //Keep the server responsive
  server.handleClient();


  //Parse data form P1 port
  parseData();

  //Print data to the display
  handleDisplay();

  //Log data to SD card every 'sdLogInterval' ms
  static unsigned long pMillis = 0;

  if (millis() - pMillis >= sdLogInterval && millis() - lastTimeCharsDecoded < 12000 && millis() > sdLogInterval) {
    pMillis = millis();
    if (!SD_ERROR) {
      logSDCard();
    } else {
      Serial.println("SD-Error");
    }
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    Serial.print("User input:");
    Serial.println(input);
    if (input.startsWith("?")) {
      Serial.println("Use NEWLINE only!\n");

      Serial.println("Set new Wi-Fi use \"WIFI_SETUP:<SSID here>,<PASSWORD here>\"");
      Serial.println("Get current IP adress use \"GET_IP\"");
      Serial.println("Print log file \"PRINT_FILE\"");
      Serial.println("Print backup log file \"PRINT_BACKUP_FILE\"");
      Serial.println("Check for new firmware use \"CHECK_FIRMWARE_VERSION\"");
      Serial.println("Set log interval use \"SET_LOG_INTERVAL\"");


    } else if (input.startsWith("GET_IP")) {
      Serial.println(WiFi.localIP().toString());
    } else if (input.startsWith("PRINT_FILE")) {
      File file2 = SD.open("/P1DataLog.csv", FILE_READ);
      while (file2.available()) {
        Serial.write(file2.read());
      }
      file2.close();
      Serial.println("done");
    } else if (input.startsWith("PRINT_BACKUP_FILE")) {
      File file2 = SD.open("/P1DataLog_Backup.csv", FILE_READ);
      while (file2.available()) {
        Serial.write(file2.read());
      }
      file2.close();
      Serial.println("done");
    }

    else if (input.startsWith("WIFI_SETUP:")) {
      ssid = input.substring(input.indexOf(":") + 1, input.indexOf(","));
      password = input.substring(input.indexOf(",") + 1, input.indexOf('\n'));

      Serial.print("New SSID: \"");
      Serial.print(ssid);
      Serial.println("\"");
      Serial.print("New Password: \"");
      Serial.print(password);
      Serial.println("\"");


      int address = 0;
      EEPROM.writeString(address, ssid);
      address += ssid.length() + 1;
      EEPROM.writeString(address, password);

      EEPROM.commit();

      WiFi.disconnect();

      WiFi.begin(ssid.c_str(), password.c_str());
    } else if (input.startsWith("CHECK_FIRMWARE_VERSION")) {
      Serial.println("Current Firmware Version: " + CURRENT_FIRMWARE_VERSION);
      Serial.println("Please Wait...\n");
      if (checkForNewFirmware()) {
        Serial.println("New Firmware Available!");
        Serial.println("Type \"INSTALL_LATEST_VERSION\" to install latest version");
      }

      else {
        Serial.println("No new Firmware found");
      }
    } else if (input.startsWith("INSTALL_LATEST_VERSION")) {
      checkAndInstallUpdate();
    } else if (input.startsWith("SET_LOG_INTERVAL")) {
      Serial.print("Current log interval(s): ");
      Serial.println(sdLogInterval / 1000);
      Serial.print("Enter new interval in seconds:");
      while (Serial.available()) {
        Serial.read();
      }
      unsigned long sm = millis();
      while (!Serial.available() && millis() - sm < 20000) {}
      float intervalT = Serial.parseFloat() * 1000;
      Serial.println(intervalT / 1000, 0);
      EEPROM.writeULong(800, intervalT);
      EEPROM.commit();
      while (Serial.available()) { Serial.read(); }
      Serial.println("done");
    } else {
      Serial.println("Command not recognized");
    }
  }
}
