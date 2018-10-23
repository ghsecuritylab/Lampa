return {
   on = {
      devices = {
         'PWM'
      }
   },
   execute = function(domoticz, switch)
	raw_val = math.floor(1024*switch.level/100);

	runcommand = "curl 'http://"..ESP_IP.."/control?cmd=PWM,"..PWM_GPIO..","..raw_val.."'";
	os.execute(runcommand);
	print(runcommand)

   end
}
