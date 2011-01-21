require "Base"

--[[

NOTE:
We set cwd to be the location of make because gmake on windows (or mingw) will auto-complete the path to make to a full path
if this full path has spaces then tbb's make logic will fail.  -C will cwd to the tbb root before it starts processing
This is mainly to support building from user folders in XP -Geoff

--]]

Helium.BuildTBB = function()

	local cwd = os.getcwd()

	if os.get() == "windows" then

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		function FlipSlashes( cmd )
			return string.gsub(cmd, "/", "\\")
		end

		function CallMake( arch, cmd )
			local path = os.getenv("TMP") .. os.tmpname() .. ".bat";
			print( "Creating temp bat file for make.exe: " .. path )

			local bat = io.open( path, "w+")		
			bat.write( bat, "@call \"%VCINSTALLDIR%\"\\vcvarsall.bat " .. arch .. "\n" )
			bat.write( bat, "@set PATH=%PATH%;" .. FlipSlashes( os.getcwd() ) .. "\\..\\..\\Utilities\\Win32\n" )
			bat.write( bat, "@make.exe " .. cmd .. "\n" )
			io.close( bat )

			local result = os.execute( "cmd.exe /c \"call \"" .. path .. "\"" )
			os.execute( "cmd.exe /c \"del " .. path .. "\"" )
			return result
		end

		os.chdir( "Dependencies/tbb" )

		local result
        result = CallMake( "x86", "tbb arch=ia32" )
        if result ~= 0 then os.exit( 1 ) end
        if Helium.Build64Bit() then
            result = CallMake( "x86_amd64", "tbb arch=intel64" )
            if result ~= 0 then os.exit( 1 ) end
        end
	else
		print("Implement support for " .. os.get() .. " to BuildTBB()")
		os.exit(1)
	end

	os.chdir( cwd )

end

Helium.CleanTBB = function()

	local cwd = os.getcwd()

	local files = {}
	
	if os.get() == "windows" then

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( "Utilities\\Win32" )

		local result
        result = CallMake( "x86", "clean arch=ia32" )
        if result ~= 0 then os.exit( 1 ) end
        if Helium.Build64Bit() then
            result = os.execute( "x86_amd64", "clean arch=intel64" )
            if result ~= 0 then os.exit( 1 ) end
        end
	else
		print("Implement support for " .. os.get() .. " to CleanTBB()")
		os.exit(1)
	end

	os.chdir( cwd )

end

Helium.PublishTBB = function( bin )

	local files = {}
	
	if _ACTION == "vs2005" then
        files[1] = { file="tbb_debug.dll",		source="Dependencies/tbb/build/windows_ia32_cl_vc8_debug",		    target=bin .. "/x32/Debug" }
        files[2] = { file="tbb_debug.pdb",		source="Dependencies/tbb/build/windows_ia32_cl_vc8_debug",		    target=bin .. "/x32/Debug" }
        files[3] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc8_release",        target=bin .. "/x32/Intermediate" }
        files[4] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc8_release",        target=bin .. "/x32/Intermediate" }
        files[5] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc8_release",        target=bin .. "/x32/Profile" }
        files[6] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc8_release",        target=bin .. "/x32/Profile" }
        files[7] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc8_release",        target=bin .. "/x32/Release" }
        files[8] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc8_release",        target=bin .. "/x32/Release" }
        if Helium.Build64Bit() then		
            files[09] = { file="tbb_debug.dll",		source="Dependencies/tbb/build/windows_intel64_cl_vc8_debug",	    target=bin .. "/x64/Debug" }
            files[10] = { file="tbb_debug.pdb",		source="Dependencies/tbb/build/windows_intel64_cl_vc8_debug",	    target=bin .. "/x64/Debug" }
            files[11] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc8_release",	    target=bin .. "/x64/Intermediate" }
            files[12] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc8_release",	    target=bin .. "/x64/Intermediate" }
            files[13] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc8_release",	    target=bin .. "/x64/Profile" }
            files[14] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc8_release",	    target=bin .. "/x64/Profile" }
            files[15] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc8_release",	    target=bin .. "/x64/Release" }
            files[16] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc8_release",	    target=bin .. "/x64/Release" }
        end       
	elseif _ACTION == "vs2008" then
        files[1] = { file="tbb_debug.dll",		source="Dependencies/tbb/build/windows_ia32_cl_vc9_debug",		    target=bin .. "/x32/Debug" }
        files[2] = { file="tbb_debug.pdb",		source="Dependencies/tbb/build/windows_ia32_cl_vc9_debug",		    target=bin .. "/x32/Debug" }
        files[3] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Intermediate" }
        files[4] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Intermediate" }
        files[5] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Profile" }
        files[6] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Profile" }
        files[7] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Release" }
        files[8] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Release" }
        if Helium.Build64Bit() then
            files[09] = { file="tbb_debug.dll",		source="Dependencies/tbb/build/windows_intel64_cl_vc9_debug",	    target=bin .. "/x64/Debug" }
            files[10] = { file="tbb_debug.pdb",		source="Dependencies/tbb/build/windows_intel64_cl_vc9_debug",	    target=bin .. "/x64/Debug" }
            files[11] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Intermediate" }
            files[12] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Intermediate" }
            files[13] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Profile" }
            files[14] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Profile" }
            files[15] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Release" }
            files[16] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Release" }
        end       
	elseif _ACTION == "vs2010" then 
        files[1] = { file="tbb_debug.dll",		source="Dependencies/tbb/build/windows_ia32_cl_vc10_debug",		    target=bin .. "/x32/Debug" }
        files[2] = { file="tbb_debug.pdb",		source="Dependencies/tbb/build/windows_ia32_cl_vc10_debug",		    target=bin .. "/x32/Debug" }
        files[3] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Intermediate" }
        files[4] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Intermediate" }
        files[5] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Profile" }
        files[6] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Profile" }
        files[7] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Release" }
        files[8] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Release" }
        if Helium.Build64Bit() then
            files[09] = { file="tbb_debug.dll",		source="Dependencies/tbb/build/windows_intel64_cl_vc10_debug",	    target=bin .. "/x64/Debug" }
            files[10] = { file="tbb_debug.pdb",		source="Dependencies/tbb/build/windows_intel64_cl_vc10_debug",	    target=bin .. "/x64/Debug" }
            files[11] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Intermediate" }
            files[12] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Intermediate" }
            files[13] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Profile" }
            files[14] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Profile" }
            files[15] = { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Release" }
            files[16] = { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Release" }
        end       
	end
	
	Helium.Publish( files )

end