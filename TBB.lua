require "Base"

Helium.BuildTBB = function( tbb )

	local cwd = os.getcwd()

	if os.get() == "windows" then
		local make = "make.exe "

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( tbb );

		local result
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " tbb arch=ia32\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " tbb arch=intel64\"" )
		if result ~= 0 then os.exit( 1 ) end
	else
		print("Implement support for " .. os.get() .. " to BuildTBB()")
		os.exit(1)
	end

	os.chdir( cwd )

end

Helium.CleanTBB = function( tbb )

	local cwd = os.getcwd()

	local files = {}
	
	if os.get() == "windows" then
		local make = "make.exe "

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( tbb );

		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " clean arch=ia32\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " clean arch=intel64\"" )
		if result ~= 0 then os.exit( 1 ) end
	else
		print("Implement support for " .. os.get() .. " to CleanTBB()")
		os.exit(1)
	end

	os.chdir( cwd )

end

Helium.PublishTBB = function( tbb )

	local files = {}
	
	if _ACTION == "vs2005" then
		files[1]  = { file="tbb_debug.dll",		source=tbb .. "/build/windows_ia32_cl_vc8_debug",		target="Bin/x32/Debug" }
		files[2]  = { file="tbb_debug.pdb",		source=tbb .. "/build/windows_ia32_cl_vc8_debug",		target="Bin/x32/Debug" }
		files[3]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc8_release",		target="Bin/x32/Intermediate" }
		files[4]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc8_release",		target="Bin/x32/Intermediate" }
		files[5]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc8_release",		target="Bin/x32/Profile" }
		files[6]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc8_release",		target="Bin/x32/Profile" }
		files[7]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc8_release",		target="Bin/x32/Release" }
		files[8]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc8_release",		target="Bin/x32/Release" }
		files[9]  = { file="tbb_debug.dll",		source=tbb .. "/build/windows_intel64_cl_vc8_debug",	target="Bin/x64/Debug" }
		files[10] = { file="tbb_debug.pdb",		source=tbb .. "/build/windows_intel64_cl_vc8_debug",	target="Bin/x64/Debug" }
		files[11] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc8_release",	target="Bin/x64/Intermediate" }
		files[12] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc8_release",	target="Bin/x64/Intermediate" }
		files[13] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc8_release",	target="Bin/x64/Profile" }
		files[14] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc8_release",	target="Bin/x64/Profile" }
		files[15] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc8_release",	target="Bin/x64/Release" }
		files[16] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc8_release",	target="Bin/x64/Release" }
	elseif _ACTION == "vs2008" then
		files[1]  = { file="tbb_debug.dll",		source=tbb .. "/build/windows_ia32_cl_vc9_debug",		target="Bin/x32/Debug" }
		files[2]  = { file="tbb_debug.pdb",		source=tbb .. "/build/windows_ia32_cl_vc9_debug",		target="Bin/x32/Debug" }
		files[3]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc9_release",		target="Bin/x32/Intermediate" }
		files[4]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc9_release",		target="Bin/x32/Intermediate" }
		files[5]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc9_release",		target="Bin/x32/Profile" }
		files[6]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc9_release",		target="Bin/x32/Profile" }
		files[7]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc9_release",		target="Bin/x32/Release" }
		files[8]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc9_release",		target="Bin/x32/Release" }
		files[9]  = { file="tbb_debug.dll",		source=tbb .. "/build/windows_intel64_cl_vc9_debug",	target="Bin/x64/Debug" }
		files[10] = { file="tbb_debug.pdb",		source=tbb .. "/build/windows_intel64_cl_vc9_debug",	target="Bin/x64/Debug" }
		files[11] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc9_release",	target="Bin/x64/Intermediate" }
		files[12] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc9_release",	target="Bin/x64/Intermediate" }
		files[13] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc9_release",	target="Bin/x64/Profile" }
		files[14] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc9_release",	target="Bin/x64/Profile" }
		files[15] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc9_release",	target="Bin/x64/Release" }
		files[16] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc9_release",	target="Bin/x64/Release" }
	elseif _ACTION == "vs2010" then 
		files[1]  = { file="tbb_debug.dll",		source=tbb .. "/build/windows_ia32_cl_vc10_debug",		target="Bin/x32/Debug" }
		files[2]  = { file="tbb_debug.pdb",		source=tbb .. "/build/windows_ia32_cl_vc10_debug",		target="Bin/x32/Debug" }
		files[3]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc10_release",	target="Bin/x32/Intermediate" }
		files[4]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc10_release",	target="Bin/x32/Intermediate" }
		files[5]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc10_release",	target="Bin/x32/Profile" }
		files[6]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc10_release",	target="Bin/x32/Profile" }
		files[7]  = { file="tbb.dll",			source=tbb .. "/build/windows_ia32_cl_vc10_release",	target="Bin/x32/Release" }
		files[8]  = { file="tbb.pdb",			source=tbb .. "/build/windows_ia32_cl_vc10_release",	target="Bin/x32/Release" }
		files[9]  = { file="tbb_debug.dll",		source=tbb .. "/build/windows_intel64_cl_vc10_debug",	target="Bin/x64/Debug" }
		files[10] = { file="tbb_debug.pdb",		source=tbb .. "/build/windows_intel64_cl_vc10_debug",	target="Bin/x64/Debug" }
		files[11] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc10_release",	target="Bin/x64/Intermediate" }
		files[12] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc10_release",	target="Bin/x64/Intermediate" }
		files[13] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc10_release",	target="Bin/x64/Profile" }
		files[14] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc10_release",	target="Bin/x64/Profile" }
		files[15] = { file="tbb.dll",			source=tbb .. "/build/windows_intel64_cl_vc10_release",	target="Bin/x64/Release" }
		files[16] = { file="tbb.pdb",			source=tbb .. "/build/windows_intel64_cl_vc10_release",	target="Bin/x64/Release" }
	end
	
	Helium.Publish( files )

end