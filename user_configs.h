#ifndef USER_CONFIGS_H_
#define USER_CONFIGS_H_

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

#include "board_configs.h"

//Copy values from xxx_PERIPHERAL_CFG in board_configs.h to initialize xxx_CFG struct

//CFW300 UART -----------------------------------------------------------------
const UART_CFG UART_CFW300 = {
    .PERIPHERAL_CFG = {
        .SYSCTL_PERIPH_UART     = SYSCTL_PERIPH_UART1,
        .SYSCTL_PERIPH_GPIO     = SYSCTL_PERIPH_GPIOB,
        .GPIO_PORT_BASE         = GPIO_PORTB_BASE,
        .UART_BASE              = UART1_BASE,
        .INT_UART               = INT_UART1,
        .GPIO_P_URX             = GPIO_PB0_U1RX,
        .GPIO_P_UTX             = GPIO_PB1_U1TX,
        .GPIO_PIN_x             = GPIO_PIN_0,
        .GPIO_PIN_y             = GPIO_PIN_1,
    },
    .USER_CFG = { //Parameters to config UART_CFW300 communication
        .BAUD_RATE              = 76800,
        .UART_CONFIG_WLEN       = UART_CONFIG_WLEN_8,
        .UART_CONFIG_STOP       = UART_CONFIG_STOP_ONE,
        .UART_CONFIG_PAR        = UART_CONFIG_PAR_EVEN,
    },
};

const PIN ENABLE_TX_PB4 = {
    .GPIO_PORT_BASE             = GPIO_PORTB_BASE,
    .GPIO_PIN                   = GPIO_PIN_4,
};

#define ADDR_CFW300_1  0x01
#define ADDR_CFW300_2  0x02

//Delay at end of telegram (Datasheet Modbus RTU CFW300 pg17: 76800kbps 2,005ms)
#define MODBUS_END_FRAME_US   2100   //[microseconds]
//-----------------------------------------------------------------------------


//MCP4725 I2C -----------------------------------------------------------------
const I2C_CFG I2C_MCP4725 = {
    .PERIPHERAL_CFG = {
        .SYSCTL_PERIPH_I2C      = SYSCTL_PERIPH_I2C0,
        .SYSCTL_PERIPH_GPIO     = SYSCTL_PERIPH_GPIOB,
        .GPIO_PORT_BASE         = GPIO_PORTB_BASE,
        .I2C_BASE               = I2C0_BASE,
        .GPIO_P_I2CSCL          = GPIO_PB2_I2C0SCL,
        .GPIO_P_I2CSDA          = GPIO_PB3_I2C0SDA,
        .GPIO_PIN_SCL           = GPIO_PIN_2,
        .GPIO_PIN_SDA           = GPIO_PIN_3,
    },
    .USER_CFG = {
        .BAUD_RATE_HIGH_SPEED   = true,
    },
};

//TIVA ALREADY PUT THE LAST BIT IN I2C:
//uint8_t FirstByte = (ADDR << 1) | 0b(RW))
#define MCP4725_ADDR0   0x60 //|1|1|0|0|0|0|0|
#define MCP4725_ADDR1   0x62 //|1|1|0|0|0|1|0|
//-----------------------------------------------------------------------------


//LABVIEW UART ----------------------------------------------------------------
const UART_CFG UART_LABVIEW = {
    .PERIPHERAL_CFG = {
        .SYSCTL_PERIPH_UART     = SYSCTL_PERIPH_UART0,
        .SYSCTL_PERIPH_GPIO     = SYSCTL_PERIPH_GPIOA,
        .GPIO_PORT_BASE         = GPIO_PORTA_BASE,
        .UART_BASE              = UART0_BASE,
        .INT_UART               = INT_UART0,
        .GPIO_P_URX             = GPIO_PA0_U0RX,
        .GPIO_P_UTX             = GPIO_PA1_U0TX,
        .GPIO_PIN_x             = GPIO_PIN_0,
        .GPIO_PIN_y             = GPIO_PIN_1,
    },
    .USER_CFG = {
        .BAUD_RATE               = 9600,
        .UART_CONFIG_WLEN        = UART_CONFIG_WLEN_8,
        .UART_CONFIG_STOP        = UART_CONFIG_STOP_ONE,
        .UART_CONFIG_PAR         = UART_CONFIG_PAR_NONE,
    },
};

#define Labview_nCommands        4
#define Labview_dataframeSize    4

static const char ValidCommands[] = {'A', 'D', 'L', 'V'};
//-----------------------------------------------------------------------------

#endif /* USER_CONFIGS_H_ */
