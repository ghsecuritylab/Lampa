PWM_GPIO = 2;
LAMP_OFF = 4;
--ESP_IP = '192.168.1.148'
return {
   on = {
      devices = {
         'PWM',
		'Resistor',
      'DAC'
      }
   },
   execute = function(domoticz, switch)
	ESP_IP = domoticz.devices(7).name
	print(ESP_IP);
   end
}
