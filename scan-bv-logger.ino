#include <SPI.h>
#include <SD.h>
#include <Ds1302.h>

#define VBATT A0

#define RTC_CLK 2
#define RTC_DAT 3
#define RTC_RST 4

#define SD_CS 10
#define SD_MOSI 11
#define SD_MISO 12
#define SD_SCK 13

Ds1302 rtc(RTC_RST, RTC_CLK, RTC_DAT);

// Take in a DateTime object and return a string representation
String buildTimeStr(Ds1302::DateTime &now) {
  String dtstr = "";
  dtstr += "20" + String(now.year) + "-";  // YYYY-
  if (now.month < 10) dtstr += "0";
  dtstr += String(now.month) + "-";  // YYYY-MM-
  if (now.day < 10) dtstr += "0";
  dtstr += String(now.day) + " ";  // YYYY-MM-DD
  if (now.hour < 10) dtstr += "0";
  dtstr += String(now.hour) + ":";  // YYYY-MM-DD HH:
  if (now.minute < 10) dtstr += "0";
  dtstr += String(now.minute) + ":";  // YYYY-MM-DD HH:MM
  if (now.second < 10) dtstr += "0";
  dtstr += String(now.second);  // YYYY-MM-DD HH:MM:SS
  return dtstr;  
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present; aborting.");
    // don't do anything more:
    while (1);
  }
  Serial.println("Card initialized.");

  Serial.print("Initializing RTC module...");
  rtc.init();

  // uncomment to manually set RTC
  //Serial.print("RTC is halted; setting time...");
  //Ds1302::DateTime dt = {
//    .year = 22,
//    .month = Ds1302::MONTH_APR,
//    .day = 13,
//    .hour = 12,
//    .minute = 2,
//    .second = 10,
//    .dow = Ds1302::DOW_SUN
//  };
//    
//  rtc.setDateTime(&dt);

  // read current time and write it 
  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  Serial.println("RTC initialized.");
  Serial.print("Current time: ");
  Serial.print(buildTimeStr(now));

  analogReference(DEFAULT);
}

void loop() {
  // Read the analog value 10 times for averaging
  int raw_data = 0;
  for (int i = 0; i < 10; i++) {
    raw_data += analogRead(VBATT);
  }
  float volts = raw_data*0.001671;
  // Magic constant is ~((4.1/1023)*(51k||68k + 91k)/(51k||68k))/10 with some manual tuning
  // The divide by 10 of the whole expression is to average out over 10 measurements for accuracy
  // 4.1/1023 converts value into volts (input saturates at 4.1 V instead of std 5 V for some reason)
  // (51k||68k + 91k)/(51k||68k) reverses the effect of the voltage divider to get the source voltage
  // Note that the 51k||68k is because we originally used a 51k resistor to drop to 5 V instead of 3.4 V
  
  // get the current time:
  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  while (now.year < 22) {
    // sometimes the clock returns bogus data, so retry until we have a valid date
    rtc.getDateTime(&now);
  }
  String dtstr = buildTimeStr(now);

  if (volts) {    
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dtstr + "," + String(volts));
      dataFile.close();
      // print to the serial port too:
      Serial.println(dtstr + "," + String(volts));
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.csv");
    }
  }

  delay(1000);
}