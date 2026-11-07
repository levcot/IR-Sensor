#include "IR.h"


static const uint8_t IR_PINS[IR_SENSOR_COUNT] =
{
    A0, A1, A2, A3, A4, A5,
    A6, A7, A8, A9, A17
};

static uint16_t ir_min[IR_SENSOR_COUNT];
static uint16_t ir_max[IR_SENSOR_COUNT];

static int ir_lastPosition = 5000;

static bool ir_calibrated = false;

//SENSOR INITIALISIEREN

void IR_begin()
{
    analogReadResolution(12); 

    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        pinMode(IR_PINS[i], INPUT);

        ir_min[i] = 4095;
        ir_max[i] = 0;
    }

    ir_lastPosition = 5000;
    ir_calibrated = false;
}



//KALIBRIEREN
// Sensor währenddessen über Schwarz und Weiss bewegen

void IR_calibrate(unsigned long durationMs)
{
    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        ir_min[i] = 4095;
        ir_max[i] = 0;
    }

    unsigned long startTime = millis();

    while (millis() - startTime < durationMs)
    {
        for (int i = 0; i < IR_SENSOR_COUNT; i++)
        {
            uint16_t wert = analogRead(IR_PINS[i]);

            if (wert < ir_min[i])
                ir_min[i] = wert;

            if (wert > ir_max[i])
                ir_max[i] = wert;
        }

        delay(5);
    }

    ir_calibrated = true;
}

//ROHE ANALOGMESSWERTE AUSLESEN

void IR_readRaw(uint16_t values[IR_SENSOR_COUNT])
{
    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        values[i] = analogRead(IR_PINS[i]);
    }
}


//BERECHNETE STROMSTRÄKE AUSLESEN
// 0 = hell, 1000 = dunkel

static uint16_t IR_readDarkness(int index)
{
    uint16_t raw = analogRead(IR_PINS[index]);

    if (!ir_calibrated)
        return 0;

    int range = ir_max[index] - ir_min[index];

    if (range < 10)
        return 0;

    int darkness =
        ((int)raw - (int)ir_min[index]) * 1000 / range;

    darkness = constrain(darkness, 0, 1000);

    return (uint16_t)darkness;
}


//ÜBERPRÜFEN, OB EINE LINIE ERKANNT WIRD

bool IR_isLineDetected()
{
    if (!ir_calibrated)
        return false;

    uint32_t sum = 0;

    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        sum += IR_readDarkness(i);
    }

    return sum > 500;
}


// POSITION DER SCHWARZEN LINIE BERECHNEN
// Ergebnis: 0 bis 10000

int IR_getPosition()
{
    if (!ir_calibrated)
        return -1;

    uint32_t gewichteteSumme = 0;
    uint32_t summe = 0;

    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        uint16_t staerke = IR_readDarkness(i);

        if (staerke < 200)
            staerke = 0;

        gewichteteSumme += (uint32_t)staerke
                           * i * 1000;

        summe += staerke;
    }

    if (summe == 0)
        return -1;

    int position = gewichteteSumme / summe;

    ir_lastPosition = position;

    return position;
}
