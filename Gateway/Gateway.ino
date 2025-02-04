/**
 * @file Gateway.ino
 * @author Shashank R (you@domain.com)
 * @brief
 * @version 2
 * @date 2023-04-12
 *
 * @copyright Copyright (c) 2023
 *
 */
//================================================================================================================================================================================================
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PubSubClient.h>
// #include <WiFiClientSecure.h>
#include <SPI.h>
#include <LoRa.h>
#include "config_v1.h"
#include "lora_code.h"
#include "wifi_code.h"

// Creating two objects from WiFiClient
WiFiClient espClient1;
WiFiClient espClient2;

// Creating two objects from PubSubClient
PubSubClient Wifi(espClient1);
PubSubClient Lora(espClient2);

char *rpc_value;
int motor_rotation_speed = 75; // initial motor rotation speed
int motor_state = 0;
int RPC_flag = 0, LORA_FLAG = 0;
int counter = 0, LoRa_counter = 0;

// Create a struct_message called WiFi_data, LoRa_data
WiFi_struct_message WiFi_data;
LoRa_struct_message LoRa_data;
Speed_struct_message Speed_data;

String LoRaData = "";

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// Callback function that will be executed when data is received
void ESP_OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{

  if (len == 4 && len <= 5)
  {
    int flag;
    memcpy(&flag, incomingData, sizeof(flag));
    if (flag == 1)
    {
#if ESP_NOW == 1
      Serial.println(" RPC SUCCESS - Gateway ");
#endif
      RPC_flag = 0;
      counter = 0;
    }
    else
    {
#if ESP_NOW == 1
      Serial.println(" RPC FAILED - Gateway ");
#endif
    }
  }
  else
  {

    memcpy(&WiFi_data, incomingData, sizeof(WiFi_data));
#if ESP_NOW == 1
    Serial.print("Bytes received: ");
#endif
    int Ack = 1;
    int result = esp_now_send(broadcastAddress, (uint8_t *)&Ack, sizeof(Ack));

    if (result == 0)
    {
#if ESP_NOW == 1
      Serial.println("Sent with success");
#endif
    }
    else
    {
#if ESP_NOW == 1
      Serial.println("Error sending the data");
#endif
    }
#if ESP_NOW == 1
    Serial.printf("\nMotor Temperature : %d\
                   \nMotor Speed       : %d\
                   \nMotor_X_Max       : %d\
                   \nMotor_Y_Max       : %d\
                   \nMotor_X_Min       : %d\
                   \nMotor_Y_Min       : %d\n",
                  WiFi_data.motor_temperature, WiFi_data.motor_rotation_speed, WiFi_data.motor_x_max, WiFi_data.motor_y_max, WiFi_data.motor_x_min, WiFi_data.motor_y_min);
#endif
    String payload_str = "{\"motor_temp\":\"" + String(WiFi_data.motor_temperature) + "\",\"motor_x_1\":\"" + String(WiFi_data.motor_x_max) + "\",\"motor_x_2\":\"" + String(WiFi_data.motor_x_min) + "\",\"motor_y_1\":\"" + String(WiFi_data.motor_y_max) + "\",\"motor_y_2\":\"" + String(WiFi_data.motor_y_min) + "\"}";
//    Serial.printf("\nPAYLOAD = %s\n", payload_str.c_str());
    Wifi.publish("v1/devices/me/telemetry", payload_str.c_str());
    motor_rotation_speed = WiFi_data.motor_rotation_speed;
  }
}

