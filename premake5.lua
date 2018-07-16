require "Helium"

function PublishBundle( bin )

	if os.host() == "windows" then
		os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Source\\Tools\\Editor\\Icons\\Helium\" \"Bin\\Debug\\Icons\" *.png")
		os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Source\\Tools\\Editor\\Icons\\Helium\" \"Bin\\Intermediate\\Icons\" *.png")
		os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Source\\Tools\\Editor\\Icons\\Helium\" \"Bin\\Profile\\Icons\" *.png")
		os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Source\\Tools\\Editor\\Icons\\Helium\" \"Bin\\Release\\Icons\" *.png")
	elseif os.host() == "macosx" then
		os.execute("mkdir -p Bin/Debug/" .. Helium.GetBundleResourcePath())
		os.execute("mkdir -p Bin/Intermediate/" .. Helium.GetBundleResourcePath())
		os.execute("mkdir -p Bin/Profile/" .. Helium.GetBundleResourcePath())
		os.execute("mkdir -p Bin/Release/" .. Helium.GetBundleResourcePath())
		os.copyfile( "Source/Tools/Editor/Icons/Helium.icns", "Bin/Debug/" .. Helium.GetBundleResourcePath() )
		os.copyfile( "Source/Tools/Editor/Icons/Helium.icns", "Bin/Intermediate/" .. Helium.GetBundleResourcePath() )
		os.copyfile( "Source/Tools/Editor/Icons/Helium.icns", "Bin/Profile/" .. Helium.GetBundleResourcePath() )
		os.copyfile( "Source/Tools/Editor/Icons/Helium.icns", "Bin/Release/" .. Helium.GetBundleResourcePath() )
		os.copyfile( "Info.plist", "Bin/Debug/" .. Helium.GetBundleConfigPath() )
		os.copyfile( "Info.plist", "Bin/Intermediate/" .. Helium.GetBundleConfigPath() )
		os.copyfile( "Info.plist", "Bin/Profile/" .. Helium.GetBundleConfigPath() )
		os.copyfile( "Info.plist", "Bin/Release/" .. Helium.GetBundleConfigPath() )
	elseif os.host() == "linux" then
		os.execute("mkdir -p Bin/Debug/" .. Helium.GetBundleResourcePath() .. "Icons/")
		os.execute("mkdir -p Bin/Intermediate/" .. Helium.GetBundleResourcePath() .. "Icons/")
		os.execute("mkdir -p Bin/Profile/" .. Helium.GetBundleResourcePath() .. "Icons/")
		os.execute("mkdir -p Bin/Release/" .. Helium.GetBundleResourcePath() .. "Icons/")
		os.execute("rsync -a --delete Source/Tools/Editor/Icons/Helium/ Bin/Debug/" .. Helium.GetBundleResourcePath() .. "Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
		os.execute("rsync -a --delete Source/Tools/Editor/Icons/Helium/ Bin/Intermediate/" .. Helium.GetBundleResourcePath() .. "Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
		os.execute("rsync -a --delete Source/Tools/Editor/Icons/Helium/ Bin/Profile/" .. Helium.GetBundleResourcePath() .. "Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
		os.execute("rsync -a --delete Source/Tools/Editor/Icons/Helium/ Bin/Release/" .. Helium.GetBundleResourcePath() .. "Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
	end

end

newoption {
	trigger	= "core",
	description	= "Core components only",
}

newoption {
	trigger	= "gfxapi",
	value	= "API",
	description	= "Choose a particular 3D API for rendering",
	allowed	= {
		{ "opengl",	"OpenGL" },
		{ "direct3d",	"Direct3D (Windows only)" },
		{ "none", 	"No Renderer" }
	}
}

if not _OPTIONS[ "gfxapi" ] then
	if os.host() == "windows" then
		_OPTIONS[ "gfxapi" ] = "direct3d"
	else
		_OPTIONS[ "gfxapi" ] = "opengl"
	end
end

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	if _ACTION ~= "clean" then
		PublishBundle()
	end

	workspace "Helium"
	startproject "Helium-Tools-Editor"

	Helium.DoBasicWorkspaceSettings()

	configuration "Debug"
		targetdir( "Bin/Debug/" .. Helium.GetBundleExecutablePath() )
		libdirs { "Bin/Debug/" .. Helium.GetBundleExecutablePath() }

	configuration "Intermediate"
		targetdir( "Bin/Intermediate/" .. Helium.GetBundleExecutablePath() )
		libdirs { "Bin/Intermediate/" .. Helium.GetBundleExecutablePath() }

	configuration "Profile"
		targetdir( "Bin/Profile/" .. Helium.GetBundleExecutablePath() )
		libdirs { "Bin/Profile/" .. Helium.GetBundleExecutablePath() }

	configuration "Release"
		targetdir( "Bin/Release/" .. Helium.GetBundleExecutablePath() )
		libdirs { "Bin/Release/" .. Helium.GetBundleExecutablePath() }

	tools = false
	dofile "Runtime.lua"

	tools = true
	dofile "Tools.lua"
	
end