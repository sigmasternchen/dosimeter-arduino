#include "dosimeter.h"

#include <Arduino.h>

#include <ESP8266TimerInterrupt.h>

DosimeterType Dosimeter;

static float getCalibrationFactor(int type) {
    switch(type) {
        case J305:
        case M4011:
            return 0.0065;
            break;
        default:
            return NAN;
    }
}

int type;
volatile int buckets[BUCKETS_PER_MINUTE] = {0};
int last = 0;
bool valid = false;
bool measureDose = false;
volatile float dose = 0;

ESP8266Timer timer;

static inline void clearISR() {
    unsigned long gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
}

static void ICACHE_RAM_ATTR geigerISR() {
    int next = (millis() / BUCKET_WIDTH_IN_MS) % BUCKETS_PER_MINUTE;

    if (last != next) {
        for (int i = (last + 1) % BUCKETS_PER_MINUTE; i != next; i = (i + 1) % BUCKETS_PER_MINUTE) {
            buckets[i] = 0;
        }

        buckets[next] = 1;
        last = next;
    } else {
        buckets[next]++;
    }

    clearISR();
}

// timerISR gets called every minute
static void ICACHE_RAM_ATTR timerISR() {
    if (measureDose && valid) {
        dose += Dosimeter.getEquivalentDoseRate() / 60;
    }
}

void DosimeterType::begin(int pin, int _type) {
    type = _type;

    pinMode(pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(pin), geigerISR, RISING);

    timer.attachInterruptInterval(60l * 1000 * 1000, timerISR);
}

int DosimeterType::getCPM() {
    int result = 0;
    for (int i = 0; i < BUCKETS_PER_MINUTE; i++) {
        result += buckets[i];
    }
    return result;
}

float DosimeterType::getEquivalentDoseRate() {
    return getCPM() * getCalibrationFactor(type);
}

float DosimeterType::getEquivalentDose() {
    if (measureDose) {
        return dose;
    } else {
        return NAN;
    }
}

bool DosimeterType::isValid() {
    if (valid) {
        return true;
    } else {
        valid = millis() > (60l * 1000);
        return valid;
    }
}

void DosimeterType::startRecording() {
    dose = 0;
    measureDose = true;
}

void DosimeterType::stopRecording() {
    measureDose = false;
}
