return {
   on = {
      devices = {
         'DAC'
      }
   },
   execute = function(domoticz, switch)

   if switch.state == 'Off' then switch.level = 0 end
   if switch.state == 'On' then switch.level = 100 end

   if switch.level <= 7 then
      runcommand = "curl 'http://"..ESP_IP.."/control?cmd=GPIO,"..LAMP_OFF..",0'";
   else
      -- accoring to mannual, minimal dimming level is 8%
      lamp_level = math.max(switch.level,8);

	   runcommand = "curl 'http://"..ESP_IP.."/control?cmd=SetDacCMD," ..lamp_level.."'";
   end

	os.execute(runcommand);
	print(runcommand)

   end
}
