#include "Communication_Interfaces.hpp"


void EnableUART(UART_CFG UARTx, bool enable_interrupt = false) {
    //Enable peripheral UARTx
    SysCtlPeripheralEnable(UARTx.PERIPHERAL_CFG.SYSCTL_PERIPH_UART);
    //Enable peripheral GPIOx with the pins of UARTx
    SysCtlPeripheralEnable(UARTx.PERIPHERAL_CFG.SYSCTL_PERIPH_GPIO);
    //Configure Pin as RX of UARTx
    GPIOPinConfigure(UARTx.PERIPHERAL_CFG.GPIO_P_URX);
    //Configure Pin as TX of UARTx
    GPIOPinConfigure(UARTx.PERIPHERAL_CFG.GPIO_P_UTX);
    //Configure Pins as RX and TX of UARTx
    GPIOPinTypeUART(UARTx.PERIPHERAL_CFG.GPIO_PORT_BASE,
                    UARTx.PERIPHERAL_CFG.GPIO_PIN_x |
                    UARTx.PERIPHERAL_CFG.GPIO_PIN_y);

    //User configs
    UARTConfigSetExpClk(UARTx.PERIPHERAL_CFG.UART_BASE,
                        SysCtlClockGet(),
                        UARTx.USER_CFG.BAUD_RATE,
                        (UARTx.USER_CFG.UART_CONFIG_WLEN    |
                        UARTx.USER_CFG.UART_CONFIG_STOP     |
                        UARTx.USER_CFG.UART_CONFIG_PAR      ));

    if(enable_interrupt) {
        IntMasterEnable(); //Enable processor interrupts
        IntEnable(UARTx.PERIPHERAL_CFG.INT_UART); //Enable the UART interrupt
        UARTIntEnable(UARTx.PERIPHERAL_CFG.UART_BASE, UART_INT_RX | UART_INT_RT);  //Enable RX and TX interrupts
    }
}

void EnableI2C(I2C_CFG I2Cx) {
    //Enable I2C module
    SysCtlPeripheralEnable(I2Cx.PERIPHERAL_CFG.SYSCTL_PERIPH_I2C);
    //Reset module
    SysCtlPeripheralReset(I2Cx.PERIPHERAL_CFG.SYSCTL_PERIPH_I2C);
    //Enable GPIO peripheral that contains I2Cx
    SysCtlPeripheralEnable(I2Cx.PERIPHERAL_CFG.SYSCTL_PERIPH_GPIO);
    //Configure the pin muxing for I2Cx functions on ports.
    GPIOPinConfigure(I2Cx.PERIPHERAL_CFG.GPIO_P_I2CSCL);
    GPIOPinConfigure(I2Cx.PERIPHERAL_CFG.GPIO_P_I2CSDA);
    //Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(I2Cx.PERIPHERAL_CFG.GPIO_PORT_BASE, I2Cx.PERIPHERAL_CFG.GPIO_PIN_SCL);
    GPIOPinTypeI2C(I2Cx.PERIPHERAL_CFG.GPIO_PORT_BASE, I2Cx.PERIPHERAL_CFG.GPIO_PIN_SDA);

    //Enable and initialize the I2Cx master module.
    I2CMasterInitExpClk(I2Cx.PERIPHERAL_CFG.I2C_BASE, SysCtlClockGet(), I2Cx.USER_CFG.BAUD_RATE_HIGH_SPEED);
    //clear I2Cx FIFOs
    HWREG(I2Cx.PERIPHERAL_CFG.I2C_BASE + I2C_O_FIFOCTL) = 80008000;
}


//MODBUS_RTU ----------------------------------------------------------------------------
MODBUS_RTU::MODBUS_RTU(UART_CFG myUART, PIN myPIN) {
        _UART           = myUART;
        _ENABLE_TX_PIN  = myPIN;

        EnableUART(_UART);

        //Enable GPIO pin to output
        GPIOPinTypeGPIOOutput(_ENABLE_TX_PIN.GPIO_PORT_BASE, _ENABLE_TX_PIN.GPIO_PIN);
}

