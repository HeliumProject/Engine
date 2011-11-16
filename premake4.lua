require "Helium"
require "Prebuild"
require "FBX"
require "TBB"
require "WxWidgets"

-- Do both instruction sets
newoption
{
   trigger = "build32and64",
   description = "Build both 32-bit and 64-bit binaries"
}

-- Custom action to just run the pre-build scripts.
newaction
{
	trigger = "prebuild",
	description = "Run the pre-build scripts without updating project or make files",
	execute = Helium.Prebuild
}

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	-- Check prereqs
	Helium.CheckEnvironment()

	if _ACTION == "vs2005" then
        print("Visual Studio 2005 is no longer supported (no regex library)")
        os.exit(1)
	end

	if _ACTION ~= "prebuild" then
		if _ACTION ~= "clean" then
			local bin = "Bin"

			Helium.BuildWxWidgets()
			Helium.PublishWxWidgets( bin )

			Helium.BuildTBB()
			Helium.PublishTBB( bin )

			Helium.PublishFBX( bin )
			Helium.PublishIcons( bin )

			Helium.Prebuild()
		else
			Helium.CleanWxWidgets()
			Helium.CleanTBB()
		end

		dofile "Dependencies.lua"
		dofile "Runtime.lua"
		dofile "Tools.lua"
	end
	
end