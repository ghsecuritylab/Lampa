#ifdef USES_P099


#include <SPI.h>

#define PLUGIN_099
#define PLUGIN_ID_099         99
#define PLUGIN_NAME_099       "Digital Potentiometer"
#define PLUGIN_VALUENAME1_099 "Potentiometer"

uint8_t Plugin_099_SPI_CS_Pin = 15;  // D8
bool Plugin_099_SensorAttached = true;
uint32_t Plugin_099_Sensor_fault = 0;
double Plugin_099_Celsius = 0.0;
#if 0
class mcp411xxx {
private:
  uint8_t m_chipSelectPin;
  SPIClass *m_spiBus;
  uint8_t currentResistance_raw;
  enum {InitialResistnace=255};
public:
  mcp411xxx(){
    m_spiBus = &SPI;
    currentResistance_raw = InitialResistnace;
  }
  void init(uint8_t chipSelectPin);
  void setResistance(uint8_t value);
  uint16_t getcurrentResistance_ohm()
  {
    static const uint8_t max_res_kohm = 100;
    uint16_t retVal = max_res_kohm*currentResistance_raw*10/255;
    return retVal;
  }
  //void shutdownMode();
};

void mcp411xxx::init(uint8_t chipSelectPin)
{
  m_chipSelectPin = chipSelectPin;
  // set the slaveSelectPin as an output:
  pinMode(m_chipSelectPin, OUTPUT);
  // initialize SPI:
  m_spiBus->setHwCs(false);
  m_spiBus->begin();
  //digitalWrite(m_chipSelectPin, HIGH);
  setResistance(InitialResistnace);
}

void mcp411xxx::setResistance(uint8_t value)
{
  currentResistance_raw = value;
  uint16_t temp = (B00010001<<8) | currentResistance_raw;

  digitalWrite(m_chipSelectPin, LOW);
  m_spiBus->transfer16(temp);
  digitalWrite(m_chipSelectPin, HIGH);
}

bool Plugin_099(byte function, struct EventStruct *event, String& string)
{
  bool success = false;
  static mcp411xxx DigitalResistor;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_099;
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
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

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_099);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_099));
        break;
      }

    case PLUGIN_INIT:
      {
        // Get CS Pin
        // If no Pin is in Config we use 15 as default -> Hardware Chip Select on ESP8266
        if (Settings.TaskDevicePin1[event->TaskIndex] != 0)
        {
          // Konvert the GPIO Pin to a Dogotal Puin Number first ...
          Plugin_099_SPI_CS_Pin = Settings.TaskDevicePin1[event->TaskIndex];
        }

        DigitalResistor.init(Plugin_099_SPI_CS_Pin);

        addLog(LOG_LEVEL_INFO, F("P099 : SPI Init"));

        success = true;
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        addFormNote(F("<b>1st GPIO</b> = CS (Usable GPIOs : 0, 2, 4, 5, 15)"));

        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = getFormItemInt(F("plugin_099_maxtype"));
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        String log = F("P099  : R= ");
        UserVar[event->BaseVarIndex] = DigitalResistor.getcurrentResistance_ohm();
        log+=UserVar[event->BaseVarIndex];
        addLog(LOG_LEVEL_INFO,log);
        break;
      }

    case PLUGIN_WRITE:
    {
      String cmd = parseString(string, 1);
      addLog(LOG_LEVEL_INFO, cmd);
      if (cmd.equalsIgnoreCase(F("PotentiometerCMD")))
  		{
        // Commands:
        // PotentiometerCMD,<value>
        String resistanceValue_text = parseString(string, 2);
        uint8_t resistanceValue_uint = resistanceValue_text.toInt();
        String log = F("P099  : SET_R= ");

        DigitalResistor.setResistance(resistanceValue_uint);

        log+=DigitalResistor.getcurrentResistance_ohm();
        addLog(LOG_LEVEL_INFO,log);

        success = true;
      }
      break;
    }
  }
  return success;
}
#endif
#endif // USES_P099
