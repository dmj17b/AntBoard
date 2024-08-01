
#include <ArduinoBLE.h>
#include <Bot.cpp>
#include <Servo.h>


#define MY_UUID(val) ("D65D0396-" val "-4381-9985-653653CE831F")

#define L1 5
#define L2 6
#define R1 9
#define R2 10
#define AUX 0

char bleName[] = "BLE_receiver";
BLEService myService(MY_UUID("0000"));
BLECharacteristic commandChar(MY_UUID("0001"), BLERead | BLEWrite, 20);


//Function prototypes 
void connectHandler(BLEDevice central);
void disconnectHandler(BLEDevice central);
void characteristicUpdated(BLEDevice central, BLECharacteristic thisChar);

// Motor objects
Motor left_wheel(L1, L2);
Motor right_wheel(R1, R2);
Servo auxServo;

// Joystick values
int joystick_ud = 0;
int joystick_lr = 0;

// Global variables to store ID and Value
String commandID = "";
String commandValue = "";



void setup() {
  // Spin up serial monitor
  Serial.begin(9600);
  pinMode(AUX, OUTPUT);
  auxServo.attach(AUX);

  // Set up bluetooth service
  if (!BLE.begin()) {
    Serial.println("starting BLE failed");
    while (1);
  }
  // Change name of ble peripheral
  BLE.setDeviceName("AntBot1");
  BLE.setLocalName(bleName);
  BLE.setAdvertisedService(myService);
  myService.addCharacteristic(commandChar);
  BLE.addService(myService);
  commandChar.writeValue("0");
  commandChar.setEventHandler(BLEWritten, characteristicUpdated);
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLE.poll();

  // Check to see if an updated command has been sent
  if (commandID.length() > 0 && commandValue.length() > 0) {
    Serial.println(commandID);
    // Add your logic here to use the ID and Value
    // IF valueStr is an array, use comma delimiter to separate values
    String value1 = "";
    String value2 = "";
    if(commandValue.indexOf(",") != -1){
      int index = commandValue.indexOf(",");
      value1 = commandValue.substring(0, index);
      value2 = commandValue.substring(index+1, commandValue.length());
      joystick_lr = value1.toInt();
      joystick_ud = value2.toInt();
    }
    else{
      Serial.print("Command Value: ");
      Serial.println(commandValue);
    }

  }
  // Parse the command ID and Value
  if(commandID == "d0"){
    float turn_gain = 0.5;
    float speed_gain = 0.7;
    // Map the joystick values to motor duty cycles
    joystick_lr = map(joystick_lr, 0, 1023, -255, 255);
    joystick_ud = map(joystick_ud, 0, 1023, -255, 255);
    joystick_lr = int(joystick_lr * turn_gain);
    joystick_ud = int(joystick_ud * speed_gain);
    int left_wheel_duty = joystick_ud - joystick_lr;
    left_wheel_duty = constrain(left_wheel_duty, -255, 255);
    int right_wheel_duty = joystick_ud + joystick_lr;
    right_wheel_duty = constrain(right_wheel_duty, -255, 255);

    // Drive the motors
    left_wheel.drive(left_wheel_duty);
    right_wheel.drive(right_wheel_duty);
  }
  else if(commandID == "sl1"){
    int sliderVal = commandValue.toInt();
    sliderVal = map(sliderVal, 0, 100, 0, 180);
    auxServo.write(sliderVal);
  }
  else if(commandID == "sw0"){
    if(commandValue == "1"){
      auxServo.write(180);
    }
    else if(commandValue == "0"){
      auxServo.write(0);
    }
  }
    // Clear the global variables after use
    commandID = "";
    commandValue = "";
}



void characteristicUpdated(BLEDevice central, BLECharacteristic thisChar) {
  int length = thisChar.valueLength();
  const uint8_t* command = thisChar.value();

  if (length < 7) {
    Serial.println("Invalid command length");
    return;
  }

  if (command[0] != 1 || command[length-1] != 3) {
    Serial.println("Invalid command delimiters");
    return;
  }

  // Find the '2' delimiter
  int delimiterIndex = -1;
  for (int i = 1; i < length - 1; i++) {
    if (command[i] == 2) {
      delimiterIndex = i;
      break;
    }
  }
  if (delimiterIndex == -1) {
    Serial.println("Invalid command format, missing delimiter");
    return;
  }

  // Extract ID as a string
  String idStr = "";
  for (int i = 1; i < delimiterIndex; i++) {
    idStr += (char)command[i];
  }

  // Extract Value as a string
  String valueStr = "";
  for (int i = delimiterIndex + 1; i < length - 1; i++) {
    valueStr += (char)command[i];
  }

  // Store the extracted ID and Value in global variables
  commandID = idStr;
  commandValue = valueStr;

}
