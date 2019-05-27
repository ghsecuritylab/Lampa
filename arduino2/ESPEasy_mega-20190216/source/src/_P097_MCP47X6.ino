#ifdef USES_P097
//#######################################################################################################
//#################################### Plugin 097: ExtWiredAnalog #######################################
//#######################################################################################################

#ifdef ARDUINO
#if ARDUINO < 100
#include "WProgram.h"
#else
#include "Arduino.h"
#endif
#else
#include "ArduinoWrapper.h"
#endif

#include <Wire.h>

// I2C Address of device
// MCP4706, MCP4716 & MCP4726 are factory programed for any of 0x60 thru 0x67
//  commonly 0x60
#define MCP47X6_DEFAULT_ADDRESS	0x60

/** Config Data Defn
* Config = 0bCCCVVPPG
*/

// Programmable Gain definitions
#define MCP47X6_GAIN_MASK	0xFE
#define MCP47X6_GAIN_1X	0x00
#define MCP47X6_GAIN_2X	0x01

// Power Down Mode definitions
#define MCP47X6_PWRDN_MASK     0xF9
#define MCP47X6_AWAKE          0x00
#define MCP47X6_PWRDN_1K       0x02
#define MCP47X6_PWRDN_100K     0x04
#define MCP47X6_PWRDN_500K     0x06

// Reference Voltage definitions
#define MCP47X6_VREF_MASK             0xE7
#define MCP47X6_VREF_VDD              0x00
#define MCP47X6_VREF_VREFPIN	      0x10
#define MCP47X6_VREF_VREFPIN_BUFFERED 0x18

// Command definitioins
#define MCP47X6_CMD_MASK       0x1F
#define MCP47X6_CMD_VOLDAC     0x00
#define MCP47X6_CMD_VOLALL     0x40
#define MCP47X6_CMD_VOLCONFIG  0x80
#define MCP47X6_CMD_ALL        0x60


class MCP47X6 {
public:
  MCP47X6();
  MCP47X6(uint8_t address);

  bool testConnection(void);

  bool begin(void);
  bool begin(uint8_t config);

  // Set the configuration bits for the DAC
  void setGain(uint8_t gain);
  void setVReference(uint8_t ref);
  bool saveSettings(void);

  // Set the DAC
  bool setOutputLevel(uint8_t level);
  bool setOutputLevel(uint16_t level);

  // Power Down
  // NOTE: writing any settings or DAC value
  // will awaken device
  bool powerDown(uint8_t pdOutR);
  bool powerDown(void);

private:
  bool writeConfigReg(uint8_t theConfig);
  uint8_t devAddr;
  uint8_t config;
};

/******************************************
* Default constructor, uses default I2C address.
* @see MCP47X6_DEFAULT_ADDRESS
*/
MCP47X6::MCP47X6() {
  devAddr = MCP47X6_DEFAULT_ADDRESS;
}

/******************************************
* Specific address constructor.
* @param address I2C address
* @see MCP47X6_DEFAULT_ADDRESS
*/
MCP47X6::MCP47X6(uint8_t address) {
  devAddr = address;
}

/******************************************
* Verify the I2C connection.
* Make sure the device is connected and responds as expected.
* @return true if connection is valid, false otherwise
*/
bool MCP47X6::testConnection(void) {
  Wire.beginTransmission(devAddr);
  return (Wire.endTransmission() == 0);
}

/******************************************
* Power on and prepare for general usage.
* This device recalls the previously saved state at power up. The begin()
* function reads these values from the DAC and uses them to reinitialize
* the driver since a reset of the microcontroller
* did not necessarily restart the device.
*/
bool MCP47X6::begin() {
  // read the settings from DAC EEPROM

  // reinitialize the device from the read settings
  return writeConfigReg(config);
}

bool MCP47X6::begin(uint8_t newConfig) {
  // initialize the device from the new settings
  config = newConfig;
  return true;
}

/******************************************
* Set the configuration bits for the DAC
*/
void MCP47X6::setGain(uint8_t gain) {
  config = (config & MCP47X6_GAIN_MASK) | (gain & !MCP47X6_GAIN_MASK);
}

void MCP47X6::setVReference(uint8_t vref) {
  config = (config & MCP47X6_VREF_MASK) | (vref & !MCP47X6_VREF_MASK);
}

//void MCP47X6::setPwrDnOutRes(uint8_t pdOutR) {
//  config = (config & MCP47X6_PWRDN_MASK) | (pdOutR & !MCP47X6_PWRDN_MASK);
//}

/******************************************
* Saves current DAC settings into DAC EEPROM for use at power up
*/
bool MCP47X6::saveSettings(void) {
  // read the current volatile settings
  // write the values back to DAC EEPROM
}

/******************************************
* Set DAC output level value
* Valid range is 0x000 to 0xFFF for all parts.
* For the MCP4726 all 4096 steps are used.
* For the MCP4716 only 1024 steps are used. The two LSBs are not used.
* (i.e. value & 0xFFC)
* For the MCP4706 only 256 steps are used. The four LSBs are not used.
* (i.e. value & 0xFF0)
*/
bool MCP47X6::setOutputLevel(uint16_t level) {
  Wire.beginTransmission(devAddr);
  Wire.write((config | MCP47X6_CMD_VOLALL) & MCP47X6_PWRDN_MASK);
  Wire.write((uint8_t) ((level>>4) & 0xFF));
  Wire.write((uint8_t) ((level<<4) & 0xF0));
  return (Wire.endTransmission() == 0);
}

