#include <dosimeter.h>

#define TUBE_PIN (4)

void setup() {
  Serial.begin(9600);

  Dosimeter.begin(TUBE_PIN, J305);
  Dosimeter.startRecording();
}

void loop() {
  Serial.print("CPM: ");
  Serial.print(Dosimeter.getCPM());
  if (Dosimeter.isValid()) {
    Serial.println();
  } else {
    Serial.println(" (invalid)");
  }
  
  Serial.print("dose rate: ");
  Serial.print(Dosimeter.getEquivalentDoseRate());
  Serial.print(" µSv/h");
  if (Dosimeter.isValid()) {
    Serial.println();
  } else {
    Serial.println(" (invalid)");
  }

  Serial.print("dose: ");
  Serial.print(Dosimeter.getEquivalentDose());
  Serial.print(" µSv");
  if (Dosimeter.isValid()) {
    Serial.println();
  } else {
    Serial.println(" (invalid)");
  }
  
  delay(1000);
}
