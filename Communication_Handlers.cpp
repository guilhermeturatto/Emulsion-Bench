#include "Communication_Handlers.hpp"
#include "Communication_Interfaces.hpp"
#include "user_configs.h"

extern const char ValidCommands[Labview_nCommands];
volatile bool DataframeIsFull = false;
volatile uint8_t ReceivedCommands[Labview_dataframeSize];


/* Table of CRC values for high�order byte */
static unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 };
/* Table of CRC values for low�order byte */
static char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40 };


//CFW300 ------------------------------------------------------------------------------------------
unsigned short CFW300_ModbusHandler::CRC16(unsigned char *puchMsg, unsigned short usDataLen) {
    unsigned char uchCRCHi = 0xFF; // high byte of CRC initialized
    unsigned char uchCRCLo = 0xFF; // low byte of CRC initialized
    unsigned uIndex; //will index into CRC lookup table */
    while (usDataLen--) { // pass through message buffer
        uIndex = uchCRCLo ^ *puchMsg++; // calculate the CRC
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
        uchCRCHi = auchCRCLo[uIndex];
    }
    return (uchCRCHi << 8 | uchCRCLo);
}


CFW300_ModbusHandler::CFW300_ModbusHandler() {
    Interface = new MODBUS_RTU(UART_CFW300, ENABLE_TX_PB4);
}
CFW300_ModbusHandler::~CFW300_ModbusHandler() {
    delete Interface;
}

int CFW300_ModbusHandler::SendDatagram(uint8_t addr, uint8_t func, uint16_t reg, uint16_t val) {
    _Datagram_Send[0] = addr;                    //Device address
    _Datagram_Send[1] = func;                    //Function
    _Datagram_Send[2] = (reg & 0xFF00) >> 8;     //8 most significant bits of register
    _Datagram_Send[3] = (reg & 0x00FF);          //8 least significant bits of register
    _Datagram_Send[4] = (val & 0xFF00) >> 8;     //8 most significant bits of value
    _Datagram_Send[5] = (val & 0x00FF);          //8 least significant bits of value

    uint16_t CRC = CRC16(_Datagram_Send, 6);     //CRC16 based on message content

    _Datagram_Send[6] = (CRC & 0x00FF);          //8 least significant bits of CRC16
    _Datagram_Send[7] = (CRC & 0xFF00) >> 8;     //8 most significant bits of CRC16

    Interface->Write(_Datagram_Send, 8);

    //Delay in microsseconds (Argument in number of timer loops. Each loop takes 3 clock cycles)
    //SysCtlDelay((MODBUS_END_FRAME_US*SysCtlClockGet()) / (3 * 1000000));
    SysCtlDelay(10*53600);

    //check returned datagram
    //if(TBD){
        //return 0;
    //}

    return 1;
}

int CFW300_ModbusHandler::ReceiveDatagram(uint8_t *data, int len) {
    uint16_t CRC; //CRC received

    //Set length of received datagram accordingly to the function (8bytes or 7bytes);
    //len = _Datagram_Send[1] == CFW300Function::WRITE ? 8 : 7;
    //OBS: LEN MUST BE PROVIDE CORRECTLY BY DEVICE
    if (len > Labview_dataframeSize)
        return 0;

    //Clear previous Datagram
    uint8_t i;
    for(i = 0; i < 8; i++)
        _Datagram_Receive[i] = 0;

    //Read bytes from UART
    Interface->Read(_Datagram_Receive, len);

    for(i = 0; i < len; i++)
        data[i] = _Datagram_Receive[i]; //Datagram received

    //Check message
    //ERROR: If received function is different from the sent function
    if(_Datagram_Receive[0] != _Datagram_Send[0])
        return 0;

    //CRC Received (last 2 bytes)
    CRC = (_Datagram_Receive[len-2] << 8) | _Datagram_Receive[len-1];

    //ERROR: CRC mismatch
    if(CRC != CRC16(_Datagram_Receive, len-2))
        return 0;

    //If function and CRC match, return success
    return 1;
}
//------------------------------------------------------------------------------------------------

//MCP4725 ----------------------------------------------------------------------------------------
MCP4725_I2CHandler::MCP4725_I2CHandler() {
    Interface = new I2C(I2C_MCP4725);
}

MCP4725_I2CHandler::~MCP4725_I2CHandler() {
    delete Interface;
}

int MCP4725_I2CHandler::SendDatagram(uint8_t addr, uint8_t WRITE_MODE, uint16_t PWR_DOWN_MODE, uint16_t ref) {
    if (ref > 4095)
        ref = 4095;

    _Datagram_Send[0] = addr;
    _Datagram_Send[1] = (WRITE_MODE << 4) | PWR_DOWN_MODE;
    //Split integer reference into 2 bytes
    _Datagram_Send[2] =  (ref & 0x0FF0)  >> 4;
    _Datagram_Send[3] = ((ref & 0x000F)  << 4) & 0x00F0;  //Last 4 bits are unused - forced to be 0


    Interface->Write(_Datagram_Send, 4);

    //check returned datagram
        //if(TBD){
            //return 0;
        //}

    return 1;
}

int MCP4725_I2CHandler::ReceiveDatagram(uint8_t *data, int len) { return -1; }


//LabView ----------------------------------------------------------------------------------------
LabView_UARTHandler::LabView_UARTHandler() {
    Interface = new UART(UART_LABVIEW);
}
LabView_UARTHandler::~LabView_UARTHandler() {
    delete Interface;
}

int LabView_UARTHandler::SendDatagram(uint8_t TBD1, uint8_t TBD2, uint16_t TBD3, uint16_t TBD4) { return -1; }

int LabView_UARTHandler::ReceiveDatagram(uint8_t *data, int len) {
    int i;
    if(DataframeIsFull) {
        for(i = 0; i < Labview_dataframeSize; i++) {
            _Datagram_Receive[i]    = ReceivedCommands[i];
            data[i]                 = ReceivedCommands[i];
        }
        DataframeIsFull = false;
        return Labview_dataframeSize;
    }
    return 0; //If dataframe is incomplete
}


extern "C" void InterruptHandler(void) {
    if(DataframeIsFull)
            return;

    static uint8_t RxData[Labview_dataframeSize];
    static uint8_t pos = 0;
    bool initDataFrame = false;

    UART intUART(UART_LABVIEW);
    intUART.Read(RxData, pos);   //Read from UART

    int i = 0;
    //Only assembly dataframe when receive the initial dataframe char (ValidCommands)
    for(i = 0; i < Labview_nCommands; i++) {
        if(RxData[0] == ValidCommands[i])
        {
            initDataFrame = true;
            pos++;
        }
    }

    //Make the interrupt waits until receive a valid start char
    if(!initDataFrame) {
        DataframeIsFull = false;
        RxData[0] = 0;
        pos = 0;
    }

    //When reaches the standard size of dataframe
    if(pos == Labview_dataframeSize){
        //Signals there is a complete dataframe available
        DataframeIsFull = true;

        //Pass the RxData to ReceivedCommands and clear the buffer
        for(i = 0; i < Labview_dataframeSize; i++){
            ReceivedCommands[i] = RxData[i];
            RxData[i] = 0;
        }
        pos = 0;
    }
}