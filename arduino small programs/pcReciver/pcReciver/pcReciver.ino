#include <Thread.h>
#include <ThreadController.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(9, 10);
const byte addresses[][6] = {"00001", "00002"};
uint8_t unitAdress = 0;

// ThreadController that will controll all threads with multi threhads
ThreadController controll = ThreadController();
Thread radioListenerThread = Thread();
Thread computerListenerThread = Thread();

bool wasActivated = false;

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  Serial.begin(9600);
  radio.begin();
  // Configure Threads
  radioListenerThread.onRun(chackRadioForInput);
  radioListenerThread.setInterval(50);

  computerListenerThread.onRun(chackComputerForInput);
  computerListenerThread.setInterval(50);

  // Adds both threads to the controller
  controll.add(&radioListenerThread);
  controll.add(&computerListenerThread);
}
void chackRadioForInput() {

  /// Reading from pipe
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();
  if (radio.available())
  {
    digitalWrite(4, HIGH);
    uint32_t text = {0};
    radio.read(&text, sizeof(text));
    sendMsgToComputer(text);
    delay(200);
    digitalWrite(4, LOW);
  }
}
void chackComputerForInput() {

  if (Serial.available()) {

    digitalWrite(2, HIGH);
    uint32_t msg = getMsgFromSerial();;
    sendMsgToRadio(msg);
    delay(200);
    digitalWrite(2, LOW);

  }
}


uint32_t getMsgFromSerial() {
  byte buffer[sizeof(uint32_t)] = {0};
  uint32_t count = 0, msg = 0;
  while (0 < Serial.available()) buffer[count++] = Serial.read();
  msg = (*(uint32_t*)buffer);
  return msg;
}

/// To Fill Daniel
void sendMsgToComputer(uint32_t msg) {
  Serial.write((byte*)&msg, sizeof(msg));
//  Serial.flush();
}

/// To Fill by michael
void sendMsgToRadio(uint32_t msg) {
  radio.openWritingPipe(addresses[0]);
  radio.stopListening();
  radio.write(&msg, sizeof(msg));
}


/// nothing to do here yet, just calling thred
// handeller to run all thredes
void loop() {
  controll.run();
}




