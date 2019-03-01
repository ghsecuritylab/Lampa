
#ifdef USES_P098


#include <SPI.h>


#define PLUGIN_098
#define PLUGIN_ID_098         98
#define PLUGIN_NAME_098       "ADC"
#define PLUGIN_VALUENAME1_098 "ADConverter"

uint8_t Plugin_098_SPI_CS_Pin = 15;  // D8
bool Plugin_098_SensorAttached = true;
uint32_t Plugin_098_Sensor_fault = 0;

union SpiData_t {
  uint16_t value;  /**< value */
  struct {
    uint8_t loByte;  /**< low byte */
    uint8_t hiByte;  /**< high byte */
  };
};

static uint16_t read_adc(uint8_t pin);
static void setup_adc();


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


enum Channel {
  SINGLE_0 = 0b1000,  /**< single channel 0 */
  SINGLE_1 = 0b1001,  /**< single channel 1 */
  SINGLE_2 = 0b1010,  /**< single channel 2 */
  SINGLE_3 = 0b1011,  /**< single channel 3 */
  SINGLE_4 = 0b1100,  /**< single channel 4 */
  SINGLE_5 = 0b1101,  /**< single channel 5 */
  SINGLE_6 = 0b1110,  /**< single channel 6 */
  SINGLE_7 = 0b1111,  /**< single channel 7 */
  DIFF_0PN = 0b0000,  /**< differential channel 0 (input 0+,1-) */
  DIFF_0NP = 0b0001,  /**< differential channel 0 (input 0-,1+) */
  DIFF_1PN = 0b0010,  /**< differential channel 1 (input 2+,3-) */
  DIFF_1NP = 0b0011,  /**< differential channel 1 (input 2-,3+) */
  DIFF_2PN = 0b0100,  /**< differential channel 2 (input 4+,5-) */
  DIFF_2NP = 0b0101,  /**< differential channel 2 (input 5-,5+) */
  DIFF_3PN = 0b0110,  /**< differential channel 3 (input 6+,7-) */
  DIFF_3NP = 0b0111   /**< differential channel 3 (input 6-,7+) */
};
static uint16_t channels[] = {
  SINGLE_0 ,  /**< single channel 0 */
  SINGLE_1,  /**< single channel 1 */
  SINGLE_2,  /**< single channel 2 */
  SINGLE_3,  /**< single channel 3 */
  SINGLE_4,  /**< single channel 4 */
  SINGLE_5,  /**< single channel 5 */
  SINGLE_6,  /**< single channel 6 */
  SINGLE_7,  /**< single channel 7 */
  DIFF_0PN,  /**< differential channel 0 (input 0+,1-) */
  DIFF_0NP,  /**< differential channel 0 (input 0-,1+) */
  DIFF_1PN,  /**< differential channel 1 (input 2+,3-) */
  DIFF_1NP,  /**< differential channel 1 (input 2-,3+) */
  DIFF_2PN,  /**< differential channel 2 (input 4+,5-) */
  DIFF_2NP,  /**< differential channel 2 (input 5-,5+) */
  DIFF_3PN,  /**< differential channel 3 (input 6+,7-) */
  DIFF_3NP   /**< differential channel 3 (input 6-,7+) */
};

  // base command structure
  // 0b000001cccc000000
  // c: channel config

static uint16_t read_adc(uint8_t pin){
    SpiData_t adc;
    SpiData_t cmd;
    cmd.value = static_cast<uint16_t>((0x0400 | (channels[pin] << 6)));


    digitalWrite(Plugin_098_SPI_CS_Pin, LOW);

    // send first command byte
    SPI.transfer(cmd.hiByte);
    // send second command byte and receive first(msb) 4 bits
    adc.hiByte = SPI.transfer(cmd.loByte) & 0x0F;
    // receive last(lsb) 8 bits
    adc.loByte = SPI.transfer(0x00);


    digitalWrite(Plugin_098_SPI_CS_Pin, HIGH);

    return adc.value;
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
        if (CONFIG_PIN1 != 0)
        {
          // Konvert the GPIO Pin to a Dogotal Puin Number first ...
          Plugin_098_SPI_CS_Pin = CONFIG_PIN1;
        }

        // set the slaveSelectPin as an output:
        pinMode(Plugin_098_SPI_CS_Pin, OUTPUT);
        // initialize SPI:
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
          uint8_t port = CONFIG_PORT;
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
