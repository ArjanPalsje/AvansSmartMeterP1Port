# AvansSmartMeterP1Port
With this repository you will be able to create your own P1 Port readout device.

It also supports Over-The-Air Firmware updates! Every time it boots up and is connected to the last known Wi-Fi network it will check if a newer firmware is available, if so it will install it automatically. 

To change Wi-Fi settings on the ESP-32 you should use the Serial Monitor (Make sure the Serial Monitor is set to NEWLINE only!).
To see the available commands type '?' in the Serial Monitor, or see the list below :)

Set new Wi-Fi use "WIFI_SETUP:<SSID here>,<PASSWORD here>"

Get the current IP address of the device use "GET_IP"

Print the log file to the Serial Monitor use "PRINT_FILE"

Print the backup log file to the Serial Monitor use "PRINT_BACKUP_FILE"

Check for new Firmware manually use "CHECK_FIRMWARE_VERSION"

Set log interval use "SET_LOG_INTERVAL"
