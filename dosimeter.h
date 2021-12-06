#ifndef _DOSIMETER_H_
#define _DOSIMETER_H_

#define M4011 (1)
#define J305  (2)

#define BUCKETS_PER_MINUTE (60)
#define BUCKET_WIDTH_IN_MS ((60l * 1000) / BUCKETS_PER_MINUTE)

class DosimeterType {
public:
    void begin(int pin, int type);
    void startRecording();
    void stopRecording();

    bool isValid();

    float getEquivalentDoseRate();
    float getEquivalentDose();

    int getCPM();
private:
};

extern DosimeterType Dosimeter;

#endif
