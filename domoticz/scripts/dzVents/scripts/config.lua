PWM_GPIO = 0;

return {
   on = {
      devices = {
         'PWM',
		'Resistor'
      }
   },
   execute = function(domoticz, switch)
	ESP_IP = domoticz.devices(7).name
	print(ESP_IP);
   end
}
