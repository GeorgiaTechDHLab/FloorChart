// Library for Arduino-to-Arduino communication (see https://www.arduino.cc/en/Tutorial/MasterWriter)
// Note on Mega: SDA/data pin is 20 and SCL/clock pin is 21
#include <Wire.h>
#define NODE_ADDRESS 1 // unique address of this I2C node
#define PAYLOAD_SIZE 4 // # bytes master I2C node receives

unsigned char nodePayload[PAYLOAD_SIZE]; // store data to send to master mega

// Library that interacts with the copper grid and stores the key states
#include <Keypad.h>

// ROWS cannot exceed 30 and COLS cannot exceed 30; otherwise, edit Keypad.h and change the max size 
const uint8_t ROWS = 30;
const uint8_t COLS = 15;


// Row pin assignments
unsigned char rowPins[ROWS] = {23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, // 1-15
                      22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50}; // 16-30
// Column pin assignments
unsigned char colPins[COLS] = {16, 15, 14, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}; //1-15

// Constructing the keypad
Keypad keypad = Keypad(rowPins, colPins, ROWS, COLS);


// Setup sets up the wires and initializes the i2c bus
void setup() {
  
  Serial.begin(9600); //Sets the speed of the serial port - I am not sure this is needed if we have the following line
  Serial.print(ROWS);
  Serial.print(COLS);
  Wire.begin(NODE_ADDRESS); // join i2c bus with NODE_ADDRESS
  Wire.onRequest(requestEvent); // function to call when master reads data (on master request)
}

//There loop continually updates the state of the keys
void loop(){
  keypad.getKeys();
}

// called when the master mega requests the next button event
void requestEvent(){
  
  // gets the next button change from the front of the queue
  unsigned long int buttonChange = keypad.getButtonChange();
  
  // Getting row, column, and type (pressed or released or none) from the button change int
  unsigned char row = (unsigned char)((buttonChange >> 24) & 0xFF);
  unsigned char col = (unsigned char)((buttonChange >> 16) & 0xFF);
  unsigned char type = (unsigned char)(buttonChange & 0xFF);
  
  // creating the payload to send to the master
  // The first  8 bytes are the address of this arduino, used to distinguish between slave megas
  // The second 8 bytes are the row of the button event
  // The third  8 bytes are the column of the button event
  // THe fourth 8 bytes are the type of the button event (pressed, released or none)
  nodePayload[0] = NODE_ADDRESS;
  nodePayload[1] = row;
  nodePayload[2] = col;
  nodePayload[3] = type;

  // Writing the payload to the i2c bus
  Wire.write(nodePayload, PAYLOAD_SIZE); // Wire.write takes (data, size) where size is in bytes
  if (type != 0) {
    //  Print the button event for debugging purposes
    Serial.println("Node Address:");
    Serial.println(NODE_ADDRESS);
    Serial.println("Row: ");
    Serial.println(nodePayload[1]);
    Serial.println("Col: ");
    Serial.println(nodePayload[2]);
    Serial.println("Type: ");
    Serial.println(nodePayload[3]);
    if (nodePayload[3] == 1) {
      Serial.println("Pressed");
    } else if (nodePayload[3] == 2) {
      Serial.println("Released");
    }
  }
}

