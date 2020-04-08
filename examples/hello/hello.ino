#include <Plog.h>
#include <FeatherFault.h>
#include <SdFat.h>
#include <sdios.h>

SdFat sd;

#define SD_CS_PIN 4

static plog::SerialAppender<plog::TxtFormatter> serialAppender(Serial);
static plog::SimpleFileAppender<plog::TxtFormatter> fa("hello.txt");

void setup(){
  digitalWrite(13, LOW);
  Serial.begin(115200);
  while(!Serial);
  digitalWrite(13, HIGH);
  if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(1))) {
    Serial.println("SD initialization failed!");
  }
  if(FeatherFault::DidFault()){
    FeatherFault::PrintFault(Serial);
    while(!Serial.available());
    Serial.println("continuing");
  }
  FeatherFault::StartWDT(FeatherFault::WDTTimeout::WDT_2S);
  MARK;
  // Time needs to be resynced at least every 60 days or any time you wake from sleep.
  // Normally you would do this with the RTC, but this is a minimal example
  plog::TimeSync(DateTime(__DATE__, __TIME__));
  MARK;
  plog::init(plog::debug, &serialAppender).addAppender(&fa);// Step2: initialize the logger.
  Serial.println("Setup complete.");
  MARK;
}

void loop(){


  // Step3: write log messages using a special macro. There are several log macros, use the macro you liked the most.
  PLOGD.printf("Hello log!");
  Serial.println("Logged");
  MARK;
  delay(1000);

}
