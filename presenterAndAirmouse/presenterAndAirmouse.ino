#include <BleCombo.h>
#include <Adafruit_MPU6050.h>

#define LEFTBUTTON 33
#define RIGHTBUTTON 25
#define SWITCH_PIN 15
#define LED_BUILTIN 2


Adafruit_MPU6050 mpu;

//MPU Variables
bool sleepMPU = true;
unsigned int receivedValue = 0;
unsigned long lastMPURead = 0;
const int mpuInterval = 10;

//Motion Presenter Variables
unsigned long lastLeftBtnTime = 0;
unsigned long lastRightBtnTime = 0;
unsigned long lastGestureTime = 0;
const int gestureDelay = 1000;

//Air Mouse Variables
unsigned long lastLeftClickTime = 0;
unsigned long lastRightClickTime = 0;
unsigned long lastMouseMove = 0;
const int mouseInterval = 10;
const int FILTER_SIZE = 5;                
const float DEADZONE_X = 0.005;            
const float DEADZONE_Z = 0.005;            
const int BASE_SPEED = 2;             
const int MAX_SPEED = 60;             
const float X_SENSITIVITY = 1.8;          
const float Z_SENSITIVITY = 1.8;  
const float ACCEL_FACTOR = 1.5;   
float gyroXBuffer[FILTER_SIZE] = {0};
float gyroZBuffer[FILTER_SIZE] = {0};
int idxX = 0;
int idxZ = 0;

float movingAverage(float buffer[], float newValue, int size, int &idx) {
  buffer[idx] = newValue;
  float sum = 0;
  for (int i = 0; i < size; i++) sum += buffer[i];
  idx = (idx + 1) % size;
  return sum / size;
}

bool initMPU() {
  if (!mpu.begin()) {
    return false;
  }
  mpu.enableSleep(false);
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(LEFTBUTTON, INPUT_PULLUP);
  pinMode(RIGHTBUTTON, INPUT_PULLUP);
  pinMode(SWITCH_PIN, INPUT);

  Serial1.begin(9600, SERIAL_8N1, 16, 17);  //RX=16, TX=17
  // Serial2.begin(9600, SERIAL_8N1, 12, 13);  //RX=12, TX=13 -                     

  Keyboard.begin();
  Mouse.begin();         

  if (!initMPU()) {
    Serial.println("MPU init failed");
  }
}

void loop() {
 digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(500);                      // wait for a second
  // bool isAirMouseMode = digitalRead(SWITCH_PIN);      //true = Air Mouse, false = Motion Presenter

  // if (Keyboard.isConnected()) {
  //   if (Serial1.available()) {
  //     String msg = Serial1.readStringUntil('\n');
  //     Keyboard.print(msg+ ' '); 
  //     delay(100); 
  //     Keyboard.releaseAll();
  //   }

  //   if (Serial2.available() >= 2) {
  //     byte highByte = Serial2.read();
  //     byte lowByte = Serial2.read();

  //     receivedValue = (highByte << 8) | lowByte;

  //     if (receivedValue == 0xAA00) {
  //       Keyboard.press(KEY_RIGHT_ARROW);
  //       delay(100);
  //       Keyboard.releaseAll();
  //     } else if (receivedValue == 0xAA11) {
  //       Keyboard.press(KEY_LEFT_ARROW);
  //       delay(100);
  //       Keyboard.releaseAll();
  //     } else if (receivedValue == 0xAA22) {
  //       Keyboard.press(KEY_F5);
  //       delay(100);
  //       Keyboard.releaseAll();
  //     } else if (receivedValue == 0xAA33) {
  //       Keyboard.press(KEY_ESC);
  //       delay(100);
  //       Keyboard.releaseAll();
  //     }
  //   }
    
  //   if (sleepMPU) {
  //     sleepMPU = false;
  //     mpu.enableSleep(false);
  //   }

  //   unsigned long now = millis();

  //   if (now - lastMPURead >= mpuInterval) {
  //     lastMPURead = now;

  //     sensors_event_t a, g, temp;
  //     bool mpuOK = true;

  //     if (!mpu.getEvent(&a, &g, &temp)) {
  //       Wire.end();
  //       delay(10);
  //       Wire.begin();
  //       mpuOK = initMPU();
  //     }

  //     if (mpuOK) {
  //       //Air Mouse
  //       if (isAirMouseMode) {
  //         if (now - lastMouseMove >= mouseInterval) {
  //           lastMouseMove = now;

  //           float smoothZ = movingAverage(gyroZBuffer, g.gyro.z, FILTER_SIZE, idxZ);
  //           float smoothX = movingAverage(gyroXBuffer, g.gyro.x, FILTER_SIZE, idxX);

  //           if (fabs(smoothZ) < DEADZONE_Z) smoothZ = 0;
  //           if (fabs(smoothX) < DEADZONE_X) smoothX = 0;

  //           float magnitude = sqrt(smoothZ * smoothZ + smoothX * smoothX);
  //           float dynamicSpeed = BASE_SPEED * log(1 + magnitude * 15);

  //           if (dynamicSpeed < BASE_SPEED) dynamicSpeed = BASE_SPEED;
  //           if (dynamicSpeed > MAX_SPEED) dynamicSpeed = MAX_SPEED;

  //           int moveX = (int)(smoothZ * -dynamicSpeed * Z_SENSITIVITY);
  //           int moveY = (int)(smoothX * -dynamicSpeed * X_SENSITIVITY);

  //           if (moveX != 0 || moveY != 0) {
  //             Mouse.move(moveX, moveY);
  //           }
  //         }

  //         if (!digitalRead(LEFTBUTTON) && now - lastLeftClickTime > 300) {
  //           Mouse.click(MOUSE_LEFT);
  //           lastLeftClickTime = now;
  //         }
  //         if (!digitalRead(RIGHTBUTTON) && now - lastRightClickTime > 300) {
  //           Mouse.click(MOUSE_RIGHT);
  //           lastRightClickTime = now;
  //         }

  //       } 
  //       //Motion Presenter
  //       else {
  //         if (now - lastGestureTime > gestureDelay) {
  //           if (a.acceleration.x > 4) {
  //             Keyboard.press(KEY_RIGHT_ARROW);
  //             delay(100);
  //             Keyboard.release(KEY_RIGHT_ARROW);
  //             lastGestureTime = now;
  //           } else if (a.acceleration.x < -4) {
  //             Keyboard.press(KEY_LEFT_ARROW);
  //             delay(100);
  //             Keyboard.release(KEY_LEFT_ARROW);
  //             lastGestureTime = now;
  //           }
  //         }

  //         if (!digitalRead(LEFTBUTTON) && now - lastLeftBtnTime > 300) {
  //           Keyboard.press(KEY_LEFT_ARROW);
  //           delay(100);
  //           Keyboard.release(KEY_LEFT_ARROW);
  //           lastLeftBtnTime = now;
  //         }

  //         if (!digitalRead(RIGHTBUTTON) && now - lastRightBtnTime > 300) {
  //           Keyboard.press(KEY_RIGHT_ARROW);
  //           delay(100);
  //           Keyboard.release(KEY_RIGHT_ARROW);
  //           lastRightBtnTime = now;
  //         }
  //       }
  //     }
  //   }
  // }

  // delay(1);
}
