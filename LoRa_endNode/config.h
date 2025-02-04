/**
 * @file config.h
 * @author Shashank R
 * @brief Coniguration file
 * @version 0.1
 * @date 2023-07-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//define the pins used by the transceiver module
#define ss 15
#define rst 16
#define dio0 2

#define DHTPIN 10     // Digital pin connected to the DHT sensor

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

typedef struct struct_message1 {
    int level_sensor_value;
    float temperature;
    float humidity;
    int rssi_value;
} LoRa_struct_message;