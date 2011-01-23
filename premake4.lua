require "Base"
require "TBB"
require "WxWidgets"

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	-- Check prereqs
	Helium.CheckEnvironment()

	-- Check for FBX integration.
	haveFbx = os.isfile( "Integrations/FBX/FBX.lua" )
	if haveFbx then
		require "Integrations/Fbx/Fbx"
	end

	-- Check for Granny integration.
	haveGranny = os.isfile( "Integrations/Granny/Granny.lua" )
	if haveGranny then
		require "Integrations/Granny/Granny"
	end

	-- Custom action to just run the pre-build scripts.
	newaction
	{
		trigger = "prebuild",
		description = "Run the pre-build scripts without updating project or make files",
		execute = Helium.Prebuild
	}

	-- Custom option to set the global Unicode (wchar_t/UTF-16,32)) vs. ASCII (char/UTF-8)
	newoption
	{
		trigger = "no-unicode",
		description = "Disable Unicode support"
	}

	if _ACTION ~= "prebuild" then

		if _ACTION ~= "clean" then
			local bin = "Bin"

			Helium.BuildWxWidgets()
			Helium.PublishWxWidgets( bin )
			Helium.BuildTBB()
			Helium.PublishTBB( bin )

			if haveFbx then
				Helium.PublishFBX( bin )
			end

			if haveGranny then
				Helium.PublishGranny( bin )
			end

			Helium.PublishIcons( bin )

			Helium.Prebuild()
		else
			Helium.CleanWxWidgets()
			Helium.CleanTBB()
		end

		solution "Dependencies"
		Helium.DoDefaultSolutionSettings()
		dofile "Dependencies.lua"

		solution "Helium"
		Helium.DoDefaultSolutionSettings()
		dofile "Helium.lua"
	end
	
end