/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2013  Paweł Stawicki. All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ChealiCharger2.h"
#include "MainMenu.h"
#include "Program.h"
#include "ProgramData.h"
#include "AnalogInputs.h"
#include "Utils.h"
#include "Buzzer.h"
#include "Version.h"
#include "Settings.h"
#include "StackInfo.h"
#include "Hardware.h"
#include "SerialLog.h"
#include "eeprom.h"
#include "cpu.h"
#include "Serial.h"
#include "Screen.h"
#include "helper.h"
#include "memory.h"


void setup()
{
    hardware::initializePins();
    cpu::init();

    hardware::initialize();
    Time::initialize();
    SMPS::initialize();
    Discharger::initialize();
    AnalogInputs::initialize();
    Serial::initialize();

#ifdef ENABLE_STACK_INFO
    StackInfo::initialize();
#endif

    Settings::load();
    Screen::initialize();

    Screen::runWelcomeScreen();
}

void doMyCharge(uint8_t cells) {
     //load a specific battery from eeprom
     ProgramData::loadProgramData(0);
     
	 //or (*)
     //ProgramData::currentProgramData.battery.type = ProgramData::Lipo;
     //ProgramData::currentProgramData.battery.C = ANALOG_CHARGE(2.200); //Ah
     //ProgramData::currentProgramData.battery.Ic = ANALOG_AMP(2.2);
     //ProgramData::currentProgramData.battery.Id = ANALOG_AMP(2.2);
     //ProgramData::currentProgramData.battery.cells = cells;
     //ProgramData::currentProgramData.battery.time = 120; //in minutes
     //end of or (*)
     
     //Program::run(Program::FastCharge);
	 Program::runWithoutInfo(Program::Charge);
     // you should call Program::runWithoutInfo, see implementation differences
     // but for the first step this should be enough 
}

void doMyStuff()
{
    uint8_t button;
    AnalogInputs::powerOn();  //we need to "powerOn" analogInputs to see any voltages
    do {
        if(AnalogInputs::isConnected(AnalogInputs::Vout) && AnalogInputs::isOutStable()) {
             uint8_t cells = AnalogInputs::getConnectedBalancePortCellsCount();
             doMyCharge(cells);
             break;
        }        

        button = Keyboard::getPressedWithDelay();
    } while (button == BUTTON_NONE);

    AnalogInputs::powerOff();
}


int main()
{
    setup();
#ifdef ENABLE_HELPER
    helperMain();
#else
    eeprom::check();
    doMyStuff();  ///<------
    MainMenu::run();
#endif
}
