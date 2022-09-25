#include <SoftwareSerial.h>

int LED_TRANSMIT = 2;
int LED_RECEIVE = 3;

int TX_PIN = 4;
int RX_PIN = 5;
int SET_PIN = 6;

SoftwareSerial HC12(TX_PIN,RX_PIN);

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  pinMode(LED_TRANSMIT, OUTPUT);
  pinMode(LED_RECEIVE, OUTPUT);
}

void loop() {
  if(HC12.available()) {
    receiveLight(true);
  }
}

void transmitLight(boolean light) {
  if (light == true) {
    digitalWrite(LED_TRANSMIT, HIGH);  
  } else {
    digitalWrite(LED_TRANSMIT, LOW);
  }
}

void receiveLight(boolean light) {
  if (light == true) {
    digitalWrite(LED_RECEIVE, HIGH);  
  } else {
    digitalWrite(LED_RECEIVE, LOW);
  }  
}

void checkCommand() {
  
  while (HC12.available()) {                    // While Arduino's HC12 soft serial rx buffer has data
    HC12ByteIn = HC12.read();                   // Store each character from rx buffer in byteIn
    HC12ReadBuffer += char(HC12ByteIn);         // Write each character of byteIn to HC12ReadBuffer
    if (HC12ByteIn == '\n') {                   // At the end of the line
      HC12End = true;                           // Set HC12End flag to true
    }
  }

  while (Serial.available()) {                  // If Arduino's computer rx buffer has data
    SerialByteIn = Serial.read();               // Store each character in byteIn
    SerialReadBuffer += char(SerialByteIn);     // Write each character of byteIn to SerialReadBuffer
    if (SerialByteIn == '\n') {                 // Check to see if at the end of the line
      SerialEnd = true;                         // Set SerialEnd flag to indicate end of line
    }
  }

  if (SerialEnd) {                              // Check to see if SerialEnd flag is true
    if (SerialReadBuffer.startsWith("AT")) {    // Has a command been sent from local computer
      HC12.print(SerialReadBuffer);             // Send local command to remote HC12 before changing settings
      delay(100);                               //
      digitalWrite(HC12SetPin, LOW);            // Enter command mode
      delay(100);                               // Allow chip time to enter command mode
      Serial.print(SerialReadBuffer);           // Echo command to serial
      HC12.print(SerialReadBuffer);             // Send command to local HC12
      delay(500);                               // Wait 0.5s for a response
      digitalWrite(HC12SetPin, HIGH);           // Exit command / enter transparent mode
      delay(100);                               // Delay before proceeding
    } else {
      HC12.print(SerialReadBuffer);             // Transmit non-command message
    }
    SerialReadBuffer = "";                      // Clear SerialReadBuffer
    SerialEnd = false;                          // Reset serial end of line flag
  }

  if (HC12End) {                                // If HC12End flag is true
    if (HC12ReadBuffer.startsWith("AT")) {      // Check to see if a command is received from remote
      digitalWrite(HC12SetPin, LOW);            // Enter command mode
      delay(100);                               // Delay before sending command
      Serial.print(SerialReadBuffer);           // Echo command to serial.
      HC12.print(HC12ReadBuffer);               // Write command to local HC12
      delay(500);                               // Wait 0.5 s for reply
      digitalWrite(HC12SetPin, HIGH);           // Exit command / enter transparent mode
      delay(100);                               // Delay before proceeding
      HC12.println("Remote Command Executed");  // Acknowledge execution
    } else {
      Serial.print(HC12ReadBuffer);             // Send message to screen
    }
    HC12ReadBuffer = "";                        // Empty buffer
    HC12End = false;                            // Reset flag
  }
}
