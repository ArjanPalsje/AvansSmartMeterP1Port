void initSDCard() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");

  } else {
    SD_ERROR = false;

    if (!SD.exists("/P1DataLog.csv")) {
      Serial.println("Not on SD Card");
      appendFile(SD, "/P1DataLog.csv", "yyyy-mm-dd hh-mm-ss,consumptionLowRate(Wh),consumptionHighRate(Wh),redeliveryLowRate(Wh),redeliveryHighRate(Wh),currentRedeliveryPower(W),Voltage(V),Power(W)\r\n");
    }
    if (!SD.exists("/P1DataLog_Backup.csv")) {
      Serial.println("Not on SD Card");
      appendFile(SD, "/P1DataLog_Backup.csv", "yyyy-mm-dd hh-mm-ss,consumptionLowRate(Wh),consumptionHighRate(Wh),redeliveryLowRate(Wh),redeliveryHighRate(Wh),currentRedeliveryPower(W),Voltage(V),Power(W)\r\n");
    }
  }



  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void logSDCard() {
  String dateString = String(YEAR) + "-" + String(MONTH) + "-" + String(DAY) + " " + String(HOUR) + ":" + String(MINUTE) + ":" + String(SECOND);


  String dataMessage = dateString + "," + String(consumptionLowRate, 0) + "," + String(consumptionHighRate, 0) + "," + String(redeliveryLowRate, 0) + "," + String(redeliveryHighRate, 0) + "," + String(currentRedeliveryPower, 0) + "," + String(voltage, 0) + "," + String(currentPower, 0) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  delay(250);
  appendFile(SD, "/P1DataLog.csv", dataMessage.c_str());
  appendFile(SD, "/P1DataLog_Backup.csv", dataMessage.c_str());
}


// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");


    appendFile(fs, "/P1DataLog.csv", "consumptionLowRate(Wh),consumptionHighRate(Wh),redeliveryLowRate(Wh),redeliveryHighRate(Wh),currentRedeliveryPower(W),Voltage(V),Power(W)\r\n");



  } else {
    Serial.println("Delete failed");
  }
}
