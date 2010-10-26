Helium = {}

Helium.Sleep = function( seconds )
	if os.get() == "windows" then
		os.execute("ping 127.0.0.1 -n " .. seconds + 1 .. " -w 1000 >:nul 2>&1")
	else
		os.execute("sleep " .. seconds)
	end
end

Helium.Publish = function( files )
	for i,v in pairs(files) do
		-- mkpath the target folder
		os.mkdir( v.target )
		
		local path = v.source .. "/" .. v.file			
		local exists = os.isfile( path )
		local destination = v.target .. "/" .. v.file
		
		-- cull existing files
		if os.isfile( destination ) then
			local delCommand = "del /q \"" .. string.gsub( destination, "/", "\\" ) .. "\""
			os.execute( delCommand )
		end

		-- do the file copy
		local linkCommand = "mklink /h \"" .. destination .. "\" \"" .. path .. "\""
		local result = os.execute( linkCommand )

		-- the files were copied, complete this entry
		if result == 0 then
			files[ i ] = nil
		else
			os.exit( 1 )
		end						
	end
end

Helium.DoDefaultSolutionSettings = function()

	location "Premake"
	
    platforms
    {
		"x32",
		"x64",
	}

	configurations
	{
		"Debug",
		"DebugUnicode",
		"Release",
		"ReleaseUnicode",
	}

--[[
	Keep in mind that solution-wide settings should be defined
	before ever mentioning configuration-specific settings because
	the indentation in this file is for humans (not premake), and
	premake might infer conditionals when walking the graph of 
	solutions/configurations/projects -Geoff
--]]

	configuration "windows"
		defines
		{
			"_WIN32",
			"WIN32",
			"_CRT_SECURE_NO_DEPRECATE",
		}
		flags
		{
			"NoMinimalRebuild",
		}

	configuration "Debug*"
		defines
		{
			"_DEBUG",
		}
		flags
		{
			"Symbols",
		}

	configuration "Release*"
		defines
		{
			"NDEBUG",
		}
		flags
		{
			"Optimize",
		}

	configuration "*Unicode"
		defines
		{
			"UNICODE=1",
			"LITESQL_UNICODE=1",
			"XML_UNICODE_WCHAR_T=1",
		}
		flags
		{
			"Unicode",
		}

end