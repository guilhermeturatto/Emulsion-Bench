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
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "modbus_RS485.h"
#include "hall.h"

/* Table of CRC values for high–order byte */
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
/* Table of CRC values for low–order byte */
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


/* The function returns the CRC as a unsigned short type */
unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen) {
    unsigned char uchCRCHi = 0xFF; /* high byte of CRC initialized     */
    unsigned char uchCRCLo = 0xFF; /* low byte of CRC initialized      */
    unsigned uIndex; /* will index into CRC lookup table */
    while (usDataLen--) /* pass through message buffer      */
    {
        uIndex = uchCRCLo ^ *puchMsg++; /* calculate the CRC                */
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
        uchCRCHi = auchCRCLo[uIndex];
    }
    return (uchCRCHi << 8 | uchCRCLo);
}




void ModbusConfigUART(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1); //Habilita periferico UART1 para comunicacao
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //Habilita periferico GPIOB onde estao os pinos do UART1
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4); //Habilita PB4 para DE/RE

    GPIOPinConfigure(GPIO_PB0_U1RX); //Configura PB0 como RX da UART1
    GPIOPinConfigure(GPIO_PB1_U1TX); //Configura PB1 como TX da UART1
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); //Configura os pinos GPIO como RX e TX da UART1

    /*Configura comunicacao UART:
        1 bit de stop;
        Sem bits de paridade;
        BAUD RATE - Configurado no arquivo modbus_RS485.h;
    */
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), MODBUS_BAUD_RATE,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_EVEN));


    //Comunicacao atraves de funcao ISR
    //IntMasterEnable(); //Habilita interrupcoes
    //IntEnable(INT_UART1); //Habilita interrupcoes da UART1
    //UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT); //Habilita interrupcoes para RX e TX
}


//Escreve os dados no canal de comunicacao
void ModbusWrite(unsigned char *data, unsigned int s){
    //Nivel logico alto em PB4 - Habilita transmissão (TX)
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, (uint8_t)16);

    uint16_t i=0;
    for(i=0; i < s; i++){
        UARTCharPut(UART1_BASE, data[i]);
    }
}

//Le os dados do canal de comunicacao
void ModbusRead(unsigned char *data, unsigned int s) {
    uint16_t i=0;
    for(i=0; i < s; i++){
        data[i] = UARTCharGet(UART1_BASE);
    }
}


// FUNCAO PARA COMANDOS DE LEITURA OU ESCRITA DE REGISTRADORES DO INVERSOR CFW300
/*  addr_slave  = Endereco do dispositivo: 1-247 (configurado em P308);
 *  func        = WRITE / READ - (Funcoes 06 ou 03);
 *  registrador =
 *      Write:  Endereco do registrador que sera modificado (2bytes)
 *      Read:   Endereco do registrador que sera lido
        (pode ser feita a leitura de varios registradores em sequencia e este endereco indica o primeiro)
 *  valor =
 *      Write:  Valor que sera escrito no registrador
 *      Read:   Numero de registradores que serao lido em sequencia
 */
void ModbusSend(char addr_slave, function func, short registrador, uint16_t valor){
    uint8_t frame[8];                           //Frame Modbus - Dados + CRC (para CFW300)

    //Escrevendo os dados no frame (montando a mensagem)
    frame[0] = addr_slave;                      //Endereco do dispositivo
    frame[1] = func;                            //Funcao (leitura ou escrita)

    frame[2] = (registrador & 0xFF00) >> 8; // Seleciona 8 bits mais significativos de "registrador"
    frame[3] = (registrador & 0x00FF);          // Seleciona 8 bits menos significativos de "registrador"

    frame[4] = (valor & 0xFF00) >> 8;           // Seleciona 8 bits mais significativos de "valor"
    frame[5] = (valor & 0x00FF);                // Seleciona 8 bits menos significativos de "valor"

    //Calculando CRC (utiliza apenas os dados para o calculo - 6bytes)
    uint16_t CRC = CRC16(frame, 6);

    //Escrevendo o CRC no frame
    frame[6] = (CRC & 0x00FF);             // Seleciona 8 bits mais significativos de "CRC"
    frame[7] = (CRC & 0xFF00) >> 8;        // Seleciona 8 bits menos significativos de "CRC"


    //Envia os 8 bytes do frame via UART para o modulo RS485 para serem convertidos para RS485
    ModbusWrite(frame, 8);

    //Delay em microssegundos (Argumento em numero de loops do timer. Cada loop leva 3 ciclos de clock)
    //Indica fim do frame
    SysCtlDelay(10*53600);
}


