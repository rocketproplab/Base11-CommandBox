

#define BITS_PER_CHAR 8

#define VALVE_1 2
#define VALVE_2 3
#define VALVE_3 4
#define NO_VALVE -1

#define LAST_BIT_MASK 0b00000001
#define LAST_BYTE 0xFF
#define BITS_PER_BYTE 8

#define SYNC_HEADER 0b11001111
#define PACKET_SIZE 2

#define PT_READ_INERVAL 500

//#define ADC

#ifdef ADC
#include <NAU7802.h>

NAU7802 adc = NAU7802();
#endif

// Maps the bits in the sent byte to the pins. Least significant bit is at index zero. 
// Make sure PIN_MAP is at least 8 elements long
static const int PIN_MAP[] = {VALVE_1, VALVE_2, VALVE_3, NO_VALVE, NO_VALVE, NO_VALVE, NO_VALVE, NO_VALVE};

void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps

  #ifdef ADC
  // init the ADC
  adc.begin();
  adc.selectCh1();
  #endif

  // define any of the pins in PIN_MAP as output
  for (int i = 0; i < BITS_PER_CHAR; i++) {
    if (PIN_MAP[i] > 0) {
      pinMode(PIN_MAP[i], OUTPUT);
    }
  }
}

void loop() {
  char inputByte;

  // Read from the serail and look for sync header
  if (Serial.available() >= PACKET_SIZE) {
    advanceSerialUntil(SYNC_HEADER);
    if (Serial.available()) {
      inputByte = Serial.read();
      updatePins(inputByte);
    }
  }

  #ifdef ADC
  static unsigned long lastTime = millis();

  // read the adc and write the bytes to serial
  if (millis() - lastTime > PT_READ_INERVAL) {
    lastTime = millis();
    long adcValue = adc.readADC();
    Serial.write(SYNC_HEADER);
    Serial.write(sizeof(long));
    for (int i = 0; i < sizeof(long); i++) {
      Serial.write(adcValue & LAST_BYTE);
      adcValue >>= BITS_PER_BYTE;
    }
  }
  #endif
}

/**
 * Advances the serial until it has no more avaiable or it is one afer target
 * 
 * @param target what character to look for
 */
void advanceSerialUntil(char target) {
  while (Serial.available()) {
    if (Serial.read() == target) {
      return;
    }
  }
}

/**
 * Sets the pins given the charcter
 */
void updatePins(char data) {
  int lastBit = 0;
  for (int i = 0; i < BITS_PER_CHAR; i++) {
    lastBit = data & LAST_BIT_MASK;
    data >>= 1;
    if (PIN_MAP[i] < 0) {
      continue;
    }
    digitalWrite(PIN_MAP[i], lastBit);
  }
}
