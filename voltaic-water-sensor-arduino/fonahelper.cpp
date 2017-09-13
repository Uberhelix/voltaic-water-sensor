#include <Adafruit_SleepyDog.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define halt(s) { Serial.println(F( s )); while(1);  }

extern Adafruit_FONA fona;
extern SoftwareSerial fonaSS;
extern int dtrPin;

void setFonaPowerDownMode(void) {
  Watchdog.reset();
  digitalWrite(dtrPin, HIGH);
  fonaSS.begin(4800);
  fonaSS.println(F("AT+CFUN=0")); //Sets SIM800 module to minimum functionality mode
}

void setFonaWakeUpMode(void){
  Watchdog.reset();
  digitalWrite(dtrPin, LOW);
  delay(60);
  fonaSS.begin(4800);
  fonaSS.println(F("AT+CFUN=1")); //Sets SIM800 module to full functionality mode
}

// Checks for active network. Sleep for 1 hr and reset if network is not detected
void checkForNetwork(void){
  int count = 0;
  while ((fona.getNetworkStatus() != 1)) {
    while ((fona.getNetworkStatus() != 1) && (count < 25)){
      Watchdog.reset();
      //Serial.print("Get network status try #: ");
      //Serial.println(count);
      Watchdog.sleep(5000);
      count += 1;
    }
    while (count == 25){
      //Serial.println("Sleeping for ~1hr.");
      setFonaPowerDownMode();
      count += 1;
    }
    while ((count > 25) && (count < 925)){
      Watchdog.reset();
      Watchdog.sleep(8000);
      count += 1;
    }
    while (count == 925){
      setFonaWakeUpMode();
      if (! fona.begin(fonaSS)) {           // can also try fona.begin(Serial1) 
        //Serial.println(F("Couldn't find FONA"));
      }
      fonaSS.println("AT+CMEE=2");
      //Serial.println(F("FONA is OK"));
      count = 0;
      Watchdog.reset();
    }
  }
}

boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password) {
  Watchdog.reset();
  
  digitalWrite(dtrPin, LOW);
  delay(60);
  
  //Serial.println(F("Initializing FONA....(May take 3 seconds)"));
  
  fonaSS.begin(4800); // if you're using software serial
  
  if (! fona.begin(fonaSS)) {           // can also try fona.begin(Serial1) 
    // Serial.println(F("Couldn't find FONA"));
    return false;
  }
  fonaSS.println("AT+CMEE=2");
  //Serial.println(F("FONA is OK"));
  Watchdog.reset();
  //Serial.println(F("Checking for network..."));
  checkForNetwork();

  Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  Watchdog.reset();
  
  fona.setGPRSNetworkSettings(apn, username, password);

  //Serial.println(F("Disabling GPRS"));
  fona.enableGPRS(false);
  
  Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  Watchdog.reset();
//
//  Serial.println(F("Enabling GPRS"));
//  if (!fona.enableGPRS(true)) {
//    Serial.println(F("Failed to turn GPRS on")); 
//    return false;
//  }
//  Watchdog.reset();

  return true;
}
