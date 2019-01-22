return {
   on = {
      devices = {
         'PWM'
      }
   },
   execute = function(domoticz, switch)

   if switch.state == 'Off' then switch.level = 0 end

   if switch.level > 10 then
      runcommand = "curl 'http://"..ESP_IP.."/control?cmd=GPIO,"..LAMP_OFF..",1'";
   else
      runcommand = "curl 'http://"..ESP_IP.."/control?cmd=GPIO,"..LAMP_OFF..",0'";
   end
	os.execute(runcommand);
	print(runcommand)

   -- accoring to mannual, minimal dimming level is 8%, just in case set to 10%
   lamp_level = math.max(switch.level,10);
   -- lamp has reversed logic 0V == max current
   lamp_level = 100-lamp_level;

	raw_val = math.floor(1024*lamp_level/100);

	runcommand = "curl 'http://"..ESP_IP.."/control?cmd=PWM,"..PWM_GPIO..","..raw_val.."'";
	os.execute(runcommand);
	print(runcommand)

   end
}
