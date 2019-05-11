#include "Wire.h"

#define VALVE_1_ID 0b00000001
#define VALVE_2_ID 0b00000010
#define VALVE_3_ID 0b00000100

#define SYNC_HEADER 0b11001111

#define VALVE_1_PIN 0
#define VALVE_2_PIN 1
#define VALVE_3_PIN 2

#define VALVE_SEND_TIME 100

#define BITS_PER_BYTE 8

void sendValveState(bool valve1, bool valve2, bool valve3);
int switches[21];
int lastSwitches[21];

void setup()
{
  Wire.begin();
  for (int i = 0; i < 21; i++) {
    pinMode(i + 34, INPUT);
  }
  Serial.begin(115200);
  Serial1.begin(300);
  Serial.println("setup() complete; dab");
}

void loop()
{
  for (int i = 0; i < 21; i++) {
    switches[i] = !digitalRead(i + 34);
  }

  bool printThem = false;

  for (int i = 0; i < 21; i++) {
    if (switches[i] != lastSwitches[i]) {
      printThem = true;
      lastSwitches[i] = switches[i];
    }
  }

  if (printThem) {
    for (int i = 0; i < 21; i++) {
      Serial.print(switches[i]);
    }
    Serial.println();

  }

  static unsigned long lastTime = millis();


  if (millis() - lastTime > VALVE_SEND_TIME) {
    readValvesFromSwitches(switches);
    lastTime = millis();
  }

  readADCInput();
}

void readADCInput() {
  if(advanceSerialUntil(SYNC_HEADER)){
    delay(100);
  }
  if (Serial1.available() >= 1) {
    delay(100);
    char packetLen = Serial1.read();
    unsigned long adcValue = 0;
    if (Serial1.available() < packetLen) {
      return;
    }
    for (int i = 0; i < packetLen; i++) {
      adcValue <<= BITS_PER_BYTE;
      adcValue |= Serial1.read();
    }
    Serial.print("ADC Value: ");
    Serial.println(adcValue);
  }
}

/**
   Advances the serial until it has no more avaiable or it is one afer target

   @param target what character to look for
*/
int advanceSerialUntil(char target) {
  while (Serial1.available()) {
    char readChar = Serial1.read();
    if (readChar == target) {
      return 1;
    }
  }
  return 0;
}

void readValvesFromSwitches(int * switches) {
  bool valve1 = switches[VALVE_1_PIN] > 0;
  bool valve2 = switches[VALVE_2_PIN] > 0;
  bool valve3 = switches[VALVE_3_PIN] > 0;
  sendValveState(valve1, valve2, valve3);
}

void sendValveState(bool valve1, bool valve2, bool valve3) {
  char data = valve1 ? VALVE_1_ID : 0;
  data |= valve2 ? VALVE_2_ID : 0;
  data |= valve3 ? VALVE_3_ID : 0;
  Serial1.write(SYNC_HEADER);
  Serial1.write(data);
}
