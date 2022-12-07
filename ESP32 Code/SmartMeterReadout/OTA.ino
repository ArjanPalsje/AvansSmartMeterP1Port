    #define LATEST_VERSION_URL "https://raw.githubusercontent.com/ArjanPalsje/AvansSmartMeterP1Port/main/LATEST_VERSION.txt"
    #define LATEST_FIRMWARE_BIN_FILE_URL "https://raw.githubusercontent.com/ArjanPalsje/AvansSmartMeterP1Port/main/SmartMeterReadout.bin"
    
    /*
     * void checkAndInstallUpdate();
     * If 0 is returned an error occured
     * If 1 is returned latest version is the same as the current version
     * If there is an update available it will be installed within 'checkAndInstallUpdate' and the ESP32 restarts itself, and it returns 'nothing'.
     */

bool checkAndInstallUpdate()
{
  String NEWEST_FIRMWARE_VERSION = "0.0";
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "Looking for");
  u8g2.drawStr(0, 20, "Firmware Update");
  u8g2.setCursor(0, 30); u8g2.print("Current FW: " + CURRENT_FIRMWARE_VERSION);
  u8g2.sendBuffer();
  client.begin(LATEST_VERSION_URL);
  int resp = client.GET();

  if (resp == 200)
  {
    NEWEST_FIRMWARE_VERSION = client.getString();

    u8g2.setCursor(0, 40); u8g2.print("Newest FW: " + NEWEST_FIRMWARE_VERSION);
    u8g2.sendBuffer();
    if (NEWEST_FIRMWARE_VERSION.toDouble() > CURRENT_FIRMWARE_VERSION.toDouble())
    {
      client.end();



      client.begin(LATEST_FIRMWARE_BIN_FILE_URL);
      

      if (client.GET() == 200) {
        // get length of document (is -1 when Server sends no Content-Length header)
        totalLength = client.getSize();
        // transfer to local variable
        int len = totalLength;
        // this is required to start firmware update process
        Update.begin(UPDATE_SIZE_UNKNOWN);
        Serial.printf("FW Size: %u\n", totalLength);
        // create buffer for read
        uint8_t buff[128] = { 0 };
        // get tcp stream
        WiFiClient * stream = client.getStreamPtr();
        // read all data from server
        Serial.println("Updating firmware...");


        while (client.connected() && (len > 0 || len == -1)) {



          // get available data size
          size_t size = stream->available();
          if (size) {
            // Read up to 128 bytes
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            // Pass update to function
            updateFirmware(buff, c);
            if (len > 0) {
              len -= c;
            }
          }
          delay(1);
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }


  client.end();

}
void updateFirmware(uint8_t *data, size_t len) {
  Update.write(data, len);
  currentLength += len;
  
   // Print dots while data is being downloaded
  static int prevPerc = 0;

  //Print update process to OLED
  if (prevPerc != map(currentLength, 0, totalLength, 0, 100))
  {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10); u8g2.print("Updating Now!");
    u8g2.setCursor(0, 20); u8g2.print(map(currentLength, 0, totalLength, 0, 100)); u8g2.print("%");
    u8g2.drawFrame(10, 40, 108, 10);
    u8g2.drawBox(10, 40, map(currentLength, 0, totalLength, 0, 108), 10);
    u8g2.sendBuffer();
    prevPerc = map(currentLength, 0, totalLength, 0, 100);
    Serial.println(String(prevPerc)+"%");
  }
  // if current length of written firmware is not equal to total firmware size, repeat
  if (currentLength != totalLength) return;

  Update.end(true);
  Serial.printf("\nUpdate Success, Total Size: %u\nRebooting...\n", currentLength);
  
  u8g2.clearBuffer();
  u8g2.setCursor(0, 10); u8g2.print("Update Done!");
  u8g2.setCursor(0, 20); u8g2.print("Restarting in 5 sec.");
  u8g2.sendBuffer();

  delay(5000);
  ESP.restart(); //Restart ESP32 and new firmware is used
}


    /*
     * void checkForNewFirmware();
     * If 0 is returned if an error occured or no update is available
     * If 1 is returned when there is an firmware update available
     */

bool checkForNewFirmware()
{
 if (WiFi.status() == WL_CONNECTED)
 {

  String NEWEST_FIRMWARE_VERSION = "0.0";

  client.begin(LATEST_VERSION_URL);


  if (client.GET() == 200)
  {
    NEWEST_FIRMWARE_VERSION = client.getString();

    client.end();

    if (NEWEST_FIRMWARE_VERSION.toDouble() > CURRENT_FIRMWARE_VERSION.toDouble())
    {
      return true;
    }
    return false;
  }
  else
  {
   return false; 
  }
 }
 return false;
}

void webUpdate()
{
server.send(200, "text/html","Trying to update, see display for info");
checkAndInstallUpdate();

}