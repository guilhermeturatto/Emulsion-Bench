#include "board_configs.h"
#include "user_configs.h"
#include "Communication_Interfaces.hpp"
#include "Communication_Handlers.hpp"
#include "Devices.hpp"


int main(void) {
    MCP4725_DAC                 *MCP4725        = new MCP4725_DAC(MCP4725_ADDR0);
    CFW300_FrequencyInverter    *CFW300_1       = new CFW300_FrequencyInverter(ADDR_CFW300_1);
    CFW300_FrequencyInverter    *CFW300_2       = new CFW300_FrequencyInverter(ADDR_CFW300_2);

    LabView_ControlComputer     *LabViewControl = new LabView_ControlComputer(MCP4725, CFW300_1, CFW300_2);

    LabViewControl->Run();

    delete MCP4725;
    delete CFW300_1;
    delete CFW300_2;
    delete LabViewControl;

    return 0;
}

