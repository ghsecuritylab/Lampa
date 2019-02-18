#ifdef USES_P098


#include <SPI.h>


#define PLUGIN_098
#define PLUGIN_ID_098         98
#define PLUGIN_NAME_098       "ADC"
#define PLUGIN_VALUENAME1_098 "ADConverter"

uint8_t Plugin_098_SPI_CS_Pin = 15;  // D8
bool Plugin_098_SensorAttached = true;
uint32_t Plugin_098_Sensor_fault = 0;

static void setup_adc()
{
  pinMode(Plugin_098_SPI_CS_Pin, OUTPUT);
  digitalWrite(Plugin_098_SPI_CS_Pin, LOW);
  delayMicroseconds(100);
  digitalWrite(Plugin_098_SPI_CS_Pin, HIGH);
  // initialize SPI:

  SPI.setHwCs(false);
  //SPI.setFrequency(500000);
  SPI.setDataMode(SPI_MODE0); //MODE3 - 1,1 , MODE0 - 0,0
  SPI.begin();
}

union SpiData {
  uint16_t value;  /**< value */
  struct {
    uint8_t loByte;  /**< low byte */
    uint8_t hiByte;  /**< high byte */
  };
};


static uint16_t read_adc(uint8_t pin){
#if 1

    digitalWrite(Plugin_098_SPI_CS_Pin, LOW);
    SPI.transfer(0x6);
    uint16_t tmp3 = SPI.transfer16(0x00);
    tmp3&=0xFFF;
    digitalWrite(Plugin_098_SPI_CS_Pin, HIGH);

    return tmp3;//(tmp1<<8 )| tmp2;
//    SpiData adc;
    // send first command byte
  //  SPI.transfer(addr);
    // send second command byte and receive first(msb) 4 bits
    //delayMicroseconds(100);
  //  adc.value = SPI.transfer16(0x00);
    // receive last(lsb) 8 bits
  //  adc.loByte = m_spiBus->transfer16(0x00);

    // deactivate ADC with slave select


#else

    //uint8_t pin = 2;
    uint8_t addr = 0b01100000 | ((pin & 0b111) << 2);
    digitalWrite(Plugin_098_SPI_CS_Pin, LOW);
  //  delayMicroseconds(100);
    (void) m_spiBus->transfer16(addr);
    //    delayMicroseconds(100);
    uint8_t b1 = m_spiBus->transfer16(0);
      //  delayMicroseconds(100);
    uint8_t b2 = m_spiBus->transfer16(0);
    //    delayMicroseconds(100);
    digitalWrite(Plugin_098_SPI_CS_Pin, HIGH);

    return (b1 << 4) | (b2 >> 4);


#endif
}

boolean Plugin_098(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_098;
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 8;
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
        string = F(PLUGIN_NAME_098);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_098));
        break;
      }

    case PLUGIN_INIT:
      {
        // Get CS Pin
        // If no Pin is in Config we use 15 as default -> Hardware Chip Select on ESP8266
        if (Settings.TaskDevicePin1[event->TaskIndex] != 0)
        {
          // Konvert the GPIO Pin to a Dogotal Puin Number first ...
        //  Plugin_098_SPI_CS_Pin = Settings.TaskDevicePin1[event->TaskIndex];
        }
        // Use the default SPI hardware interface.
        setup_adc();


        addLog(LOG_LEVEL_INFO, F("P098 : SPI Init"));

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
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = getFormItemInt(F("plugin_098_maxtype"));
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
          String log = F("MCP32008  : Analog value: ");
          uint8_t port =Settings.TaskDevicePort[event->TaskIndex];
          log+=port;
          UserVar[event->BaseVarIndex] = (float)read_adc(port); // now read actual value and store into Nodo var
          log += UserVar[event->BaseVarIndex];
          addLog(LOG_LEVEL_INFO,log);

          success = true;

        break;
      }

    case PLUGIN_WRITE:
    {

      break;
    }
  }
  return success;
}

#endif // USES_P098
