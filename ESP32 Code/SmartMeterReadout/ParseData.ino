void parseData() {
  while (Serial2.available()) {
    lastTimeCharsReceived = millis();
    char incomingChar = Serial2.read();


    // It could be 8N1, but also 7E1.
    // Remove any parity bit in the highest bit.
    // Inverting of the signal is done by HardwareSerial library
    incomingChar &= 0x7F;

    if (incomingChar == '\n') {  //Newline char found, getting values from string

      Serial.println(inputBuffer);
      long tl = 0;
      long tld = 0;

      //Timestamp
      char buffer[20];
      if (sscanf(inputBuffer, "0-0:1.0.0(%s", &buffer) > 0) {
        String stringObject = buffer;
        YEAR = 2000 + stringObject.substring(0, 2).toInt();
        MONTH = stringObject.substring(2, 4).toInt();
        DAY = stringObject.substring(4, 6).toInt();
        HOUR = stringObject.substring(6, 8).toInt();
        MINUTE = stringObject.substring(8, 10).toInt();
        SECOND = stringObject.substring(10, 12).toInt();

#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.println(String(YEAR) + "-" + String(MONTH) + "-" + String(DAY) + "|" + String(HOUR) + ":" + String(MINUTE) + ":" + String(SECOND));
#endif
        lastTimeCharsDecoded = millis();
      }


      char smartMeterSerialNumber[40];
      if (sscanf(inputBuffer, "0-0:96.1.1(%s", &smartMeterSerialNumber) > 0) {

        for (int i = 0; i < sizeof(smartMeterSerialNumber); i++) {
          if (smartMeterSerialNumber[i] == ')') {
            smartMeterSerialNumber[i] = 0;
          }
        }

#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Serial number meter: ");
        Serial.println(smartMeterSerialNumber);
#endif
        lastTimeCharsDecoded = millis();
      }

      //Usage low tarif
      if (sscanf(inputBuffer, "1-0:1.8.1(%ld.%ld%*s", &tl, &tld) > 0) {
        consumptionLowRate = tl * 1000 + tld;
#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Verbruik laag tarief(Wh): ");
        Serial.println(consumptionLowRate);
#endif
        lastTimeCharsDecoded = millis();
      }
      //Usage low tarif
      else if (sscanf(inputBuffer, "1-0:1.8.2(%ld.%ld%*s", &tl, &tld) > 0) {
        consumptionHighRate = tl * 1000 + tld;
#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Verbruik Hoog tarief(Wh): ");
        Serial.println(consumptionHighRate);
#endif
        lastTimeCharsDecoded = millis();
      } else if (sscanf(inputBuffer, "1-0:1.7.0(%ld.%ld%d*s", &tl, &tld) > 0) {
        currentPower = tl * 1000 + tld;
#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Current Power(W): ");
        Serial.println(currentPower);
#endif
        lastTimeCharsDecoded = millis();

      } else if (sscanf(inputBuffer, "1-0:2.8.1(%ld.%ld%d*s", &tl, &tld) > 0) {
        redeliveryLowRate = tl * 1000 + tld;
#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Teruglevering laag tarief(Wh): ");
        Serial.println(redeliveryLowRate);
#endif
        lastTimeCharsDecoded = millis();
      } else if (sscanf(inputBuffer, "1-0:2.8.2(%ld.%ld%d*s", &tl, &tld) > 0) {
        redeliveryHighRate = tl * 1000 + tld;
#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Teruglevering hoog tarief(Wh): ");
        Serial.println(redeliveryHighRate);
#endif
        lastTimeCharsDecoded = millis();
      }

      else if (sscanf(inputBuffer, "1-0:2.7.0(%ld.%ld%d*s", &tl, &tld) > 0) {
        currentRedeliveryPower = tl * 1000 + tld;
#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Huidige teruglevering(W): ");
        Serial.println(currentRedeliveryPower);
#endif
        lastTimeCharsDecoded = millis();
      }

      else if (sscanf(inputBuffer, "1-0:32.7.0(%ld.%ld%d*s", &tl, &tld) > 0) {
        voltage = tl + tld * 0.1;
#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Current Voltage(V): ");
        Serial.println(voltage);
#endif
        lastTimeCharsDecoded = millis();
      }

      else if (sscanf(inputBuffer, "0-n:24.2.1.255(%ld.%ld%d*s", &tl, &tld) > 0) {
        gasMeter = tl + tld * 0.1;
#ifdef PRINT_HUMAN_READABLE_SENTENCES
        Serial.print("Gas delivery(m3): ");
        Serial.println(gasMeter);
#endif
        lastTimeCharsDecoded = millis();
      }


      //Clear input buffer
      for (int i = 0; i < (sizeof(inputBuffer) / sizeof(inputBuffer[0])) - 1; i++) {
        inputBuffer[i] = 0;
      }
      currentBufferPosition = 0;

    } else {
      inputBuffer[currentBufferPosition] = incomingChar;
      currentBufferPosition++;
    }
  }
}
