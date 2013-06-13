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

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	if _ACTION ~= "clean" then
	
		local bin = "Bin"

		PublishIcons( bin )
	
	end

	solution "Runtime"
    Helium.DoBasicSolutionSettings()

    if os.get() == "macosx" then
        buildoptions
        {
            "-std=c++11",
            "-stdlib=libc++",
        }
    end

	dofile "Runtime.lua"

	solution "Tools"
	Helium.DoBasicSolutionSettings()

    if os.get() == "macosx" then
        buildoptions
        {
            "-std=c++11",
            "-stdlib=libc++",
        }
    end

	dofile "Tools.lua"

    if _ACTION == "gmake" then
        if os.isfile( "Makefile." .. os.get() ) then
            os.copyfile( "Makefile." .. os.get(), "Makefile" )
        end
    end

end