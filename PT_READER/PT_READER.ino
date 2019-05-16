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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial3.begin(BAUD_RATE);

  rs485Transmitter = {3, 2};
  initRS485(rs485Transmitter);
  setTX(rs485Transmitter);
  Serial.println("Initialized");
}

uint16_t readValues[ADC_COUNT];

void loop() {

  for(uint8_t i = 0; i<ADC_COUNT; i++){
    int adcValue = analogRead(i);
    uint16_t adc16Value = adcValue;
    Serial3.write(i);
    Serial3.write(adc16Value & LOWER_BITS);
    Serial3.write((adc16Value & UPPER_BITS) >> BITS_PER_BYTE);
  }
  delay(100);

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
