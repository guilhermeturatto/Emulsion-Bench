#ifndef BOARD_CONFIGS_H_
#define BOARD_CONFIGS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"


//TYPES USED TO CONFIGURE PERIPHERALS

//UART PERIPHERAL CONFIG
typedef struct UART_PERIPHERAL_CFG {
    uint32_t SYSCTL_PERIPH_UART;
    uint32_t SYSCTL_PERIPH_GPIO;
    uint32_t GPIO_PORT_BASE;
    uint32_t UART_BASE;
    uint32_t INT_UART;
    uint32_t GPIO_P_URX;
    uint32_t GPIO_P_UTX;
    uint32_t GPIO_PIN_x;
    uint32_t GPIO_PIN_y;
} UART_PERIPHERAL_CFG;

//UART USER CONFIG
typedef struct UART_USER_CFG {
    uint32_t BAUD_RATE;
    uint32_t UART_CONFIG_WLEN;
    uint32_t UART_CONFIG_STOP;
    uint32_t UART_CONFIG_PAR;
} UART_USER_CFG;

//UART CONFIG
typedef struct UART_CFG {
    UART_PERIPHERAL_CFG PERIPHERAL_CFG;
    UART_USER_CFG USER_CFG;
} UART_CFG;


//I2C PERIPHERAL CONFIG
typedef struct I2C_PERIPHERAL_CFG {
    uint32_t SYSCTL_PERIPH_I2C;
    uint32_t SYSCTL_PERIPH_GPIO;
    uint32_t GPIO_PORT_BASE;
    uint32_t I2C_BASE;
    uint32_t GPIO_P_I2CSCL;
    uint32_t GPIO_P_I2CSDA;
    uint32_t GPIO_PIN_SCL;
    uint32_t GPIO_PIN_SDA;
} I2C_PERIPHERAL_CFG;

//I2C USER CONFIG
typedef struct I2C_USER_CFG {
    bool BAUD_RATE_HIGH_SPEED; //True: 400kbps, False: 100kbps
} I2C_USER_CFG;

//I2C CONFIG
typedef struct I2C_CFG {
    I2C_PERIPHERAL_CFG PERIPHERAL_CFG;
    I2C_USER_CFG USER_CFG;
} I2C_CFG;


//PIN STRUCT TO ENABLE TX IN RS485 MODULE
typedef struct PIN {
    uint32_t GPIO_PORT_BASE;
    uint32_t GPIO_PIN;
} PIN;

// STANDARD PERIPHERAL CONFIGS TO TIVA-C (dont modify)
//Copy this values into CFG structs with USER_CFG,
//the structures below are not referenced.

const UART_PERIPHERAL_CFG UART0_CFG = {
    .SYSCTL_PERIPH_UART     = SYSCTL_PERIPH_UART0,
    .SYSCTL_PERIPH_GPIO     = SYSCTL_PERIPH_GPIOA,
    .GPIO_PORT_BASE         = GPIO_PORTA_BASE,
    .UART_BASE              = UART0_BASE,
    .INT_UART               = INT_UART0,
    .GPIO_P_URX             = GPIO_PA0_U0RX,
    .GPIO_P_UTX             = GPIO_PA1_U0TX,
    .GPIO_PIN_x             = GPIO_PIN_0,
    .GPIO_PIN_y             = GPIO_PIN_1,
};

const UART_PERIPHERAL_CFG UART1_CFG = {
    .SYSCTL_PERIPH_UART     = SYSCTL_PERIPH_UART1,
    .SYSCTL_PERIPH_GPIO     = SYSCTL_PERIPH_GPIOB,
    .GPIO_PORT_BASE         = GPIO_PORTB_BASE,
    .UART_BASE              = UART1_BASE,
    .INT_UART               = INT_UART1,
    .GPIO_P_URX             = GPIO_PB0_U1RX,
    .GPIO_P_UTX             = GPIO_PB1_U1TX,
    .GPIO_PIN_x             = GPIO_PIN_0,
    .GPIO_PIN_y             = GPIO_PIN_1,
};

const I2C_PERIPHERAL_CFG I2C0_CFG = {
    .SYSCTL_PERIPH_I2C      = SYSCTL_PERIPH_I2C0,
    .SYSCTL_PERIPH_GPIO     = SYSCTL_PERIPH_GPIOB,
    .GPIO_PORT_BASE         = GPIO_PORTB_BASE,
    .I2C_BASE               = I2C0_BASE,
    .GPIO_P_I2CSCL          = GPIO_PB2_I2C0SCL,
    .GPIO_P_I2CSDA          = GPIO_PB3_I2C0SDA,
    .GPIO_PIN_SCL           = GPIO_PIN_2,
    .GPIO_PIN_SDA           = GPIO_PIN_3,
};

#endif /* BOARD_CONFIGS_H_ */
