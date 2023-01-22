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

#include "Devices.hpp"


//CFW300
CFW300_FrequencyInverter::CFW300_FrequencyInverter(uint8_t ADDR) {
    ModbusHandler = new CFW300_ModbusHandler();
    _ModBusADDR = ADDR;
    _velRef13b = 0;

    setVelRef13b(0);
    setMotorON(false);
}

CFW300_FrequencyInverter::~CFW300_FrequencyInverter() {
    delete ModbusHandler;
}

bool CFW300_FrequencyInverter::getMotorON() {
    return _motorON;
}

int CFW300_FrequencyInverter::setMotorON(bool is_ON) {
    //Prevent useless messages
   if (is_ON == _motorON)
       return 1;

    _motorON = is_ON;

    int res = 0;

    //Set state of motor in P682
    if(_motorON)
        res = ModbusHandler->SendDatagram(_ModBusADDR, _Function::WRITE, _Register::P682, _State::ON);
    else
        res = ModbusHandler->SendDatagram(_ModBusADDR, _Function::WRITE, _Register::P682, _State::OFF);

    return res;
}

uint16_t CFW300_FrequencyInverter::getVelRef13b() {
    return _velRef13b;
}

int CFW300_FrequencyInverter::setVelRef13b(int velRef) {
    //Prevent useless messages
    if (velRef == _velRef13b)
        return 1;

    _velRef13b = velRef;

    return ModbusHandler->SendDatagram(_ModBusADDR, _Function::WRITE, _Register::P683, velRef);
}
//----------------------------------------------------------------------------------------------------------


//MCP4725
MCP4725_DAC::MCP4725_DAC(uint8_t ADDR){
    _I2CADDR = ADDR;
    I2CHandler = new MCP4725_I2CHandler();
    _voltageRef12b = 4095;

    //Write in DAC_EEPROM mode to to keep the valve open in case of failure
    //EEPROM value is load at startup of MCP4725
    I2CHandler->SendDatagram(_I2CADDR, _Function::DAC_EEPROM, _PowerMode::NORMAL_MODE, _voltageRef12b);
}

MCP4725_DAC::~MCP4725_DAC(){
    delete I2CHandler;
}

uint16_t MCP4725_DAC::getVoltageLevel() {
    return _voltageRef12b;
}

int MCP4725_DAC::setVoltageLevel(uint8_t voltageRef12b) {
    //Prevent useless messages
    if (voltageRef12b == _voltageRef12b)
        return 1;

    _voltageRef12b = voltageRef12b;

    //Write in DAC_ONLY mode and return state
    return I2CHandler->SendDatagram(_I2CADDR, _Function::DAC_ONLY, _PowerMode::NORMAL_MODE, _voltageRef12b);
}
//----------------------------------------------------------------------------------------------------------


//LabView
LabView_ControlComputer::LabView_ControlComputer(MCP4725_DAC *MCP4725,
                                                 CFW300_FrequencyInverter *CFW300_1,
                                                 CFW300_FrequencyInverter *CFW300_2) {
    UARTHandler = new LabView_UARTHandler();

    _MCP4725    = MCP4725;
    _CFW300[0]  = CFW300_1;
    _CFW300[1]  = CFW300_2;

    _velRefCFW300[0]    = 0;
    _velRefCFW300[1]    = 0;
    _motorON[0]         = false;
    _motorON[1]         = false;
    _voltageRefMCP4725  = 4095;

    _state = 0;
}

LabView_ControlComputer::~LabView_ControlComputer() {
    delete UARTHandler;
}

int LabView_ControlComputer::ReceiveLabviewCommands() {
    char command, device, aux1, aux2;

    //Wait until get a complete datagram
    while(UARTHandler->ReceiveDatagram(_Datagram, Labview_dataframeSize) == 0);

    command     = _Datagram[0];
    device      = _Datagram[1];
    aux1        = _Datagram[2];
    aux2        = _Datagram[3];

    //Only changes the control variables according to the received command
    switch (command) {
        //Command D: Turn Off CFW300_x inverter
        case 'D':
            if (device == '1')
                _motorON[0] = false; //Turn Off Motor 1
           else if (device == '2')
                _motorON[1] = false; //Turn Off Motor 2

            break;

        //Command D: Turn Off CFW300_x inverter
        case 'L':
            if (device == '1')
                _motorON[0] = true; //Turn On Motor 1
           else if (device == '2')
                _motorON[1] = true; //Turn On Motor 2

            break;

        //Command V: Set velref of CFW300_x inverter
        case 'V':
            if (device == '1')
                _velRefCFW300[0] = (aux1 << 8) | aux2; //Update Velref Motor 1
            else if (device == '2')
                _velRefCFW300[1] = (aux1 << 8) | aux2; //Update Velref Motor 2

            break;

        //Command A: Set VoltageRef of MCP4725
        case 'A':
            _voltageRefMCP4725 = (aux1 << 8) | aux2; //Update VoltageRef MCP4725 2
            break;

        //If the command is not recognized, return fail
        default:
            return 0;
    }

    return 1;
}

void LabView_ControlComputer::Run() {
    while(1) {
        //Wait until get valid dataframe with valid command
        while(ReceiveLabviewCommands() == 0);

        int i = 0;

        //Send all command variables,
        //treatments to avoid repeated messages are performed internally in devices
        _MCP4725->setVoltageLevel(_voltageRefMCP4725);
        for(i = 0; i < 2; i++) {
            _CFW300[i]->setVelRef13b(_velRefCFW300[i]);
            _CFW300[i]->setMotorON(_motorON[i]);
        }
    }
}

int LabView_ControlComputer::SendResponseMessage() { return -1; }


//----------------------------------------------------------------------------------------------------------

/*States:
    00 - Read Datagram
    11 - Turn On/Off CFW300 1
    12 - Turn On/Off CFW300 2
    21 - Set Ref CFW300 1
    22 - Set Ref CFW300 2
    31 - Set Ref of MCP4725
*/
