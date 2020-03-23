#ifndef THERMISTER_
#define THERMISTER_

#include <Arduino.h>
#include <Wire.h>

#define TEMP_PIN 0
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 10
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000


float readThermister() {
    static int samples[NUMSAMPLES];
    uint8_t i;

    float average = 0;

    for (i=0; i<NUMSAMPLES; ++i) {
        samples[i] = analogRead(TEMP_PIN);
        delay(10);
    }

    for (i=0; i<NUMSAMPLES; ++i) {
        average += samples[i];
    }
    average /= NUMSAMPLES;

    // convert the value to resistance
    average = 1023 / average - 1;
    average = SERIESRESISTOR / average;
    //Serial.print("Thermistor resistance ");
    //Serial.println(average);

    float steinhart;
    steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
    steinhart = log(steinhart);                  // ln(R/Ro)
    steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart -= 273.15;                         // convert to C

    //Serial.print("Temperature ");
    //Serial.print(steinhart);
    //Serial.println(" *C");

    return steinhart;
}

#endif // THERMISTER_
