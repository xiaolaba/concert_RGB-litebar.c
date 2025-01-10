// testing Program: Decode OOK Signal and Light RGB LEDs, by xiaolaba
// remote control and central lighting sync for concert or gathering
// 2022/Oct
// ASK/OOK and control message is up to individual definition and config
// OOK output serial data, 9600 BAUD, N81
// ASK/OOK RF front end, CMT2220LS / WS480L or pin-to-pin compatible, 433MHz unlicneced ISM band
// ASK/OOK xtal 13.52127MHz 


#include <stdint.h>
#include <stdio.h>
#include "ch32v003_gpio.h"
#include "ch32v003_uart.h"
#include "ch32v003_pwm.h"

#define UART_RX_PIN 3    // Serial RX pin for OOK signal
#define RED_LED_PIN 7    // RED LED PWM pin
#define GREEN_LED_PIN 6  // GREEN LED PWM pin
#define BLUE_LED_PIN 5   // BLUE LED PWM pin

// Function to initialize GPIO
void gpio_init(void) {
    // Configure LED pins as PWM outputs
    GPIO_ConfigPin(RED_LED_PIN, GPIO_MODE_OUTPUT);
    GPIO_ConfigPin(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    GPIO_ConfigPin(BLUE_LED_PIN, GPIO_MODE_OUTPUT);
}

// Function to initialize UART
void uart_init(void) {
    UART_Config(UART_RX_PIN, 9600, UART_PARITY_NONE, UART_STOPBITS_1, UART_DATABITS_8);
}

// Function to initialize PWM
void pwm_init(void) {
    PWM_Config(RED_LED_PIN);
    PWM_Config(GREEN_LED_PIN);
    PWM_Config(BLUE_LED_PIN);
}

// Function to parse incoming RGB string
void parse_rgb_string(char *input, uint8_t *r, uint8_t *g, uint8_t *b) {
    sscanf(input, "R%2hhxG%2hhxB%2hhx", r, g, b);
}

// Function to set PWM duty cycle
void set_pwm(uint8_t pin, uint8_t duty_cycle) {
    PWM_SetDutyCycle(pin, duty_cycle);
}

// Function to blink the GREEN LED
void blink_green_led(uint8_t times) {
    for (uint8_t i = 0; i < times; i++) {
        GPIO_WritePin(GREEN_LED_PIN, 1);  // Turn ON GREEN LED
        DelayMs(200);                     // Wait for 200ms
        GPIO_WritePin(GREEN_LED_PIN, 0);  // Turn OFF GREEN LED
        DelayMs(200);                     // Wait for 200ms
    }
}

// Main function
int main(void) {
    char buffer[16] = {0};
    uint8_t r = 0, g = 0, b = 0;
    uint8_t index = 0;
    char incoming_byte;

    gpio_init();   // Initialize GPIO
    uart_init();   // Initialize UART
    pwm_init();    // Initialize PWM

    // Blink GREEN LED 3 times to indicate initialization complete
    blink_green_led(3);

    // init color of light bar, likely purple
    set_pwm(RED_LED_PIN, 0xff);
    set_pwm(BLUE_LED_PIN, 0xff);

    while (1) {
        // Read incoming byte from UART
        if (UART_ReadByte(&incoming_byte)) {
            if (incoming_byte == '\n') {  // End of string
                buffer[index] = '\0';    // Null-terminate the string

                // Parse RGB values
                parse_rgb_string(buffer, &r, &g, &b);

                // Set PWM duty cycles
                set_pwm(RED_LED_PIN, r);
                set_pwm(GREEN_LED_PIN, g);
                set_pwm(BLUE_LED_PIN, b);

                // Reset buffer index for the next string
                index = 0;
            } else if (index < sizeof(buffer) - 1) {
                // Add byte to buffer
                buffer[index++] = incoming_byte;
            }
        }
    }

    return 0;
}

