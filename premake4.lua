dofile "Base.lua"
dofile "WxWidgets.lua"

local wx = "Dependencies/wxWidgets"

if _ACTION ~= "clean" then
	Helium.BuildWxWidgets( wx )
	Helium.PublishWxWidgets( wx )
else
	Helium.CleanWxWidgets( wx )
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

solution "Dependencies"
Helium.DoDefaultSolutionSettings()
dofile "Dependencies.lua"

solution "Helium"
Helium.DoDefaultSolutionSettings()
dofile "Helium.lua"