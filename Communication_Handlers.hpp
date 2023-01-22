#ifndef COMMUNICATION_HANDLERS_HPP_
#define COMMUNICATION_HANDLERS_HPP_

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

#include "Communication_Interfaces.hpp"
#include "user_configs.h"

//Abstract base class
class Communication_Handler {
protected:
    bool ERROR_FLAG;
    Communication_Interface *Interface;

public:
    virtual int SendDatagram(uint8_t, uint8_t, uint16_t, uint16_t) = 0;
    virtual int ReceiveDatagram(uint8_t *, int) = 0;
};

//Modbus Handler
class CFW300_ModbusHandler : public Communication_Handler {
public:
    CFW300_ModbusHandler();
    ~CFW300_ModbusHandler();
    int SendDatagram(uint8_t addr, uint8_t func, uint16_t reg, uint16_t value) override;
    int ReceiveDatagram(uint8_t *data, int len) override;

private:
    uint8_t _Datagram_Send[8];
    uint8_t _Datagram_Receive[8];
    unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen);
};


//I2C Handler
class MCP4725_I2CHandler : public Communication_Handler {
public:
    MCP4725_I2CHandler();
    ~MCP4725_I2CHandler();
    int SendDatagram(uint8_t addr, uint8_t WRITE_MODE, uint16_t PWR_DOWN_MODE, uint16_t ref) override;
    int ReceiveDatagram(uint8_t *data, int len) override;

private:
    uint8_t _Datagram_Send[4];
    uint8_t _Datagram_Receive[4];
};

//UART Handler
class LabView_UARTHandler : public Communication_Handler {
public:
    LabView_UARTHandler();
    ~LabView_UARTHandler();
    int SendDatagram(uint8_t TBD1, uint8_t TBD2, uint16_t TBD3, uint16_t TBD4) override;
    int ReceiveDatagram(uint8_t *data, int len) override;

private:
    uint8_t _Datagram_Send[Labview_dataframeSize];
    uint8_t _Datagram_Receive[Labview_dataframeSize];

};


#endif /*C++ Section*/

#endif /* COMMUNICATION_HANDLERS_HPP_ */


