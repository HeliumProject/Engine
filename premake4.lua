require "Dependencies/Helium"

function PublishIcons( bin )

	if os.get() == "windows" then
		if Helium.Build32Bit() then
			os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Debug\\Icons\" *.png")
			os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Intermediate\\Icons\" *.png")
			os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Profile\\Icons\" *.png")
			os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Release\\Icons\" *.png")
		end
		if Helium.Build64Bit() then
			os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Debug\\Icons\" *.png")
			os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Intermediate\\Icons\" *.png")
			os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Profile\\Icons\" *.png")
			os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Release\\Icons\" *.png")
		end
	else
		if Helium.Build32Bit() then
			os.execute("mkdir -p " .. bin .. "/x32/Debug/Icons/")
			os.execute("mkdir -p " .. bin .. "/x32/Intermediate/Icons/")
			os.execute("mkdir -p " .. bin .. "/x32/Profile/Icons/")
			os.execute("mkdir -p " .. bin .. "/x32/Release/Icons/")
			os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Debug/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
			os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Intermediate/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
			os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Profile/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
			os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Release/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
		end
		if Helium.Build64Bit() then
			os.execute("mkdir -p " .. bin .. "/x64/Debug/Icons/")
			os.execute("mkdir -p " .. bin .. "/x64/Intermediate/Icons/")
			os.execute("mkdir -p " .. bin .. "/x64/Profile/Icons/")
			os.execute("mkdir -p " .. bin .. "/x64/Release/Icons/")
			os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Debug/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
			os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Intermediate/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
			os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Profile/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
			os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Release/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
		end
	end

end

-- Select renderer. If no explicit choice of renderer, select by OS.
newoption {
   trigger	= "gfxapi",
   value	= "API",
   description	= "Choose a particular 3D API for rendering",
   allowed	= {
      { "opengl",	"OpenGL" },
      { "direct3d",	"Direct3D (Windows only)" }
   }
}
if not _OPTIONS[ "gfxapi" ] then
	if os.get() == "windows" then
		_OPTIONS[ "gfxapi" ] = "direct3d"
	else
		_OPTIONS[ "gfxapi" ] = "opengl"
	end
end

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	if _ACTION ~= "clean" then
		local bin = "Bin"
		local depsBin = "Dependencies/Bin"
		PublishIcons( bin )
		Helium.PublishSharedLibs( depsBin, bin )
	end

	solution "Helium"
	startproject "Helium-Tools-TestApp"

	Helium.DoBasicSolutionSettings()

	dofile "Runtime.lua"
	dofile "Tools.lua"

end