void MODBUS_RTU::Write(uint8_t *data, int len) {
    //HIGH to enable transmission
    GPIOPinWrite(_ENABLE_TX_PIN.GPIO_PORT_BASE, _ENABLE_TX_PIN.GPIO_PIN, (uint8_t)16);

    int i = 0;
    for(i=0; i < len; i++){
        UARTCharPut(_UART.PERIPHERAL_CFG.UART_BASE, data[i]);
    }
}

void MODBUS_RTU::Read(uint8_t *data, int len) {
    int i = 0;
    for(i=0; i < len; i++){
        data[i] = UARTCharGet(_UART.PERIPHERAL_CFG.UART_BASE);
    }
}
//---------------------------------------------------------------------------------------

//I2C -----------------------------------------------------------------------------------
I2C::I2C(I2C_CFG myI2C){
    _I2C = myI2C;

    EnableI2C(_I2C);
}

void I2C::Write(uint8_t *data, int len) {
    //Tell the master module what address it will place on the bus when communicating.
    I2CMasterSlaveAddrSet(_I2C.PERIPHERAL_CFG.I2C_BASE, data[0], false);

    //Put data to be sent into FIFO
    I2CMasterDataPut(_I2C.PERIPHERAL_CFG.I2C_BASE, data[1]);

    //If there is only one argument, use the single send I2C function
    if(len == 2) {
        //Initiate send of data from the MCU
        I2CMasterControl(_I2C.PERIPHERAL_CFG.I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        //Wait until MCU is done transferring
        while(I2CMasterBusy(_I2C.PERIPHERAL_CFG.I2C_BASE));
    }

    //Otherwise, start transmission of multiple bytes on the I2C bus
    else {
        //Initiate send of data from the MCU
        I2CMasterControl(_I2C.PERIPHERAL_CFG.I2C_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        // Wait until MCU is done transferring
        while(I2CMasterBusy(_I2C.PERIPHERAL_CFG.I2C_BASE));

        int i = 2;

        //Send num_of_args-2 pieces of data, using the BURST_SEND_CONT command
        while(i != len-1) {
            //Put next byte of data into I2C FIFO
            I2CMasterDataPut(_I2C.PERIPHERAL_CFG.I2C_BASE, data[i++]);
            //Send next data that was just placed into FIFO
            I2CMasterControl(_I2C.PERIPHERAL_CFG.I2C_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            //Wait until MCU is done transferring
            while(I2CMasterBusy(_I2C.PERIPHERAL_CFG.I2C_BASE));
        }

        //Put last byte of data into I2C FIFO
        I2CMasterDataPut(_I2C.PERIPHERAL_CFG.I2C_BASE, data[i]);
        //send next data that was just placed into FIFO
        I2CMasterControl(_I2C.PERIPHERAL_CFG.I2C_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        //Wait until MCU is done transferring
        while(I2CMasterBusy(_I2C.PERIPHERAL_CFG.I2C_BASE));
    }
}

void I2C::Read(uint8_t *data, int len) {}
//---------------------------------------------------------------------------------------


//UART ----------------------------------------------------------------------------------
UART::UART(UART_CFG myUART) {
    _UART = myUART;
    EnableUART(_UART, true);
}

void UART::Write(uint8_t *data, int len) {}

void UART::Read(uint8_t *data, int pos) {
    //Get interrupt status
    uint32_t ui32Status = UARTIntStatus(_UART.PERIPHERAL_CFG.UART_BASE, true);
    //Clear the asserted interrupts
    UARTIntClear(_UART.PERIPHERAL_CFG.UART_BASE, ui32Status);

    //Asserts that are chars to read
    if(!UARTCharsAvail(_UART.PERIPHERAL_CFG.UART_BASE)) {
        data[pos] = 0;
        return;
    }

    data[pos] = UARTCharGet(_UART.PERIPHERAL_CFG.UART_BASE);
}
//---------------------------------------------------------------------------------------


