
void HomePage()
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>P1 Port Readout</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 120px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 12px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Webserver P1 Port</h1>\n";
  ptr +="<h3>Main Menu</h3>\n";
  ptr +="<p>Click below to download .csv logfile</p><a class=\"button button-on\" href=\"/downloadLogFile\">Download</a>\n";
  ptr +="<p>Click below to download .csv backup logfile</p><a class=\"button button-on\" href=\"/downloadLogFileBackup\">Download</a>\n";
  ptr +="<p>Click below to check for new firmware</p><a class=\"button button-on\" href=\"/firmwareUpdate\">Check</a>\n";
  ptr +="<p>Click below to delete .csv logfile</p><a class=\"button button-on\" href=\"/deleteLogFile\">Delete</a>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  server.send(200, "text/html",ptr);
}

void deleteLogFile()
{
  File DELETE = SD.open("/P1DataLog.csv");
    if (DELETE) 
    {
      DELETE.close();
  deleteFile(SD, "/P1DataLog.csv");
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 120px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 12px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>File Deleted Sucsessfully!</h1>\n";
  ptr +="<p>Click below to go to Main Menu</p><a class=\"button button-on\" href=\"/\">Back</a>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  server.send(200, "text/html",ptr);
    }
    else
    {
     server.send(200,"text/text", "File not found or no SD pluged in!"); 
    }

  
}

void downloadLogFile() 
{
  SD_file_download("P1DataLog.csv");
}
void downloadLogFileBackup() 
{
  SD_file_download("P1DataLog_Backup.csv");
}

void SD_file_download(String filename){

    File download = SD.open("/"+filename);
    if (download) 
    {
      server.sendHeader("Content-Type", "text/text");
      server.sendHeader("Content-Disposition", "attachment; filename="+filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } 
    else
    {
      server.send(200,"text/text", "File not found or no SD pluged in!");
    }
  
}
