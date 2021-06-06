/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017
   -------------------------------------------------------------------------------------
*/

/*
   Settling time (number of samples) and data filtering can be adjusted in the config.h file
   For calibration and storing the calibration value in eeprom, see example file "Calibration.ino"

   The update() function checks for new data and starts the next conversion. In order to acheive maximum effective
   sample rate, update() should be called at least as often as the HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS.
   If you have other time consuming code running (i.e. a graphical LCD), consider calling update() from an interrupt routine,
   see example file "Read_1x_load_cell_interrupt_driven.ino".

   This is an example sketch on how to use this library
*/

/*
  einfache Testversion, liest Wegsensor auf Analog Input #0 und Kaft vom HX711 in einer Schleife.
  Die Daten werden in ein Array geschrieben und erst wenn der voll ist im seriellen Monitor ausgegeben, csv Format mit Semikolon ; so lange, bis Taste 'q' gedrückt wird.
*/

#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

#define BAUDRATE 115200
#define MAX_SAMP 100 // mehr geht offenbar nicht wg. Speicherplatz !!

//pins:
const int HX711_dout = 4; //mcu > HX711 dout pin    aus dammi0.ino WJ
const int HX711_sck = 5; //mcu > HX711 sck pin    aus dammi0.ino WJ
const int DIST_SENSOR = 0;  // Wegsensor, analog input #0
// constants:
const unsigned long stabilizingtime = 1000; // tare preciscion can be improved by adding a few seconds of stabilizing time
const int calVal_calVal_eepromAdress = 0;
const int serialPrintInterval = 200; //increase value to slow down serial print activity, default = 500 = 0.5 sec, 200 = 5 Hz Abtastrate
// globals:
unsigned long t = 0;

unsigned long ticks[MAX_SAMP];
float force[MAX_SAMP];
int distance[MAX_SAMP];

int i = 0;
int nsamp = 0;

/*
const int MAX_SIZE = 64;
char lines [MAX_SAMP] [MAX_SIZE]; // braucht noch mehr Speicher :-/
*/
        
//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

void setup() {
    Serial.begin(BAUDRATE); delay(10);
    Serial.println();
    Serial.println("Starting...");

    float calibrationValue; // calibration value
    calibrationValue = 22.75; // uncomment this if you want to set this value in the sketch
    #if defined(ESP8266) || defined(ESP32)
    //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch this value from eeprom
    #endif
    //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch this value from eeprom

    LoadCell.begin();
    boolean _tare = false; //set this to false if you don't want tare to be performed in the next step
    LoadCell.start(stabilizingtime, _tare);
    if (LoadCell.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    }    else {
        LoadCell.setCalFactor(calibrationValue); // set calibration factor (float)
        Serial.println("Startup is complete");
    }
    while (!LoadCell.update());
    Serial.print("Calibration value: ");
    Serial.println(LoadCell.getCalFactor());
    Serial.print("HX711 measured conversion time ms: ");
    Serial.println(LoadCell.getConversionTime());
    Serial.print("HX711 measured sampling rate HZ: ");
    Serial.println(LoadCell.getSPS());
    Serial.print("HX711 measured settlingtime ms: ");
    Serial.println(LoadCell.getSettlingTime());
    Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
    if (LoadCell.getSPS() < 7) {
        Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
    }    else if (LoadCell.getSPS() > 100) {
        Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
    }
    Serial.println("Beenden mit Taste 'q'");
    //  Serial.println("Zeit;Kraft;Weg");
}

void loop() {
    static boolean newDataReady = 0;
    float load;
    int dist;
    int k;
    //  char line[MAX_SIZE];
    
    // check for new data/start next conversion:
    if (LoadCell.update()) newDataReady = true;

    // get smoothed value from the dataset:
    if (newDataReady) {
        if (millis() > t + serialPrintInterval) {
            load = LoadCell.getData();
            dist = analogRead(DIST_SENSOR);
            t = millis();
            //  snprintf(lines[i], MAX_SIZE, "%d;%f;%d\n", t, load, dist);
            //  snprintf(lines[i], MAX_SIZE, "%s", line);
            ticks[i] = t;
            force[i] = load;
            distance[i] = dist;
            /*
            Serial.print(millis());
            Serial.print(";");
            Serial.print(load);
            Serial.print(";");
            Serial.println(dist);
             */
            newDataReady = 0;
            i++;
            nsamp = i;
        }
    }

    if (i >= MAX_SAMP) {
      Serial.println("Zeit;Kraft;Weg");
      for (k=0;k < MAX_SAMP;k++)  {
        Serial.print(ticks[k]);
        Serial.print(";");
        Serial.print(force[k]);
        Serial.print(";");
        Serial.println(distance[k]);
      }
      i = 0;  // restart from index 0
    }
    // receive command from serial terminal, send 't' to initiate tare operation:
    if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte == 't') {
          LoadCell.tareNoDelay();
          // check if last tare operation is complete:
          if (LoadCell.getTareStatus() == true) {
              Serial.println("Tare complete");
          }
        }
        if (inByte == 'q') {   // ende auf Taste 'q', vorher Arrays ausgeben
          for (k=0;k < nsamp;k++)  {
            Serial.print(ticks[k]);
            Serial.print(";");
            Serial.print(force[k]);
            Serial.print(";");
            Serial.println(distance[k]);
          } 
          exit(0);
        }
    }
}
