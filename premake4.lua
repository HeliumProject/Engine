require "Helium"
require "FBX"
require "TBB"
require "WxWidgets"

-- Do both instruction sets
newoption
{
   trigger = "universal",
   description = "Build for both 32-bit and 64-bit target machines"
}

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	-- Check prereqs
	Helium.CheckEnvironment()

	if _ACTION == "vs2005" then
        print("Visual Studio 2005 is no longer supported (no regex library)")
        os.exit(1)
	end

	if _ACTION ~= "clean" then
		local bin = "Bin"

		Helium.BuildWxWidgets()
		Helium.PublishWxWidgets( bin )

		Helium.BuildTbb()
		Helium.PublishTbb( bin )
		Helium.PublishFbx( bin )
		Helium.PublishIcons( bin )
	else
		Helium.CleanWxWidgets()
		Helium.CleanTbb()
	end

	solution "Dependencies"
	Helium.DoDefaultSolutionSettings()
	dofile "Dependencies.lua"

	solution "Helium"
	Helium.DoDefaultSolutionSettings()

	print( "Temporarily defining HELIUM_TOOLS everywhere" )
	defines
	{
		"HELIUM_TOOLS=1",
	}

	prefix = "Runtime."
	dofile "Runtime.lua"

	prefix = "Tools."
	dofile "Tools.lua"
	
end