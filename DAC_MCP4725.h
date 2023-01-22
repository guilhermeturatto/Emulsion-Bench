/*
*   UNIVERSIDADE FEDERAL DE SANTA CATARINA
*      Centro Tecnologico de Joinville
*
* Biblioteca para comunicacao I2C com o DAC MCP4725
*
*           Guilherme Turatto
*               23/05/2022
*
*/

#include <stdint.h>

#ifndef DAC_MCP4725_H_
#define DAC_MCP4725_H_

//---------------------------------------------------------------------------------------------------------------------------
//1st Byte: DEVICE ADDRESSING AND READ/WRITE BIT: |1|1|0|0|A2=0|A1=0|A0=0/1|RW|

//Device Addressing
//TIVA ALREADY PUT THE LAST BIT IN I2C:
//uint8_t FirstByte = (ADDR << 1) | 0b(RW))
#define MCP4725_ADDR0       0x60 //|1|1|0|0|0|0|0|
#define MCP4725_ADDR1       0x62 //|1|1|0|0|0|1|0|

//---------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
//2nd Byte: WRITE COMMAND TYPE AND POWER DOWN SELECT: |C2|C1|C0|X|X|PD1|PD0|X|

//Write Command Type |C2|C1|C0|X=0|
#define     DAC_ONLY        0x0     //Write DAC Register only : (C2 = 0, C1 = 1, C0 = 0, X = 0)
#define     DAC_EEPROM      0x6     //Write DAC Register and EEPROM: (C2 = 0, C1 = 1, C0 = 1, X = 0)


//Power Down Selection |X=0|PD1|PD0|X=0|
//In the power-down mode: VOUT is off and most of internal circuits are disabled.
#define    NORMAL_MODE      0x0      //Normal Mode (X = 0, PD1 = 0, PD0 = 0, X = 0)
#define    PULL_DOWN_1K     0x2      //1K   Ohms resistor to ground  (X = 0, PD1 = 0, PD0 = 1, X = 0)
#define    PULL_DOWN_100K   0x4      //100K Ohms resistor to ground  (X = 0, PD1 = 1, PD0 = 0, X = 0)
#define    PULL_DOWN_500K   0x6      //500K Ohms resistor to ground  (X = 0, PD1 = 1, PD0 = 1, X = 0)

//---------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
//3rd and 4th Byte:
//D11 D10 D9 D8 D7 D6 D5 D4 D3 D2 D0 X=0 X=0 X=0 X=0
//12bits DAC REG

#define DAC_VDD     0xFFF0  //OUT: VDD(V)
#define DAC_GND     0x0000  //OUT:   0(V)
//---------------------------------------------------------------------------------------------------------------------------



void InitI2C0(void);
void I2CSend(uint8_t slave_addr, char array[], int n);
void MCP4725Send(uint16_t ref);



#endif /* DAC_MCP4725_H_ */
