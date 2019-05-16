#define BAUD_RATE 115200
#define ADC_COUNT 16
#define LOWER_BITS 0x00FF
#define UPPER_BITS 0xFF00
#define BITS_PER_BYTE 8

#define ZERO_PSI_OFFSET 0.006 // V
#define HI_PSI_OFFSET 2.516 // V
#define PSI_AT_HI 500

#define FIVE_VOLT_MAX_READ 1024 // Sets per 5V
#define VOLTS_AT_MAX 5.0 //V
#define LO_PSI_VAL ( ZERO_PSI_OFFSET * FIVE_VOLT_MAX_READ / VOLTS_AT_MAX) // ADC value at 0 PSI
#define HI_PSI_VAL (HI_PSI_OFFSET * FIVE_VOLT_MAX_READ / VOLTS_AT_MAX) // ADC value at 500 PSI
#define PSI_OFFSET LO_PSI_VAL // b in y=m(x-a) regression
#define PSI_SLOPE (PSI_AT_HI/(HI_PSI_VAL-LO_PSI_VAL)) // m in y=m(x-a) regression


typedef struct RS485 {
  int txEn;
  int rxEn;
} RS485;


// Serial 2
RS485 rs485Reciver;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial3.begin(BAUD_RATE);

  rs485Reciver = {3, 2};
  initRS485(rs485Reciver);
  setRX(rs485Reciver);
  Serial.println(HI_PSI_VAL);
  Serial.print("TIME,\t");
  for(int i = 0; i<ADC_COUNT; i++){
    Serial.print("ADC");
    Serial.print(i);
    Serial.print(",\t");
  }
  Serial.println();
}

uint16_t readValues[ADC_COUNT];

void loop() {

  if(Serial3.available()){
    delay(10);
    decodeSerial(readValues);
    printValues(readValues);
  }

  
}

void decodeSerial(uint16_t * values){
  while(Serial3.available() >= 3){
    uint8_t id = Serial3.read();
    unsigned char lower = Serial3.read();
    unsigned char upper = Serial3.read();
    if(id >= 0 && id < ADC_COUNT){
      uint16_t value = lower | (upper << BITS_PER_BYTE);
      values[id] = value;
    } else {
      Serial.print("We got an illeagle value ");
      Serial.println((int)id);
    }
  }
  while(Serial3.available()){
    Serial3.read();
    Serial.println("Flushing buffer!");
  }
}

void printValues(uint16_t * values){
  Serial.print(millis());
  Serial.print(",\t");
  for(int i = 0; i<ADC_COUNT; i++){
    Serial.print(covertVoltageToPressure(values[i]));
    Serial.print(",\t");
  }
  Serial.println();
}

float covertVoltageToPressure(uint16_t value){
  return PSI_SLOPE * (value - PSI_OFFSET);
}

void initRS485(RS485 &port){
  pinMode(port.txEn, OUTPUT);
  pinMode(port.rxEn, OUTPUT);
}

void setRX(RS485 &port){
  digitalWrite(port.txEn, LOW);
  digitalWrite(port.rxEn, LOW);
}

void setTX(RS485 &port){
  digitalWrite(port.txEn, HIGH);
  digitalWrite(port.rxEn, HIGH);
}
