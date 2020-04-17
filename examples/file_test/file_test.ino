#include <Plog.h>
#include <SdFat.h>
#include <sdios.h>

SdFat sd;

static plog::RollingFileAppender<plog::CsvFormatter> fa("test.txt", 500, 10);

#define SD_CS_PIN 4

void setup(){
  digitalWrite(13, LOW);
  Serial1.begin(115200);
  digitalWrite(13, HIGH);
  if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(1)))
    Serial1.println("SD initialization failed!");
  // Time needs to be resynced at least every 60 days or any time you wake from sleep.
  // Normally you would do this with the RTC, but this is a minimal example
  plog::TimeSync(DateTime(__DATE__, __TIME__), -7);
  plog::init(plog::debug, &fa);
  Serial.println("Setup complete.");
  /*
  plog::util::File f("test.txt");
  f.write("Writing to the end of the file...\n");
  f.seek(0, SEEK_SET);
  f.write("Beginning ");
  f.unlink("newtest.txt");
  f.rename("test.txt", "newtest.txt");
  f.close();
  */
  Serial.println("Done");
}

void loop(){
  static int i = 0;
  PLOGD << ++i;
  Serial1.println("Printing Directory...");
  sd.ls(&Serial1, "/", LS_R);
  delay(1000);

}
