#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <mcp_can.h>
#include <SPI.h>
#include <math.h>

#define CAN_INTERRUPT_PIN 2
#define CAN_CONTROL_PIN 9

#define ANGLE_THRESHOLD 8
#define THROTTLE_THRESHOLD 1

#define FORWARD_PIN 7
#define BACK_PIN 6
#define LEFT_PIN 5
#define RIGHT_PIN 4
#define FIRE_PIN 8

Adafruit_BNO055 bno = Adafruit_BNO055();
MCP_CAN can(CAN_CONTROL_PIN);

uint8_t system_cal, gyro, accel, mag;

void recalibrate() {
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
    
  while (true) {
    bno.getCalibration(&system_cal, &gyro, &accel, &mag);
    if (gyro == 3) {
      digitalWrite(16, LOW);
    }
    if (accel == 3) {
      digitalWrite(17, LOW);
    }
    if (gyro == 3) {// && accel == 3) {
      digitalWrite(17, LOW);
      break;
    }
    delay(20);
  }
}

bool checkCalibration() {
  bno.getCalibration(&system_cal, &gyro, &accel, &mag);
  return gyro == 3; // && accel == 3;
}

void setup() {
  Serial.begin(9600);
  //detachInterrupt(digitalPinToInterrupt(3));

  Serial.println("Welcome to Akronyme Analogikers' Wirklichkeitssteuerungsgeraet!");
  Serial.println("Setting up Gyro...");

  /* Initialise the sensor */
  if(!bno.begin(Adafruit_BNO055::adafruit_bno055_opmode_t::OPERATION_MODE_IMUPLUS)) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  bno.setExtCrystalUse(true);
  recalibrate();

  Serial.println("Done setting up gyro. Continuing with CAN...");
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if (can.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("Error Initializing MCP2515...");
    while (1);
  }

  can.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
  Serial.println("MCP2515 Initialized Successfully!");

  pinMode(FORWARD_PIN, OUTPUT);
  pinMode(BACK_PIN, OUTPUT);
  pinMode(LEFT_PIN, OUTPUT);
  pinMode(RIGHT_PIN, OUTPUT);
  pinMode(FIRE_PIN, OUTPUT);

  digitalWrite(FORWARD_PIN, HIGH);
  digitalWrite(BACK_PIN, HIGH);
  digitalWrite(LEFT_PIN, HIGH);
  digitalWrite(RIGHT_PIN, HIGH);
  digitalWrite(FIRE_PIN, HIGH);

  Serial.println("Setup done! AMIIIIGAAA!");
}

unsigned char len = 0;
unsigned char data[] = {0,0,0,0,0,0,0,0};
unsigned long address = 0x0;

unsigned int counter = 0;

struct JoystickState {
  int fb;
  int lr;
  bool fire;
};

JoystickState state { 0, 0, false };

unsigned long next_ms_log = 0;
void loop() {
  uint8_t system, gyro, accel, mag;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  int newLr = 0;

  if(CAN_MSGAVAIL == can.checkReceive()) {
    can.readMsgBuf(&address, &len, data);      // Read data: len = data length, buf = data byte(s)
    if (address == 0x2d0) {
      state.fire = data[5] & 0x3;
    } else if (address == 0x294) {
      if (((unsigned int)data[6]) & 4) {
        state.fb = 1;
      } else if (state.fb == 1) {
        state.fb = 0;
      }
    } else if (address == 0x10c) {
      if ((unsigned int)data[6] > THROTTLE_THRESHOLD) {
        state.fb = -1;
      } else if (state.fb == -1) {
        state.fb = 0;
      }
    }
  }
  if (!checkCalibration()) {
    recalibrate();
  }
  digitalWrite(16, LOW);
  digitalWrite(17, LOW);
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  if (euler.z() > ANGLE_THRESHOLD) {
    digitalWrite(17, HIGH);
    state.lr = -1;
  } else if (euler.z() < -ANGLE_THRESHOLD) {
    digitalWrite(16, HIGH);
    state.lr = 1;
  } else {
    state.lr = 0;
  }
  /*unsigned long current_ms = millis();
  if (next_ms_log <= current_ms) {
    Serial.print("X: ");
    Serial.print(euler.x(), 4);
    Serial.print(" Y: ");
    Serial.print(euler.y(), 4);
    Serial.print(" Z: ");
    Serial.println(euler.z(), 4);
    next_ms_log = current_ms + 100;
  }*/
  
  digitalWrite(FORWARD_PIN, state.fb == -1 ? LOW : HIGH);
  digitalWrite(BACK_PIN, state.fb == 1 ? LOW : HIGH);

  digitalWrite(LEFT_PIN, state.lr == -1 ? LOW : HIGH);
  digitalWrite(RIGHT_PIN, state.lr == 1 ? LOW : HIGH);

  digitalWrite(FIRE_PIN, state.fire ? LOW : HIGH);
}