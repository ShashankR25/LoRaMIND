/**
 * @file LoRa_endNode.ino
 * @author Shashank R (@shashank-r25)
 * @brief 
 * @version 1
 * @date 2023-07-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include "config.h"

const int trigPin = 4;
const int echoPin = 5;

long duration;
float distanceCm;
float distanceInch;

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
int init_value;
int counter = 0;

LoRa_struct_message Lora_data;

void ultrasonic_sensor()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  Serial.println("DISTANCE "+String(duration * SOUND_VELOCITY/2));
  distanceCm = map(duration * SOUND_VELOCITY/2, init_value, 4, 0, 100);
  
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("LoRa Sender");
//  while (!Serial);
  Serial.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  dht.begin();
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America 
  //865E6 -867E6 For India
  /**
   * @brief 
   * 
   */
  while (!LoRa.begin(865E6)) {
    Serial.print(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
//  LoRa.setSyncWord(0xF3);
LoRa.setTxPower(30);
  Serial.println("LoRa Initializing OK!");

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  Serial.println("INIT DISTANCE "+String(duration * SOUND_VELOCITY/2));
  init_value = duration * SOUND_VELOCITY/2;
}

void loop() {
  ultrasonic_sensor();
  Lora_data.level_sensor_value = distanceCm;
    Lora_data.temperature = dht.readTemperature();
    Lora_data.humidity = dht.readHumidity();
//    Lora_data.motor1_y_vibration = random(10,99);
  Serial.print("Sending packet: ");
  Serial.println(Lora_data.level_sensor_value);
  String myString = String(Lora_data.level_sensor_value) + "," + String(Lora_data.temperature) + "," + String(Lora_data.humidity) +"," +String(counter);
  Serial.println(myString);
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(myString);
  LoRa.endPacket();

  counter++;
  Serial.println(Lora_data.level_sensor_value);
  Serial.println(Lora_data.temperature);
  Serial.println(Lora_data.humidity);

  delay(7000);
}
