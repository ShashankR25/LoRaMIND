/**
 * @file WiFi_endNode.ino
 * @author Shashank R (@shashank-r25)
 * @brief
 * @version 3
 * @date 2023-04-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "MPU6050.h"
#include "I2Cdev.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

int MOTOR_SPEED = 75, motor_flag = 1, motor_status = 1;

// REPLACE WITH RECEIVER MAC Address
uint8_t senderAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#ifdef accelerometer
MPU6050 accelgyro(0x68);
MPU6050 accelgyro2(0x69);
#endif

#ifdef temperature
const int oneWireBus = 13;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
#endif

// Create a struct_message called myData
struct_message myData;
Speed_struct_message incomingReadings;

unsigned long lastTime = 0;
unsigned long timerDelay = 7000; // send readings timer

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t counter = 0;
int16_t x_max = -32768, x_min = 32767, y_max = -32768, y_min = 32767;

void ESP_OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  Serial.printf("Bytes received: %d\n", len);
  if (len == 4 && len <= 5)
  {
    int flag;
    memcpy(&flag, incomingData, sizeof(flag));
    if (flag == 1)
    {
#if ESP_DBG == 1
      Serial.println(" ESP_ACK SUCCESS - Transmisstor ");
#endif
    }
    else
    {
#if ESP_DBG == 1
      Serial.println(" ESP_ACK FAILED ");
#endif
    }
  }
  else
  {
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

    int Ack = 1;
    int result = esp_now_send(senderAddress, (uint8_t *)&Ack, sizeof(Ack));

    if (result == 0)
    {
#if ESP_DBG == 1
      Serial.println("ESP Data transmitted to GATEWAY Successfully");
#endif
    }
    else
    {
#if ESP_DBG == 1
      Serial.println("Error in sending the data to GATEWAY");
#endif
    }
    if (incomingReadings.value == "MOTOR")
    {
      MOTOR_SPEED = incomingReadings.motor_rotation_speed;
      motor_flag = 1;
    }
    else if (incomingReadings.value == "SWITCH")
    {
      if (incomingReadings.motor_rotation_speed)
      {
        motor_status = 1;
        digitalWrite(MOTOR_GND_PIN, LOW);
        digitalWrite(MOTOR_VCC_PIN, HIGH);
      }
      else if (!incomingReadings.motor_rotation_speed)
      {
        motor_status = 0;
        digitalWrite(MOTOR_GND_PIN, LOW);
        digitalWrite(MOTOR_VCC_PIN, LOW);
      }
    }
#if ESP_DBG == 1
    if (incomingReadings.value == "MOTOR")
      Serial.printf("\nReceived MOTOR SPEED = %d", MOTOR_SPEED);
    else
      Serial.printf("\nReceived MOTOR STATUS = %s", incomingReadings.motor_rotation_speed == 1 ? "ON" : "OFF");

#endif
  }
}

/**
 * @brief Callback when data is sent
 * 
 * @param mac_addr 
 * @param sendStatus 
 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Last Packet Send Status: ");
  Serial.print("\t");
  if (sendStatus == 0)
  {
    Serial.println("Delivery success");
  }
  else
  {
    Serial.println("Delivery fail");
  }
}

void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);
  Wire.begin();
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_network_ssid, wifi_network_password);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(ESP_OnDataRecv);
  //  esp_wifi_set_channel(3, WIFI_SECOND_CHAN_NONE);
  // Register peer
  //  esp_now_add_peer(senderAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

#ifdef accelerometer
  // Initialize the MPU6050 sensors
  accelgyro.initialize();
  Serial.println("Testing device connections...(0x68)");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  accelgyro2.initialize();
  Serial.println("Testing device connections...(0x69)");
  Serial.println(accelgyro2.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // accelgyro.setXAccelOffset(163);
  // accelgyro.setYAccelOffset(-3620);
  // accelgyro.setZAccelOffset(4236);
  // accelgyro.setXGyroOffset(69);
  // accelgyro.setYGyroOffset(-5);
  // accelgyro.setZGyroOffset(43);
#endif

  pinMode(MOTOR_CONTROL_PIN, OUTPUT);
  pinMode(MOTOR_VCC_PIN, OUTPUT);
  pinMode(MOTOR_GND_PIN, OUTPUT);
  Serial.println(WiFi.macAddress());

#ifdef temperature
  sensors.begin();
#endif

  digitalWrite(MOTOR_GND_PIN, LOW);
  digitalWrite(MOTOR_VCC_PIN, HIGH);
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    // Set values to send

    myData.motor_rotation_speed = MOTOR_SPEED;

#ifdef temperature
    sensors.requestTemperatures();
    Serial.println(sensors.getTempCByIndex(0));
#endif
    // Temperature in Celsius degrees

    myData.motor_x_max = x_max;
    myData.motor_y_max = y_max;
    myData.motor_x_min = x_min;
    myData.motor_y_min = y_min;
#ifdef temperature
    myData.motor_temperature = sensors.getTempCByIndex(0);
#endif
    counter = 0;
// Send message via ESP-NOW
#if ESP_DBG == 1
    Serial.printf("\nMotor Temperature : %d\
                   \nMotor Speed       : %d\
                   \nMotor_X_Max       : %d\
                   \nMotor_Y_Max       : %d\
                   \nMotor_X_Min       : %d\
                   \nMotor_Y_Min       : %d\n",
                  myData.motor_temperature, myData.motor_rotation_speed, myData.motor_x_max, myData.motor_y_max, myData.motor_x_min, myData.motor_y_min);

#endif
     esp_now_send(senderAddress, (uint8_t *)&myData, sizeof(myData));

    lastTime = millis();
    clear_acc();
  }
#ifdef accelerometer

  x_Max_acc(accelgyro.getAccelerationX());
  x_Min_acc(accelgyro.getAccelerationX());

  y_Max_acc(accelgyro2.getAccelerationY());
  y_Min_acc(accelgyro2.getAccelerationY());

#endif
  counter++;

  if (motor_flag == 1)
  {
    Serial.printf("MOTOR SPEED & MOTOR STATUS Before Setting: %d\n", MOTOR_SPEED);
    delay(50);
    if (MOTOR_SPEED == 0)
    {
      analogWrite(MOTOR_CONTROL_PIN, 0);
      //      digitalWrite(MOTOR_GND_PIN, LOW);
      //      digitalWrite(MOTOR_VCC_PIN, LOW);
    }
    else
    {
      //      digitalWrite(MOTOR_GND_PIN, LOW);
      //      digitalWrite(MOTOR_VCC_PIN, HIGH);
      analogWrite(MOTOR_CONTROL_PIN, map(MOTOR_SPEED, 1, 100, 80, 255));
    }
    motor_flag = 0;
  }
  delay(100);
}

void x_Max_acc(int16_t x_acc)
{
  x_max = (x_acc > x_max) ? x_acc : x_max;
}

void x_Min_acc(int16_t x_acc)
{
  x_min = (x_acc < x_min) ? x_acc : x_min;
}

void y_Max_acc(int16_t y_acc)
{
  y_max = (y_acc > y_max) ? y_acc : y_max;
}

void y_Min_acc(int16_t y_acc)
{
  y_min = (y_acc < y_min) ? y_acc : y_min;
}

void clear_acc()
{
  x_max = -32768;
  x_min = 32767;
  y_max = -32768;
  y_min = 32767;
}
