/**
 * @file config.h
 * @author Shashank R 
 * @brief
 * @version 3
 * @date 2023-04-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#define ESP_DBG     1
#define ESP_NOW
#define accelerometer
#define temperature

// pins
#define MOTOR_CONTROL_PIN 12
#define MOTOR_GND_PIN 14
#define MOTOR_VCC_PIN 2

const char *wifi_network_ssid = "DLink-007-2.4G";    /*Replace with Your own network SSID*/
const char *wifi_network_password = "h3k5brrck84vu"; /*Replace with Your own network PASSWORD*/

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
} struct_message;

void x_Max_acc(int16_t x_acc);
void x_Min_acc(int16_t x_acc);
void y_Max_acc(int16_t y_acc);
void y_Min_acc(int16_t y_acc);
