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


#include "hall.h"
#include "modbus_RS485.h"
#include "DAC_MCP4725.h"

volatile uint8_t mode = 0;
volatile uint8_t dacmode = 100;
volatile uint16_t velREF = 1280;

volatile uint8_t Receivedcommands[4] = {0, 0, 0, 0};
volatile bool completeDataFrame = false;

static char ValidCommands[4] = {'V', 'L', 'D', 'A'};


void UART0IntHandler(void)
{
    static uint8_t RxData[4] = {0, 0, 0, 0};
    static uint8_t pos = 0;
    bool initDataFrame = false;
    int i = 0;

    uint32_t ui32Status = UARTIntStatus(UART0_BASE, true);   //Get interrupt status
    UARTIntClear(UART0_BASE, ui32Status);           //clear the asserted interrupts

    //Asserts that are chars to read AND the previous dataframe was already consumed
    if(!UARTCharsAvail(UART0_BASE) || completeDataFrame)
        return; //OBS, NOVA VERSAR CHECAR DATAFRAME COMPLETO ANTES DE HCAMAR UART::READ

    RxData[pos] = UARTCharGet(UART0_BASE);

    //Only assembly dataframe when receive the initial dataframe char (ValidCommands)
    for(i = 0; i < 4; i++)
        if(RxData[0] == ValidCommands[i])
        {
            initDataFrame = true;
            pos++;
        }

    //Make the interrupt waits until receive a valid start char
    if(!initDataFrame){
        completeDataFrame = false;
        RxData[0] = 0;
        pos = 0;
    }

    //When reaches the standard size of dataframe
    if(pos == 4){
        //Signals there is a complete dataframe available
        completeDataFrame = true;

        //Pass the RxData to ReceivedCommands and clear the buffer
        for(i = 0; i < 4; i++){
            Receivedcommands[i] = RxData[i];
            RxData[i] = 0;
        }
        pos = 0;
    }
}


int main(void)
{

    //Configura clock para 40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //Configura comunicacao USB com o computador
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); //Habilita periferico UART0 para comunicacao
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //Habilita periferico GPIOA onde estao os pinos do UART0

    GPIOPinConfigure(GPIO_PA0_U0RX); //Configura PA0 como RX da UART0
    GPIOPinConfigure(GPIO_PA1_U0TX); //Configura PA1 como TX da UART0
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); //Configura os pinos GPIO como RX e TX da UART0

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
                       (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART0); //enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts


    //Habilita UART para comunicacao com o modulo XYK485
    ModbusConfigUART();

    //Habilita I2C para comunicacao com o DAC
    InitI2C0();

    char input;
    char funcao;
    char dispositivo;

    //Habilita GPIO e pinos dos LEDS
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //enable pin for LED PF2

