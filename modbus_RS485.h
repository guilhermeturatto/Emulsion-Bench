/*
*   UNIVERSIDADE FEDERAL DE SANTA CATARINA
*      Centro Tecnologico de Joinville
*
* Biblioteca para comunicacao Modbus utilizando
*      modulo RS485 (UART to RS485)
*
*
*           Guilherme Turatto
*               23/05/2022
*
*/

#ifndef MODBUS_RS485_H_
#define MODBUS_RS485_H_

//Configuracao da comunicacao UART com o modulo RS485
#define MODBUS_BAUD_RATE            76800
#define TIME_END_FRAME_US           2100    //[microssegundos] - T3,5X: Indica o fim do telegrama - Datasheet Modbus RTU CFW300: 4,01ms (pg17)

//Enum das possiveis funcoes
typedef enum function {
      WRITE = 0x06,
      READ  = 0x03
}function;

//Tabelas para CRC
static unsigned char auchCRCHi[];
static char auchCRCLo[];

//Funcoes disponiveis
void ModbusConfigUART();
void ModbusSend(char, function, short, uint16_t);

#endif /* MODBUS_RS485_H_ */
