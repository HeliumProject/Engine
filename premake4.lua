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

newoption {
   trigger     = "direct3d",
   description = "Enable Direct3D support"
}

newoption {
   trigger     = "opengl",
   description = "Enable OpenGL support"
}

if os.get() == "windows" then
     _OPTIONS[ "direct3d" ] = 1
else
	_OPTIONS[ "opengl" ] = 1
end

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	if _ACTION ~= "clean" then
		local bin = "Bin"
		PublishIcons( bin )
	end

	solution "Helium"
	startproject "Helium-Tools-TestApp"

	Helium.DoBasicSolutionSettings()

	dofile "Runtime.lua"
	dofile "Tools.lua"

end