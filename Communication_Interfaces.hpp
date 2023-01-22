#ifndef COMMUNICATION_INTERFACES_HPP_
#define COMMUNICATION_INTERFACES_HPP_

/*C++ Section*/
#ifdef __cplusplus

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


//Abstract base class
class Communication_Interface {
public:
    virtual void Write(uint8_t *, int) = 0;
    virtual void Read(uint8_t *, int) = 0;
};

//Modbus interface
class MODBUS_RTU : public Communication_Interface {
public:
    MODBUS_RTU(UART_CFG myUART, PIN myPIN);

    void Write(uint8_t *data, int len) override;
    void Read(uint8_t *data, int len) override;

private:
    UART_CFG _UART;
    PIN _ENABLE_TX_PIN;
};

//I2C interface
class I2C : public Communication_Interface {
public:
    I2C(I2C_CFG myI2C);

    void Write(uint8_t *data, int len) override;
    void Read(uint8_t *data, int len) override;

private:
    I2C_CFG _I2C;
};

//UART interface
class UART : public Communication_Interface {
public:
    UART(UART_CFG myUART);

    void Write(uint8_t *data, int len) override;
    void Read(uint8_t *data, int pos) override;

private:
    UART_CFG _UART;
};

#endif /*C++ Section*/

#endif /* COMMUNICATION_INTERFACES_HPP_ */
