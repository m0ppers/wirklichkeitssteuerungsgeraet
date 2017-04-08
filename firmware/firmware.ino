#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <Wire.h>
#include <mcp_can.h>
#include <SPI.h>

// mpu code parts shamelessy stolen and adapted:

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#define CAN_INTERRUPT_PIN 2
#define CAN_CONTROL_PIN 9

#define MPU_INTERRUPT_PIN 3

#define ANGLE_THRESHOLD 6
#define THROTTLE_THRESHOLD 1

#define FORWARD_PIN 7
#define BACK_PIN 6
#define LEFT_PIN 5
#define RIGHT_PIN 4
#define FIRE_PIN 8

MCP_CAN can(CAN_CONTROL_PIN);

MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
VectorFloat gravity;    // [x, y, z]            gravity vector
Quaternion q;           // [w, x, y, z]         quaternion container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

bool setupMpu() {
  
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

  mpu.initialize();
  pinMode(MPU_INTERRUPT_PIN, INPUT);

  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(85);
  mpu.setYGyroOffset(49);
  mpu.setZGyroOffset(-32);
  mpu.setXAccelOffset(-1563);
  mpu.setYAccelOffset(-1226);
  mpu.setZAccelOffset(1316);

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(85);
  mpu.setYGyroOffset(49);
  mpu.setZGyroOffset(-32);
  mpu.setXAccelOffset(-1563);
  mpu.setYAccelOffset(-1226);
  mpu.setZAccelOffset(1316);

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);

      // enable Arduino interrupt detection
      //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
      attachInterrupt(digitalPinToInterrupt(MPU_INTERRUPT_PIN), dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      //Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();

      return true;
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));

      return false;
  }
}

void setup() {
  Serial.begin(9600);
  
  Serial.println("Welcome to Akronyme Analogikers' Wirklichkeitssteuerungsgeraet!");
  Serial.println("Setting up MPU...");

  if (!setupMpu()) {
    Serial.println("Failed to setup MPU...");
    while (1);
  }
  
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

void updateYPR() {
  //Serial.println("updating ypr");
  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));
  // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    
    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  }
}

unsigned long next_ms_log = 0;
float angle = 0;
void loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  if (mpuInterrupt || fifoCount >= packetSize) {
    updateYPR();
  }
  angle = ypr[1] *  180/M_PI;
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

  auto left_led = LOW;
  auto right_led = LOW;
  if (angle > ANGLE_THRESHOLD) {
    right_led = HIGH;
    state.lr = 1;
  } else if (angle < -ANGLE_THRESHOLD) {
    left_led = HIGH;
    state.lr = -1;
  } else {
    state.lr = 0;
  }
  digitalWrite(16, right_led);
  digitalWrite(17, left_led);
  unsigned long current_ms = millis();
  if (next_ms_log <= current_ms) {
    Serial.print("FB: ");
    Serial.print(state.fb);
    Serial.print(" LR: ");
    Serial.print(state.lr);
    Serial.print(" F: ");
    Serial.println(state.fire);
    next_ms_log = current_ms + 100;
  }
  
  digitalWrite(FORWARD_PIN, state.fb == -1 ? LOW : HIGH);
  digitalWrite(BACK_PIN, state.fb == 1 ? LOW : HIGH);

  digitalWrite(LEFT_PIN, state.lr == -1 ? LOW : HIGH);
  digitalWrite(RIGHT_PIN, state.lr == 1 ? LOW : HIGH);

  digitalWrite(FIRE_PIN, state.fire ? LOW : HIGH);
}
