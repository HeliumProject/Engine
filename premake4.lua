require "Base"
require "TBB"
require "WxWidgets"

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
	local wx = "Dependencies/wxWidgets"
	local tbb = "Dependencies/tbb"

	if _ACTION ~= "clean" then
		Helium.BuildWxWidgets( wx )
		Helium.PublishWxWidgets( wx )
		Helium.BuildTBB( tbb )
		Helium.PublishTBB( tbb )

		if haveFbx then
			local fbx = "Integrations/FBX"
			Helium.PublishFBX( fbx )
		end

		if haveGranny then
			local granny = "Integrations/Granny/granny_sdk"
			Helium.PublishGranny( granny )
		end

		if os.get() == "windows" then
			if _OPTIONS["no-unicode"] then
				os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"Bin\\x32\\Debug\\Icons\" *.png")
				os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"Bin\\x32\\Release\\Icons\" *.png")
				os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"Bin\\x64\\Debug\\Icons\" *.png")
				os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"Bin\\x64\\Release\\Icons\" *.png")
			else
				os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"Bin\\x32\\DebugUnicode\\Icons\" *.png")
				os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"Bin\\x32\\ReleaseUnicode\\Icons\" *.png")
				os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"Bin\\x64\\DebugUnicode\\Icons\" *.png")
				os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"Bin\\x64\\ReleaseUnicode\\Icons\" *.png")
			end
		end

		Helium.Prebuild()
	else
		Helium.CleanWxWidgets( wx )
		Helium.CleanTBB( tbb )
	end

	solution "Dependencies"
	Helium.DoDefaultSolutionSettings()
	dofile "Dependencies.lua"

	solution "Helium"
	Helium.DoDefaultSolutionSettings()
	dofile "Helium.lua"
end