/*
    while(1)
    {
        switch(mode)
        {
        case 1:
            UARTCharPut(UART0_BASE, 'L');

            //LIGA MOTOR NA VELOCIDADE NOMINAL
            ModbusSend(ADDR_INVERSOR_1, WRITE, P683, velREF);
            ModbusSend(ADDR_INVERSOR_1, WRITE, P682,  MOTOR_ON);

            mode = 0;
            continue;

        case 2:
            UARTCharPut(UART0_BASE, 'D');

            //DESLIGA MOTOR
            ModbusSend(ADDR_INVERSOR_1, WRITE, P682, MOTOR_OFF);

            mode = 0;
            continue;

        case 3:
            //INCREMENTA VEL
            UARTCharPut(UART0_BASE, '+');

            velREF += 50;
            ModbusSend(ADDR_INVERSOR_1, WRITE, P683, velREF);
            ModbusSend(ADDR_INVERSOR_1, WRITE, P682,  MOTOR_ON);

            mode = 0;
            continue;

        case 4:
            //DECREMENTA VEL
           UARTCharPut(UART0_BASE, '-');

           velREF -= 50;
           ModbusSend(ADDR_INVERSOR_1, WRITE, P683, velREF);

           mode = 0;
           continue;

        //Teste DAC
        case 5:
            UARTCharPut(UART0_BASE, 'V');
            while(1){
                switch(dacmode)
                {
                case 0:
                    UARTCharPut(UART0_BASE, '0');
                    MCP4725Send((uint16_t)(0));
                    dacmode = 100;
                    continue;
                case 1:
                    UARTCharPut(UART0_BASE, '1');
                    MCP4725Send((uint16_t)(4095*1/5));
                    dacmode = 100;
                    continue;
                case 2:
                    UARTCharPut(UART0_BASE, '2');
                    MCP4725Send((uint16_t)(4095*2/5));
                    dacmode = 100;
                    continue;
                case 3:
                    UARTCharPut(UART0_BASE, '3');
                    MCP4725Send((uint16_t)(4095*3/5));
                    dacmode = 100;
                    continue;
                case 4:
                    UARTCharPut(UART0_BASE, '4');
                    MCP4725Send((uint16_t)(4095*4/5));
                    dacmode = 100;
                    continue;
                case 5:
                    UARTCharPut(UART0_BASE, '5');
                    MCP4725Send((uint16_t)(4095*5/5));
                    dacmode = 100;
                    continue;

                case 10:
                    dacmode = 100;
                    break;

                default:
                    input = UARTCharGet(UART0_BASE);

                    if(input == '0')
                        dacmode = 0;
                    else if(input == '1')
                        dacmode = 1;
                    else if(input == '2')
                        dacmode = 2;
                    else if(input == '3')
                        dacmode = 3;
                    else if(input == '4')
                        dacmode = 4;
                    else if(input == '5')
                        dacmode = 5;
                    else
                        dacmode = 10;

                    continue;
                }
                break;
            }

            mode = 0;
            continue;

        default:
            input = UARTCharGet(UART0_BASE);

            if(input == 'L')
                mode = 1;
            else if(input == 'D')
                mode = 2;
            else if(input == '+')
                mode = 3;
            else if(input == '-')
                mode = 4;
            else if(input == 'V')
                mode = 5;
        }
    }
*/
    char i1;
    char i2;
    uint16_t velocidade = 0;
    uint16_t abertura = 4095;

    while(1)
    {
        switch(mode)
        {
        case 1: //LIGAR MOTOBOMBA (funcao = L)
            if(dispositivo == '1')
                ModbusSend(ADDR_INVERSOR_1, WRITE, P682,  MOTOR_ON);

            else if(dispositivo == '2')
                ModbusSend(ADDR_INVERSOR_2, WRITE, P682,  MOTOR_ON);


            mode = 0;
            continue;

        case 2: //DESLIGAR MOTOBOMBA (funcao = D)
            if(dispositivo == '1')
                ModbusSend(ADDR_INVERSOR_1, WRITE, P682,  MOTOR_OFF);

            else if(dispositivo == '2')
                ModbusSend(ADDR_INVERSOR_2, WRITE, P682,  MOTOR_OFF);

            mode = 0;
            continue;

        case 3:  //DEFINIR VELOCIDADE MOTOBOMBA (funcao = V)
            //i1 = UARTCharGet(UART0_BASE);
            //i2 = UARTCharGet(UART0_BASE);

            velocidade = (i1 << 8) | i2;


            if(dispositivo == '1')
                ModbusSend(ADDR_INVERSOR_1, WRITE, P683, velocidade);

            else if(dispositivo == '2')
                ModbusSend(ADDR_INVERSOR_2, WRITE, P683, velocidade);


            mode = 0;
            continue;


        case 4: //DEFINIR ABERTURA VALVULA (funcao = A)
            abertura = (i1 << 8) | i2;

            MCP4725Send(abertura);

            mode = 0;
            continue;

        default:
            /*
            funcao      = UARTCharGet(UART0_BASE);
            dispositivo = UARTCharGet(UART0_BASE);
            i1 = UARTCharGet(UART0_BASE);
            i2 = UARTCharGet(UART0_BASE);
            */

            while(!completeDataFrame);
            completeDataFrame = false;

            //if(completeDataFrame){
                funcao      = Receivedcommands[0];
                dispositivo = Receivedcommands[1];
                i1          = Receivedcommands[2];
                i2          = Receivedcommands[3];

                if(funcao == 'L')
                    mode = 1;
                else if(funcao == 'D')
                    mode = 2;
                else if(funcao == 'V')
                    mode = 3;
                else if(funcao == 'A')
                    mode = 4;
            //}
                else
                    mode = 0;
        }
    }
}
