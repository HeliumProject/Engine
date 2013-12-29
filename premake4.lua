require "Helium"

function PublishBundle( bin )

	if os.get() == "windows" then

		os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"Bin\\Debug\\Icons\" *.png")
		os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"Bin\\Intermediate\\Icons\" *.png")
		os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"Bin\\Profile\\Icons\" *.png")
		os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"Bin\\Release\\Icons\" *.png")

	else

		os.execute("mkdir -p Bin/Debug/" .. Helium.GetBundleResourcePath() .. "Icons/")
		os.execute("mkdir -p Bin/Intermediate/" .. Helium.GetBundleResourcePath() .. "Icons/")
		os.execute("mkdir -p Bin/Profile/" .. Helium.GetBundleResourcePath() .. "Icons/")
		os.execute("mkdir -p Bin/Release/" .. Helium.GetBundleResourcePath() .. "Icons/")
		os.execute("rsync -a --delete Editor/Icons/Helium/ Bin/Debug/" .. Helium.GetBundleResourcePath() .. "Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
		os.execute("rsync -a --delete Editor/Icons/Helium/ Bin/Intermediate/" .. Helium.GetBundleResourcePath() .. "Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
		os.execute("rsync -a --delete Editor/Icons/Helium/ Bin/Profile/" .. Helium.GetBundleResourcePath() .. "Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
		os.execute("rsync -a --delete Editor/Icons/Helium/ Bin/Release/" .. Helium.GetBundleResourcePath() .. "Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")

	end

	if os.get() == "macosx" then
		os.copyfile( "Editor/Icons/Helium.icns", "Bin/Debug/" .. Helium.GetBundleResourcePath() )
		os.copyfile( "Editor/Icons/Helium.icns", "Bin/Intermediate/" .. Helium.GetBundleResourcePath() )
		os.copyfile( "Editor/Icons/Helium.icns", "Bin/Profile/" .. Helium.GetBundleResourcePath() )
		os.copyfile( "Editor/Icons/Helium.icns", "Bin/Release/" .. Helium.GetBundleResourcePath() )
		os.copyfile( "Info.plist", "Bin/Debug/" .. Helium.GetBundleConfigPath() )
		os.copyfile( "Info.plist", "Bin/Intermediate/" .. Helium.GetBundleConfigPath() )
		os.copyfile( "Info.plist", "Bin/Profile/" .. Helium.GetBundleConfigPath() )
		os.copyfile( "Info.plist", "Bin/Release/" .. Helium.GetBundleConfigPath() )
	end

end

newoption {
	trigger     = "direct3d",
	description = "Enable Direct3D support"
}

newoption {
	trigger     = "opengl",
	description = "Enable OpenGL support"
}

newoption {
	trigger     = "no-graphics",
	description = "No graphics support"
}

if not _OPTIONS[ "no-graphics" ] then
	if os.get() == "windows" then
		_OPTIONS[ "direct3d" ] = 1
	else
		_OPTIONS[ "opengl" ] = 1
	end
end

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	if _ACTION ~= "clean" then
		PublishBundle()
	end

	solution "Helium"
	Helium.DoBasicSolutionSettings()

	objdir( "Build" )

	configuration "Debug"
		targetdir( "Bin/Debug/" .. Helium.GetBundleExecutablePath() )
		libdirs { "Dependencies/Bin/Debug/" }

	configuration "Intermediate"
		targetdir( "Bin/Intermediate/" .. Helium.GetBundleExecutablePath() )
		libdirs { "Dependencies/Bin/Intermediate/" }

	configuration "Profile"
		targetdir( "Bin/Profile/" .. Helium.GetBundleExecutablePath() )
		libdirs { "Dependencies/Bin/Profile/" }

	configuration "Release"
		targetdir( "Bin/Release/" .. Helium.GetBundleExecutablePath() )
		libdirs { "Dependencies/Bin/Release/" }

	dofile "Runtime.lua"
	dofile "Tools.lua"

	if Helium.GetPremakeVersion() > 4 then
		startproject "Helium-Tools-TestApp"
	end

end