// FUNCAO PARA RECEBER AS RESPOSTAS DO INVERSOR CFW300
/*
 * Considerando uma resposta de no maximo 8 bytes
 *
 * RESTRICAO:
 * AO UTILIZAR ModbusSend(...) PARA REQUISICAO LEITURA (FUNCAO READ) O PARAMETRO "valor" DEVE SER 1,
 * POIS A FUNCAO ModbusReceive(...) ESPERA NO MAXIMO 8 BYTES DE RESPOSTA
 *
 * Respostas esperadas:
 *      -Utilizando a Funcao ModbusSend(...) como WRITE (1registrador): 8bytes de resposta (replica da requisicao);
 *      -Utilizando a Funcao ModbusSend(...) como READ  (1registrador): 7bytes de resposta: |Endereço do escravo | Função | ByteCount | PXXX(high) | PXXX(low) | CRC- | CRC+ |;
 *
 *Parametros:
 *      *addr_slave:    sera escrito nessa variavel o endereco do escravo informado na resposta recebida (para indicar sucesso na transmissao deve ser o mesmo que o informado no envio);
 *      func:           funcao que foi enviada no ModbusSend(...): READ ou WRITE (deve ser informada na chamada da funcao para determinar o tamanho da resposta esperada);
 *      *registrador:   sera escrito nessa variavel o endereco do registrador informado na resposta recebida (para indicar sucesso na transmissao deve ser o mesmo que o informado no envio)
 *                      OBS: RESPOSTAS REFERENTES A OPERACOES DE LEITURA NAO INFROMAM ESTE CAMPO, SERA ATRIBUIDO O VALOR -1 NESTE CASO;
 *      *valor:         sera escrito nessa variavel o valor do registrador;
 * Retorno:
 *      1 se a leitura ocorreu corretamente (CRC correto);
 *      0 em caso de falha;
 */
int ModbusReceive(char *addr_slave, function func, short *registrador, unsigned short *valor){
    uint8_t frame[8];                     //buffer de resposta
    uint16_t CRC;                         //CRC da respsota
    int s = (func == WRITE) ? 8 : 7;      //tamanho da resposta em bytes (8 ou 7);

    //Leitura do frame via UART
    ModbusRead(frame, s);

    //Lendo os dados do frame (desmontando a mensagem)
    *addr_slave = frame[0];     //Endereco do dispositivo

    //ERRO: Se a funcao nao corresponder com a funcao enviada
    if(frame[1] != func)
        return 0;


    if(s == 8)  //Se a resposta for referente a uma operacao de escrita
    {
        *registrador    = (frame[2] << 8) | frame[3];   //Endereco do registrador    (bytes 3 e 4)
        *valor          = (frame[4] << 8) | frame[5];   //Valor do registrador       (bytes 5 e 6)
        CRC             = (frame[6] << 8) | frame[7];   //CRC da resposta            (bytes 7 e 8)

        //ERRO: CRC incorreto (falha na transmissao)
        if(CRC != CRC16(frame, 6))
            return 0;
    }
    else        //Se a resposta for referente a uma operacao de leitura de 1 registrador
    {
        *registrador    = -1;                           //Resposta de operacoes de leitura nao informam este campo
        *valor          = (frame[3] << 8) | frame[4];   //Valor do registrador       (bytes 4 e 5)
        CRC             = (frame[5] << 8) | frame[6];   //CRC da resposta            (bytes 6 e 7)

        //ERRO: CRC incorreto (falha na transmissao)
        if(CRC != CRC16(frame, 5))
            return 0;
    }

    return 1;
}
