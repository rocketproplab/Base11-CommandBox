

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
#define FAIL_TIMEOUT 4000

#define PT_READ_INERVAL 500

#define ADC_ACTIVE

#ifdef ADC_ACTIVE
#include <NAU7802.h>

NAU7802 adc = NAU7802();
#endif

// Maps the bits in the sent byte to the pins. Least significant bit is at index zero. 
// Make sure PIN_MAP is at least 8 elements long
static const int PIN_MAP[] = {VALVE_1, VALVE_2, VALVE_3, NO_VALVE, NO_VALVE, NO_VALVE, NO_VALVE, NO_VALVE};

static const char failPins = 0b001;

unsigned long lastRecive = -1;

void setup() {
  Serial1.begin(300);     // opens Serial1 port, sets data rate to 9600 bps
  Serial.begin(115200);

  Serial.println("Starting!");

  #ifdef ADC_ACTIVE
  // init the ADC_ACTIVE
  adc.begin();
  adc.selectCh1();
  #endif

  Serial.println("ADC Initialized!");

  // define any of the pins in PIN_MAP as output
  for (int i = 0; i < BITS_PER_CHAR; i++) {
    if (PIN_MAP[i] > 0) {
      pinMode(PIN_MAP[i], OUTPUT);
    }
  }

  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}

void loop() {
  char inputByte;

  // Read from the serail and look for sync header
  if (Serial1.available() >= PACKET_SIZE) {
    advanceSerial1Until(SYNC_HEADER);
    if (Serial1.available()) {
      inputByte = Serial1.read();
      updateLastRead();
      updatePins(inputByte);
    }
  }
  checkLastRead();

  #ifdef ADC_ACTIVE
  static unsigned long lastTime = millis();

  // read the adc and write the bytes to Serial1
  if (millis() - lastTime > PT_READ_INERVAL) {
    lastTime = millis();
    long adcValue = adc.readADC();
    Serial1.write(SYNC_HEADER);
    Serial1.write(sizeof(long));
    for (int i = 0; i < sizeof(long); i++) {
      Serial1.write(adcValue & LAST_BYTE);
      adcValue >>= BITS_PER_BYTE;
    }
  }
  #endif
}

/**
 * Advances the Serial1 until it has no more avaiable or it is one afer target
 * 
 * @param target what character to look for
 */
void advanceSerial1Until(char target) {
  while (Serial1.available()) {
    char readVal = Serial1.read();
    if (readVal == target) {
      return;
    }
  }
}

void updateLastRead(){
  lastRecive = millis();
}

void checkLastRead(){
  if(lastRecive == -1){
    return;
  }
  if((millis() - lastRecive) > FAIL_TIMEOUT){
    updatePins(failPins); 
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
