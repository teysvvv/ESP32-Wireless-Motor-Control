#include <SPI.h>
#include <mcp2515.h>
#include <SmartDriver.h>
#include <WiFi.h>
#include <esp_now.h>

// CAN
MCP2515 mcp2515(5);

struct can_frame can_msg_transmit;
struct can_frame can_msg_receive;

uint8_t commandData[8];

SmartDriver motor1; // MOTOR 1 = DRIVE
SmartDriver motor2;// MOTOR 2 = STEERING

// Encoder / SmartDriver feedback
float motor1_feedback_speed = 0.0;
float motor1_feedback_position = 0.0;
float holdPosition = 0.0;
bool holding = false;
const int DEADZONE = 20;

float motor2_feedback_position = 0.0;

// Data received from ESP1
typedef struct {
  int yVal;
  int btnA;
  int btnB;
  int btnC;
  int btnD;
} ControlData;

ControlData controls;

// Desired commands
float driveSpeed = 0.0;
float steerPosition = 0.0;
unsigned long lastSteerTime = 0;
const unsigned long STEER_INTERVAL = 100;
const int CENTER_Y = 453;

//esp recieving
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(ControlData)) {

    // Copy the received ESP1 data into "controls"
    memcpy(&controls, incomingData, sizeof(controls));
  }
                }
void setup() {
  Serial.begin(115200);

  delay(1000);
  mcp2515.reset();
  mcp2515.setBitrate(
    CAN_1000KBPS,
    MCP_8MHZ
  );
  mcp2515.setNormalMode();
  WiFi.mode(WIFI_STA);

  delay(500);

  Serial.print("ESP2 MAC: ");
  Serial.println(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW ERROR");
   return;
  }
  esp_now_register_recv_cb(
    OnDataRecv
  );
  Serial.println("ESP2 READY");
  
  // void setup for recieving message from smart driver
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

void loop() {
 //motor1 
if (abs(controls.yVal - CENTER_Y) <= DEADZONE) {  //motor 1 holding position
  if (!holding) { 
    holdPosition = motor1_feedback_position; 
    holding = true; 
  } 
  motor1.positionMode = true; 
  motor1.speedMode = false;
  motor1.voltageMode = false; 
  motor1.goal = holdPosition; 
} else { //motor 1 moving
  holding = false; 
  motor1.positionMode = false; 
  motor1.speedMode = true; 
  motor1.voltageMode = false; 
  driveSpeed = map(controls.yVal, 0, 905, -5, 5); 
  motor1.goal = driveSpeed; 
} 
motor1.stop = false; 
motor1.reset = false;
motor1.getCommand(commandData); 

can_msg_transmit.can_id = 0x01; 
can_msg_transmit.can_dlc = 8; 

memcpy( 
  can_msg_transmit.data, 
  commandData, 
  8 
); 

mcp2515.sendMessage(&can_msg_transmit);

// MOTOR 2
if (millis() - lastSteerTime >= STEER_INTERVAL) {
  if (controls.btnA == 1) {
  steerPosition += 2.5;
}
else if (controls.btnB == 1) {
  steerPosition += 10.0;
}
else if (controls.btnC == 1) {
  steerPosition -= 2.5;
}
else if (controls.btnD == 1) {
  steerPosition -= 10.0;
}
  steerPosition = constrain(steerPosition, -270.0, 270.0);
  lastSteerTime = millis();

  motor2.positionMode = true;
  motor2.stop = false;
  motor2.reset = false;
  motor2.voltageMode = false;
  motor2.speedMode = false;
  motor2.goal = steerPosition * PI / 180.0;

  motor2.getCommand(commandData);

  can_msg_transmit.can_id = 0x02;
  can_msg_transmit.can_dlc = 8;

  memcpy(
    can_msg_transmit.data,
    commandData,
    8
  );
  mcp2515.sendMessage(&can_msg_transmit);
}
  delay(10);
  
  // void loop recieving message
  if (mcp2515.readMessage(&can_msg_receive) == MCP2515::ERROR_OK) {  
      if (can_msg_receive.can_id == 102){
        memcpy(&motor2_feedback_position, can_msg_receive.data, sizeof(float));
      }
      else if (can_msg_receive.can_id == 101) {
        memcpy(&motor1_feedback_position, can_msg_receive.data, sizeof(float));

        memcpy(&motor1_feedback_speed, can_msg_receive.data + 4, sizeof(float));
      }   
}
Serial.print("Drive target: ");
Serial.print(driveSpeed);

Serial.print(" | Actual speed: ");
Serial.print(motor1_feedback_speed);

Serial.print(" | Steering target: ");
Serial.print(steerPosition);
Serial.print(" deg");

Serial.print(" | Actual position: ");
Serial.println(motor2_feedback_position);
}





