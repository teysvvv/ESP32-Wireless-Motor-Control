#include <WiFi.h>
#include <esp_now.h>
uint8_t receiverAddress[] = {0xFC, 0xE8, 0xC0, 0xC3, 0x54, 0x94}; 
#define JOY_Y      35
#define BTN_A      32 
#define BTN_C      25 
#define BTN_D      26 
#define BTN_B      33 

typedef struct { int yVal; int btnA; int btnB; int btnC; int btnD; } ControlData;

ControlData controls;

void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("WARNING: ESP-NOW DELIVERY FAILED!");
  }
}

void setup() {
  Serial.begin(115200);

  delay(1000);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_C, INPUT_PULLUP);
  pinMode(BTN_D, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);

  
  WiFi.mode(WIFI_STA);
  delay(500);
  // Start ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ERROR: ESP-NOW initialization failed!");
    return;
  }
  // Register send callback
  esp_now_register_send_cb(OnDataSent);

  // Add ESP32 #2 as peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("ERROR: Failed to add ESP32 #2!");
    return;
  }
  Serial.println();
  Serial.println("ESP32 #1 JOYSTICK TRANSMITTER");
  Serial.println("READY!");
}

void loop() {
  int rawY = analogRead(JOY_Y);
  controls.yVal = map( rawY, 0, 4095, 0, 1023 );
  typedef struct { int yVal; int btnA; int btnB; int btnC; int btnD; } ControlData;
  controls.btnA = !digitalRead(BTN_A);
  controls.btnC = !digitalRead(BTN_C);
  controls.btnD = !digitalRead(BTN_D);
  controls.btnB = !digitalRead(BTN_B);
  
  esp_err_t result = esp_now_send( receiverAddress, (uint8_t *)&controls, sizeof(controls) );
  
  Serial.print("Y: "); Serial.print(controls.yVal);
  Serial.print(" | A: "); Serial.print(controls.btnA);
  Serial.print(" | B: "); Serial.print(controls.btnB);
  Serial.print(" | C: "); Serial.print(controls.btnC);
  Serial.print(" | D: "); Serial.print(controls.btnD);
  if (result == ESP_OK) {
    Serial.println(" | SENT");
  
  } else {
    Serial.println(" | SEND ERROR");
  }
}