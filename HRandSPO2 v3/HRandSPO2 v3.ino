#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
 
#define REPORTING_PERIOD_MS     1000

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
 
PulseOximeter pox;
uint32_t tsLastReport = 0;
 
void onBeatDetected()
{
  Serial.println("Beat!");
  //We sensed a beat!
  long delta = millis() - lastBeat;
  lastBeat = millis();

  beatsPerMinute = 60 / (delta / 1000.0);

  if (beatsPerMinute < 255 && beatsPerMinute > 20)
  {
    rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
    rateSpot %= RATE_SIZE; //Wrap variable

    //Take average of readings
    beatAvg = 0;
    for (byte x = 0 ; x < RATE_SIZE ; x++)
      beatAvg += rates[x];
    beatAvg /= RATE_SIZE;
  }

  Serial.print("From the old method HR: ");
  Serial.println(beatAvg);
}
 
void setup()
{
    Serial.begin(115200);
    Serial.print("Initializing pulse oximeter..");

    if (!pox.begin()) {
      Serial.println("FAILED");
      for(;;);
      } else {
        Serial.println("SUCCESS");
        }
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
    pox.setOnBeatDetectedCallback(onBeatDetected);
}
 
void loop()
{
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      Serial.print("Heart rate:");
      Serial.print(pox.getHeartRate());
      Serial.print("bpm / SpO2:");
      Serial.print(pox.getSpO2());
      Serial.println("%");

      tsLastReport = millis();
    }
}