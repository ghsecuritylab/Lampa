return {
   on = {
      devices = {
         'Resistor'
      }
   },
   execute = function(domoticz, switch)
	raw_val = math.floor(255*switch.level/100);

	runcommand = "curl 'http://"..ESP_IP.."/tools?cmd=PotentiometerCMD%2C" ..raw_val.."'";
	os.execute(runcommand);
	print(runcommand)

   end
}
