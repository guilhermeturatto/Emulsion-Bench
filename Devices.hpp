#ifndef DEVICES_HPP_
#define DEVICES_HPP_

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
#include "user_configs.h"
#include "Communication_Handlers.hpp"

/*C++ Section*/
#ifdef __cplusplus

class CFW300_FrequencyInverter {
public:
    CFW300_FrequencyInverter(uint8_t ADDR);
    ~CFW300_FrequencyInverter();
    bool getMotorON();
    int setMotorON(bool is_ON);
    uint16_t getVelRef13b();
    int setVelRef13b(int velRef);

private:
    Communication_Handler* ModbusHandler;
    uint8_t _ModBusADDR;
    uint16_t _velRef13b;
    bool _motorON;

    enum _Register {
        P680 = 0x02A8, //RO  - Device status monitoring
        P681 = 0x02A9, //RO  - Frequency reference in 13 bits
        P682 = 0x02AA, //R/W - Control word via Serial
        P683 = 0x02AB, //R/W - Frequency reference via Serial (0-8192)
    };

    enum _Function {
        WRITE = 0x06,
        READ  = 0x03,
    };

    enum _State {
        ON  = 0x0017, //00010111 (P682)
        OFF = 0x0016, //00010110 (P682)
    };
};


class MCP4725_DAC {
public:
    MCP4725_DAC(uint8_t ADDR);
    ~MCP4725_DAC();
    uint16_t getVoltageLevel();
    int setVoltageLevel(uint8_t voltageRef12b);

private:
    Communication_Handler* I2CHandler;
    uint8_t _I2CADDR;
    uint16_t _voltageRef12b;

    //Power Down Selection |X=0|PD1|PD0|X=0|
    //In the power-down mode: VOUT is off and most of internal circuits are disabled.
    enum _PowerMode {
        NORMAL_MODE     = 0x0000, //Normal Mode (X = 0, PD1 = 0, PD0 = 0, X = 0)
        PULL_DOWN_1K    = 0x0002, //1K   Ohms resistor to ground (X = 0, PD1 = 0, PD0 = 1, X = 0)
        PULL_DOWN_100K  = 0x0004, //100K Ohms resistor to ground (X = 0, PD1 = 1, PD0 = 0, X = 0)
        PULL_DOWN_500K  = 0x0006, //500K Ohms resistor to ground (X = 0, PD1 = 1, PD0 = 1, X = 0)
    };

    enum _Function {
        DAC_ONLY        = 0x00, //Write DAC Register only : (C2 = 0, C1 = 1, C0 = 0, X = 0)
        DAC_EEPROM      = 0x06, //Write DAC Register and EEPROM: (C2 = 0, C1 = 1, C0 = 1, X = 0)
    };


};

class LabView_ControlComputer {
public:
    LabView_ControlComputer(MCP4725_DAC *MCP4725,
                            CFW300_FrequencyInverter *CFW300_1,
                            CFW300_FrequencyInverter *CFW300_2);
    ~LabView_ControlComputer();
    void Run();

private:
    int ReceiveLabviewCommands();
    int SendResponseMessage();

    int         _state;
    uint8_t     _Datagram[Labview_dataframeSize];
    uint16_t    _velRefCFW300[2];
    bool        _motorON[2];
    uint16_t    _voltageRefMCP4725;

    Communication_Handler *UARTHandler;

    //Devices
    MCP4725_DAC                 *_MCP4725;
    CFW300_FrequencyInverter    *_CFW300[2];
};


#endif /*C++ Section*/

#endif /* DEVICES_HPP_ */
