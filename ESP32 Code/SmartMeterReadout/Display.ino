void handleDisplay()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  if (millis() - lastTimeCharsReceived > 12000) //P1 Port most likely not connected
  {
    u8g2.drawStr(0, 10, "Plug in P1 Cable");
    u8g2.drawStr(0, 20, "Waiting for data...");
    u8g2.drawStr(0, 40, WiFi.localIP().toString().c_str());
  }
  else if (millis() - lastTimeCharsReceived < 12000  && millis() - lastTimeCharsDecoded > 12000)//Chars coming in but no decoded, most likely wrong baud
  {
    u8g2.drawStr(0, 10, "P1 Cable plugged in!");
    u8g2.drawStr(0, 20, "Autobauding...");
    
    static unsigned long prevBaudChangeMillis = 0;

    if(millis() - lastTimeCharsReceived > 250)
    {
      if(millis() - prevBaudChangeMillis >= 12000)
      {
      if(BAUDRATE == 9600)
      {
      BAUDRATE = 115200;
      Serial2.end();  
      Serial2.begin(BAUDRATE, SERIAL_8N1, P1_RX_PIN, 17, true);
      }
      else if(BAUDRATE == 115200)
      {
      BAUDRATE = 9600;
      Serial2.end();  
      Serial2.begin(BAUDRATE, SERIAL_7E1, P1_RX_PIN, 17, true);
      }
      prevBaudChangeMillis = millis();
      }
    }
    
  }

  else if (millis() - lastTimeCharsDecoded < 12000)
  {
    u8g2.drawLine(0, 10, 128, 10);
    u8g2.setCursor(0, 10); u8g2.print("OK | "); u8g2.print(BAUDRATE); u8g2.print(" | V"); u8g2.print(CURRENT_FIRMWARE_VERSION);
    u8g2.setCursor(0, 24); u8g2.print("Power: "); u8g2.print(currentPower, 0); u8g2.print("W");
    u8g2.setCursor(0, 34); u8g2.print("Cons T1: "); u8g2.print(consumptionLowRate, 0); u8g2.print("Wh");
    u8g2.setCursor(0, 44); u8g2.print("Cons T2: "); u8g2.print(consumptionHighRate, 0); u8g2.print("Wh");
    u8g2.setCursor(0, 54); u8g2.print("WiFi IP: "); u8g2.print(WiFi.localIP().toString());
    
  }
  u8g2.sendBuffer();
}
