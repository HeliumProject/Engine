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

	if _ACTION == "xcode3" then
        print("XCode 3 is no longer supported")
        os.exit(1)
	end

	if _ACTION == "vs2005" then
        print("Visual Studio 2005 is no longer supported (no regex library)")
        os.exit(1)
	end

	if _ACTION ~= "clean" then
		local bin = "Bin"

		Helium.BuildWxWidgets()
		Helium.PublishWxWidgets( bin )

		Helium.BuildTBB()
		Helium.PublishTBB( bin )

		Helium.PublishFBX( bin )
		Helium.PublishIcons( bin )
	else
		Helium.CleanWxWidgets()
		Helium.CleanTBB()
	end

	dofile "Dependencies.lua"
	dofile "Runtime.lua"
	dofile "Tools.lua"
	
end