// Special case for 8-bit device (MCP4706) - saves one byte of transfer
// and is therefore faster
bool MCP47X6::setOutputLevel(uint8_t level) {
  Wire.beginTransmission(devAddr);
  Wire.write((uint8_t) MCP47X6_CMD_VOLDAC);
  Wire.write(level);
  return (Wire.endTransmission() == 0);
}


/******************************************
* Put the DAC into a low power state
* NOTE: writing any settings or DAC output level value
* returns the DAC to the awake power state.
*/
bool MCP47X6::powerDown() {
  return writeConfigReg(config);
}

bool MCP47X6::powerDown(uint8_t pdOutR) {
  config = (config & MCP47X6_PWRDN_MASK) | (pdOutR & !MCP47X6_PWRDN_MASK);
  return writeConfigReg(config);
}

/******************************************
* Private helper function to write just the config register
*/
bool MCP47X6::writeConfigReg(uint8_t theConfig) {
  Wire.beginTransmission(devAddr);
  Wire.write(theConfig | MCP47X6_CMD_VOLCONFIG);
  return (Wire.endTransmission() == 0);
}

MCP47X6 mDAC = MCP47X6();


#define PLUGIN_097
#define PLUGIN_ID_097         97
#define PLUGIN_NAME_097       "DAC output - MCP47X6"
#define PLUGIN_VALUENAME1_097 "DAC"
uint16_t proc_value_dac = 0;
uint16_t raw_val_dac;

boolean Plugin_097(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  // static byte portValue = 0;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
    {
      Device[++deviceCount].Number = PLUGIN_ID_097;
      Device[deviceCount].Type = DEVICE_TYPE_I2C;
      Device[deviceCount].VType = SENSOR_TYPE_NONE;
      Device[deviceCount].Ports = 0;
      Device[deviceCount].PullUpOption = false;
      Device[deviceCount].InverseLogicOption = false;
      Device[deviceCount].FormulaOption = true;
      Device[deviceCount].ValueCount = 1;
      Device[deviceCount].SendDataOption = true;
      Device[deviceCount].TimerOption = true;
      Device[deviceCount].GlobalSyncOption = true;
      break;
    }
    case PLUGIN_INIT:
    {

      addLog(LOG_LEVEL_INFO, F("P097 : DAC I2C init"));

      Wire.begin(); // start transmission to device

      mDAC.begin();
      mDAC.setVReference(MCP47X6_VREF_VDD);
      mDAC.setGain(MCP47X6_GAIN_1X);

      success = true;
      break;
    }
    case PLUGIN_GET_DEVICENAME:
    {
      string = F(PLUGIN_NAME_097);
      break;
    }

    case PLUGIN_GET_DEVICEVALUENAMES:
    {
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_097));
      break;
    }

    case PLUGIN_WEBFORM_LOAD:
    {

      addFormNumericBox(F("Power (0-100\%)"), F("p097_proc_value_dac"), proc_value_dac, 0, 100);

      success = true;
      break;
    }
    case PLUGIN_WEBFORM_SAVE:
    {
      PCONFIG(0) = getFormItemInt(F("p097_proc_value_dac"));
      proc_value_dac = PCONFIG(0);
      UserVar[event->BaseVarIndex]=proc_value_dac;
      raw_val_dac=proc_value_dac*4095/100;
      raw_val_dac=raw_val_dac%4096;
      mDAC.setOutputLevel(raw_val_dac);
      String log = F("Lamp set to: ");
      log += UserVar[event->BaseVarIndex];
      log += F("\%");
      addLog(LOG_LEVEL_INFO,log);
      success = true;
      break;
    }

    case PLUGIN_ONCE_A_SECOND:
    {
      // addFormNumericBox(F("Range (1-4095)"), F("p022_range"), range, 1, 4095);
      // mDAC.setOutputLevel(range);
      // String log = F("MCP  : Set DAC: ");
      // log += UserVar[event->BaseVarIndex];
      // addLog(LOG_LEVEL_INFO,log);
      success = true;
      break;
    }
    case PLUGIN_READ:
    {
      // byte unit = (CONFIG_PORT - 1) / 4;
      // byte port = CONFIG_PORT - (unit * 4);
      // uint8_t address = 0x48 + unit;
      //
      // // get the current pin value
      // Wire.beginTransmission(address);
      // Wire.write(port - 1);
      // Wire.endTransmission();
      //
      // Wire.requestFrom(address, (uint8_t)0x2);
      // if (Wire.available())
      // {
      //   Wire.read(); // Read older value first (stored in chip)
      //   UserVar[event->BaseVarIndex] = (float)Wire.read(); // now read actual value and store into Nodo var
      //   String log = F("PCF  : Analog value: ");
      //   log += UserVar[event->BaseVarIndex];
      //   addLog(LOG_LEVEL_INFO,log);
      //   success = true;
      // }
      break;
    }
    case PLUGIN_WRITE:
    {
      String cmd = parseString(string, 1);
      addLog(LOG_LEVEL_INFO, cmd);
      if (cmd.equalsIgnoreCase(F("SetDacCMD")))
      {
        // Commands:
        // SetDacCMD,<value>
        String newDacValue_text = parseString(string, 2);
        uint8_t proc_value_dac = newDacValue_text.toInt();
        PCONFIG(0) = proc_value_dac;
        UserVar[event->BaseVarIndex]=proc_value_dac;

        uint16_t rawValDac = proc_value_dac*4095/100;
        rawValDac = rawValDac%4096;
        mDAC.setOutputLevel(rawValDac);



        String log = F("P097 : SET_DAC= ");
        log+=newDacValue_text;
        addLog(LOG_LEVEL_INFO,log);

        success = true;
      }
      break;
    }
  }
  return success;
}
#endif // USES_P097