void RPC_callback(char *topic, uint8_t *message, unsigned int length)
{
  String send_rpc_data;
  String messageTemp;
  String RPCdata = (char *)message;
#if ESP_NOW == 1
  
  Serial.println(RPCdata);
  Serial.println();
  Serial.printf("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println(". Message: ");
  Serial.println(RPCdata.substring(11, 19));
#endif

  if (String(RPCdata.substring(11, 19)) == "getValue")
  {
    String temp = String(topic).substring(26);
    send_rpc_data = "v1/devices/me/rpc/response/" + temp;
    Serial.printf("\nMOTOR SPEED Pushing to server : %d\n", motor_rotation_speed);
    Wifi.publish(send_rpc_data.c_str(), String(motor_rotation_speed).c_str());
  }
  else if (String(RPCdata.substring(11, 19)) == "setValue")
  {
    motor_rotation_speed = RPCdata.substring(30, 35).toInt();
    Serial.printf("\nMOTOR SPEED receiving from server : %d\n", motor_rotation_speed);
    Speed_data.motor_rotation_speed = motor_rotation_speed;
    Speed_data.value = "MOTOR";
    esp_now_send(broadcastAddress, (uint8_t *)&Speed_data, sizeof(Speed_data));
    RPC_flag = 1;
    
  }
  else if (String(RPCdata.substring(11, 19)) == "G-Switch")
  {
    String temp = String(topic).substring(26);
    send_rpc_data = "v1/devices/me/rpc/response/" + temp;
    Serial.println(motor_state ? 1 : 0);
    Serial.printf("\nMOTOR STATUS setting to server : %d\n", motor_state ? 1 : 0);
    String payload = "{\"method\":\"switch_status\", \"params\":\"" + String(motor_state ? 1 : 0) + "\"}";
    String payload2 = "{\"method\":\"G-Switch\", \"params\":\"" + String(motor_state ? 1 : 0) + "\", \"additionalInfo\":null" + "}";
    Wifi.publish(send_rpc_data.c_str(), payload2.c_str());
    Wifi.publish(send_rpc_data.c_str(), payload.c_str());
  }
  else if (String(RPCdata.substring(11, 19)) == "S-Switch")
  {
    Serial.println(" STATE - " + RPCdata.substring(30, 31));
    motor_state = RPCdata.substring(30, 31).toInt();
    Speed_data.value = "SWITCH";
    Speed_data.motor_rotation_speed = motor_state;
    esp_now_send(broadcastAddress, (uint8_t *)&Speed_data, sizeof(Speed_data));
    RPC_flag = 1;
    Serial.printf("\nMOTOR STATUS receiving from server : %d\n", motor_state);
  }
}

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0)
  {
    //    Serial.print(WiFi_data.motor_temperature);
    Serial.print("\t");
    Serial.println("Delivery success");
  }
  else
  {
    Serial.println("Delivery fail");
  }
}

