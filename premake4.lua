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

-- Use wchar_t support for strings
newoption
{
	trigger = "wchar_t",
	description = "Build using wchar_t instead of UTF-8 strings"
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

		Helium.BuildTbb()
		Helium.PublishTbb( bin )
		Helium.PublishFbx( bin )
		Helium.PublishIcons( bin )
	else
		Helium.CleanWxWidgets()
		Helium.CleanTbb()
	end

	dofile "Dependencies.lua"
	dofile "Runtime.lua"
	dofile "Tools.lua"
	
end