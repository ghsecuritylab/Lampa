-- this scripts holds all the globally persistent variables and helper functions
-- see the documentation in the wiki
-- NOTE:
-- THERE CAN BE ONLY ONE global_data SCRIPT in your Domoticz install.

return {
	-- global persistent data
	data = {
		ESP_IP = { initial = '192.1.1.1' }
	},

	-- global helper functions
	helpers = {
		myHelperFunction = function(domoticz)
			-- code
		end
	}
}
