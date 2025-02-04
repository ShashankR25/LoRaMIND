/**
 * @file config_v1.h
 * @author Shashank R
 * @brief
 * @version 0.1
 * @date 2023-04-14
 *
 * @copyright Copyright (c) 2023
 *
 */

const char *wifi_network_ssid = "WIFI_SSID";    /*Replace with Your own network SSID*/
const char *wifi_network_password = "WIFI_PASS"; /*Replace with Your own network PASSWORD*/
const char *mqtt_server = "IP_ADDR";            // Azure server DOMAIN NAME:- dev.thingspad.io

#define LORA 0
#define WIFI 1

#define ESP_NOW 1
#define LoRA 1

typedef struct struct_message1
{
    int level_sensor_value;
    float temperature;
    float humidity;
    int rssi_value;
} LoRa_struct_message;

typedef struct struct_message3
{
    String value;
    int motor_rotation_speed;
} Speed_struct_message;

typedef struct struct_message2
{
    int motor_temperature;
    int16_t motor_x_max;
    int16_t motor_y_max;
    int16_t motor_x_min;
    int16_t motor_y_min;
    int motor_rotation_speed;
} WiFi_struct_message;

boolean runEvery(unsigned long interval);
