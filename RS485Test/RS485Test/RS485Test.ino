#define BAUD_RATE 115200


typedef struct RS485 {
  int txEn;
  int rxEn;
} RS485;


// Serial 3
RS485 port1;

// Serial 2
RS485 port2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(BAUD_RATE);
  Serial3.begin(BAUD_RATE);

  port1 = {3, 2};
  port2 = {26, 28};
  initRS485(port1);
  initRS485(port2);
  setRX(port1);
  setTX(port2);
}

void loop() {

  while(Serial.available()){
    char data = Serial.read();
    Serial2.write(data);
  }

  while(Serial3.available()){
    char data = Serial3.read();
    Serial.write(data);
  }
  delay(10);

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
