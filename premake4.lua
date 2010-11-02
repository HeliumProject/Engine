require "Base"
require "TBB"
require "WxWidgets"

local wx = "Dependencies/wxWidgets"
local tbb = "Dependencies/tbb"

if _ACTION ~= "clean" then
	Helium.BuildWxWidgets( wx )
	Helium.PublishWxWidgets( wx )
	Helium.BuildTBB( tbb )
	Helium.PublishTBB( tbb )
else
	Helium.CleanWxWidgets( wx )
	Helium.CleanTBB( tbb )
end

if os.get() == "windows" then
	os.execute("robocopy /MIR /MT \"Editor\\Icons\\Helium\" \"Bin\\x32\\Debug\\Icons\" *.png")
	os.execute("robocopy /MIR /MT \"Editor\\Icons\\Helium\" \"Bin\\x32\\Release\\Icons\" *.png")
	os.execute("robocopy /MIR /MT \"Editor\\Icons\\Helium\" \"Bin\\x32\\DebugUnicode\\Icons\" *.png")
	os.execute("robocopy /MIR /MT \"Editor\\Icons\\Helium\" \"Bin\\x32\\ReleaseUnicode\\Icons\" *.png")
	os.execute("robocopy /MIR /MT \"Editor\\Icons\\Helium\" \"Bin\\x64\\Debug\\Icons\" *.png")
	os.execute("robocopy /MIR /MT \"Editor\\Icons\\Helium\" \"Bin\\x64\\Release\\Icons\" *.png")
	os.execute("robocopy /MIR /MT \"Editor\\Icons\\Helium\" \"Bin\\x64\\DebugUnicode\\Icons\" *.png")
	os.execute("robocopy /MIR /MT \"Editor\\Icons\\Helium\" \"Bin\\x64\\ReleaseUnicode\\Icons\" *.png")
end

newoption
{
	trigger = "no-unicode",
	description = "Disable Unicode support"
}

solution "Dependencies"
Helium.DoDefaultSolutionSettings()
dofile "Dependencies.lua"

solution "Helium"
Helium.DoDefaultSolutionSettings()
dofile "Helium.lua"