void reconnect(int value)
{
  // Loop until we're reconnected

  switch (value)
  {
  case WIFI:
    while (!Wifi.connected())
    {
      Serial.print("Attempting MQTT - WiFi connection...");
      // Attempt to connect
      if (Wifi.connect("clientId", ESP_WIFI_TOKEN, ""))
      {
        Serial.println("connected");
        // Subscribe
        Wifi.subscribe(ESP_SUBSCRIBE);
      }
      else
      {
        Serial.print("failed, rc=");
        Serial.print(Wifi.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    break;

  case LORA:
    while (!Lora.connected())
    {
      Serial.print("Attempting MQTT - LORA connection...");
      // Attempt to connect
      if (Lora.connect("clientId", ESP_LORA_TOKEN, ""))
      {
        Serial.println("connected");
      }
      else
      {
        Serial.print("failed, rc=");
        Serial.print(Lora.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    break;
  }
}

void lora_init()
{
  Serial.println("LoRa Receiver - GateWay");
  // setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  // replace the LoRa.begin(---E-) argument with your location's frequency
  // 433E6 for Asia
  // 866E6 for Europe
  // 915E6 for North America
  while (!LoRa.begin(865E6))
  {
    Serial.println(".");
    delay(500);
  }
  Serial.println("LoRa Initializing OK!");
  //    LoRaData.reserve(200);
  LoRa.onReceive(onReceive);
  // put the radio into receive mode
  LoRa.receive();
}


void setup()
{
  /**
   * @brief Create ACCESS AND STATION MODE, Where ESP can connect to the router as well as the Access point.
   *
   */
  // Initialize Serial Monitor
  delay(5000);
  Serial.begin(115200);
  lora_init();
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println("MAC ADDR of the current Device -" + WiFi.macAddress());
  Serial.println("Creating Gateway STATION_MODE & ACCESS_POINT_MODE");
  Serial.print("AP Created with IP Gateway ");
  //    Serial.println(WiFi.softAPIP());     /*Printing the AP IP address*/
  WiFi.printDiag(Serial); // Uncomment to verify channel change after

  WiFi.begin(wifi_network_ssid, wifi_network_password); /*Connecting to Defined Access point*/
  WiFi.printDiag(Serial);                               // Uncomment to verify channel change after

  Serial.println("Connecting to WiFi Network");
  Serial.println(WiFi.macAddress());
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.print("Connected to WiFi network with local IP : ");
  Serial.println(WiFi.localIP()); /*Printing IP address of Connected network*/

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else
  {
    Serial.println("LoRa Initializing OK!");
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(ESP_OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  Wifi.setServer(mqtt_server, 1883);
  Wifi.setCallback(RPC_callback);

  Lora.setServer(mqtt_server, 1883);
}

void loop()
{
  if (LORA_FLAG == 1)
    lora_data();

  if (!Wifi.connected())
  {
    reconnect(WIFI);
  }

  if (RPC_flag == 1)
  {

    if (runEvery(1500) && RPC_flag != 0)
    {
      int result = esp_now_send(broadcastAddress, (uint8_t *)&Speed_data, sizeof(Speed_data));
      if (result == 0)
      {
        Serial.println("Sent with success - RPC retry");
      }
      else
      {
        Serial.println("Error sending the data - RPC retry");
      }
    }
  }
  if (!Lora.connected())
  {
    reconnect(LORA);
  }
  Wifi.loop();
  Lora.loop();
}

void onReceive(int packetSize)
{
  // received a packet
  LoRaData = "";
  Serial.print("Received packet '");

  for (int i = 0; i < packetSize; i++)
  {
    LoRaData += (char)LoRa.read();
  }
  LORA_FLAG = 1;
}

void lora_data()
{

  int commaIndex = LoRaData.indexOf(','); // 100,26.0,50.8,4
  if (commaIndex != -1)
  {
    LoRa_data.level_sensor_value = LoRaData.substring(0, commaIndex).toInt();
    LoRaData = LoRaData.substring(commaIndex + 1);

    commaIndex = LoRaData.indexOf(',');
    if (commaIndex != -1)
    {
      LoRa_data.temperature = LoRaData.substring(0, commaIndex).toFloat();
      LoRaData = LoRaData.substring(commaIndex + 1);

      commaIndex = LoRaData.indexOf(',');
      if (commaIndex != -1)
      {
        LoRa_data.humidity = LoRaData.substring(0, commaIndex).toFloat();
        LoRaData = LoRaData.substring(commaIndex + 1);

        int Lora_counter = LoRaData.toInt();
        Serial.print("Counter: ");
        Serial.println(LoRaData);
      }
    }
  }
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
  if(commaIndex != -1)
  {
    String payload_str = "{\"rssi\":\"" + String(LoRa.packetRssi()) + "\",\"temperature\":\"" + String(LoRa_data.temperature) + "\",\"humidity\":\"" + String(LoRa_data.humidity) + "\",\"level\":\"" + String(LoRa_data.level_sensor_value) + "\"}";
    Serial.println(payload_str);
    Lora.publish("v1/devices/me/telemetry", payload_str.c_str());
    LORA_FLAG = 0;
  }
  else
  {
    Serial.println("ERROR in the Data");
  }
  
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    //    Serial.println(previousMillis);
    previousMillis = currentMillis;
    Serial.print("COUNTER ");
    Serial.println(counter);
    if (counter > 0)
    {
      counter++;
      return true;
    }
    counter++;
  }
  return false;
}
