require "Dependencies/Helium"

function PublishIcons( bin )

    if os.get() == "windows" then
        os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Debug\\Icons\" *.png")
        os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Intermediate\\Icons\" *.png")
        os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Profile\\Icons\" *.png")
        os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Release\\Icons\" *.png")
        if Helium.Build64Bit() then
            os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Debug\\Icons\" *.png")
            os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Intermediate\\Icons\" *.png")
            os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Profile\\Icons\" *.png")
            os.execute("robocopy /njs /nfl /ndl /nc /ns /np /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Release\\Icons\" *.png")
        end
    else
        os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Debug/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Intermediate/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Profile/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Release/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        if Helium.Build64Bit() then
            os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Debug/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
            os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Intermediate/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
            os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Profile/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
            os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Release/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        end
    end

end

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	if _ACTION ~= "clean" then
	
		local bin = "Bin"

		PublishIcons( bin )
	
	end

	solution "Runtime"
    Helium.DoBasicSolutionSettings()
    location( "Premake" )
	dofile "Runtime.lua"

	solution "Tools"
	Helium.DoBasicSolutionSettings()
    location( "Premake" )
	dofile "Tools.lua"

end