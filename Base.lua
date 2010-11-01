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
		"Intermediate",
		"Profile",
		"Release",
	}
	
	configuration {}
		defines
		{
			"XML_STATIC=1",
		}
		flags
		{
			"EnableSSE2",
			"NoMinimalRebuild",
		}
	
	configuration "not no-unicode"
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
	
	for i, platform in ipairs( platforms() ) do
		for j, config in ipairs( configurations() ) do
			configuration( { config, platform } )
				objdir( "Intermediate" ) -- premake seems to automatically add the platform and config name
			configuration( { config, platform } )
				targetdir( "Bin/" .. platform .. "/" .. config )
		end
	end

	configuration "windows"
		defines
		{
			"_WIN32",
			"WIN32",
			"_CRT_SECURE_NO_DEPRECATE",
		}

	configuration "Debug"
		defines
		{
			"_DEBUG",
		}
		flags
		{
			"Symbols",
		}
		
	configuration "Intermediate"
		flags
		{
			"OptimizeSpeed",
			"Symbols",
		}
		
	configuration "Profile"
		defines
		{
			"NDEBUG",
		}
		flags
		{
			"NoFramePointer",
			"OptimizeSpeed",
		}

	configuration "Release"
		defines
		{
			"NDEBUG",
		}
		flags
		{
			"NoFramePointer",
			"OptimizeSpeed",
		}

	configuration { "windows", "Debug" }
		buildoptions
		{
			"/Ob0",
		}

	configuration { "windows", "not Debug" }
		buildoptions
		{
			"/Ob2",
			"/Oi",
		}

end
