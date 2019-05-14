#define BAUD_RATE 115200
#define ADC_COUNT 16
#define LOWER_BITS 0x00FF
#define UPPER_BITS 0xFF00
#define BITS_PER_BYTE 8


typedef struct RS485 {
  int txEn;
  int rxEn;
} RS485;


// Serial 3
RS485 rs485Transmitter;

// Serial 2
RS485 port2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(BAUD_RATE);
  Serial3.begin(BAUD_RATE);

  rs485Transmitter = {3, 2};
  port2 = {26, 28};
  initRS485(rs485Transmitter);
  initRS485(port2);
  setTX(rs485Transmitter);
  setRX(port2);
  Serial.println("Initialized");
}

uint16_t readValues[ADC_COUNT];

void loop() {

  if(Serial2.available()){
    decodeSerial(readValues);
    printValues(readValues);
  }
  
  for(uint8_t i = 0; i<ADC_COUNT; i++){
    int adcValue = analogRead(i);
    uint16_t adc16Value = adcValue;
    Serial3.write(i);
    Serial3.write(adc16Value & LOWER_BITS);
    Serial3.write((adc16Value & UPPER_BITS) >> BITS_PER_BYTE);
  }
  delay(100);

}

void decodeSerial(uint16_t * values){
  while(Serial2.available() >= 3){
    uint8_t id = Serial2.read();
    unsigned char lower = Serial2.read();
    unsigned char upper = Serial2.read();
    if(id >= 0 && id < ADC_COUNT){
      uint16_t value = lower | (upper << BITS_PER_BYTE);
      values[id] = value;
    } else {
      Serial.print("We got an illeagle value ");
      Serial.print((int)id);
    }
  }
  while(Serial2.available()){
    Serial2.read();
    Serial.println("Flushing buffer!");
  }
}

void printValues(uint16_t * values){
  for(int i = 0; i<ADC_COUNT; i++){
    Serial.print("ADC ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(values[i]);
    Serial.print("\t");
    if(i%4==3){
      Serial.println();
    }
  }
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
