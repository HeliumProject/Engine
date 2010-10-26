require "Base"

Helium.BuildWxWidgets = function( wx )

	local cwd = os.getcwd()

	if os.get() == "windows" then
		local make = "nmake.exe -f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( wx .. "/build/msw" );

		local result
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
	else
		print("Implement support for " .. os.get() .. " to BuildWxWidgets()")
		os.exit(1)
	end
	
	os.chdir( cwd )

end

Helium.CleanWxWidgets = function( wx )

	local cwd = os.getcwd()

	local files = {}
	
	if os.get() == "windows" then
		local make = "nmake.exe -f makefile.vc clean SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( wx .. "/build/msw" );

		local result
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
	else
		print("Implement support for " .. os.get() .. " to CleanWxWidgets()")
		os.exit(1)
	end

end

Helium.PublishWxWidgets = function( wx )

	local files = {}
	files[1]  = { file="wxmsw291d_vc_custom.dll",		source=wx .. "/lib/vc_dll", 			target="Bin/x32/Debug" }
	files[2]  = { file="wxmsw291d_vc_custom.pdb",		source=wx .. "/lib/vc_dll", 			target="Bin/x32/Debug" }
	files[3]  = { file="wxmsw291_vc_custom.dll",		source=wx .. "/lib/vc_dll", 			target="Bin/x32/Release" }
	files[4]  = { file="wxmsw291_vc_custom.pdb",		source=wx .. "/lib/vc_dll", 			target="Bin/x32/Release" }
	files[5]  = { file="wxmsw291ud_vc_custom.dll",		source=wx .. "/lib/vc_dll", 			target="Bin/x32/DebugUnicode" }
	files[6]  = { file="wxmsw291ud_vc_custom.pdb",		source=wx .. "/lib/vc_dll", 			target="Bin/x32/DebugUnicode" }
	files[7]  = { file="wxmsw291u_vc_custom.dll",		source=wx .. "/lib/vc_dll", 			target="Bin/x32/ReleaseUnicode" }
	files[8]  = { file="wxmsw291u_vc_custom.pdb",		source=wx .. "/lib/vc_dll", 			target="Bin/x32/ReleaseUnicode" }
	files[9]  = { file="wxmsw291d_vc_custom.dll",		source=wx .. "/lib/vc_amd64_dll", 		target="Bin/x64/Debug" }
	files[10] = { file="wxmsw291d_vc_custom.pdb",		source=wx .. "/lib/vc_amd64_dll", 		target="Bin/x64/Debug" }
	files[11] = { file="wxmsw291_vc_custom.dll",		source=wx .. "/lib/vc_amd64_dll", 		target="Bin/x64/Release" }
	files[12] = { file="wxmsw291_vc_custom.pdb",		source=wx .. "/lib/vc_amd64_dll", 		target="Bin/x64/Release" }
	files[13] = { file="wxmsw291ud_vc_custom.dll",		source=wx .. "/lib/vc_amd64_dll", 		target="Bin/x64/DebugUnicode" }
	files[14] = { file="wxmsw291ud_vc_custom.pdb",		source=wx .. "/lib/vc_amd64_dll", 		target="Bin/x64/DebugUnicode" }
	files[15] = { file="wxmsw291u_vc_custom.dll",		source=wx .. "/lib/vc_amd64_dll", 		target="Bin/x64/ReleaseUnicode" }
	files[16] = { file="wxmsw291u_vc_custom.pdb",		source=wx .. "/lib/vc_amd64_dll", 		target="Bin/x64/ReleaseUnicode" }
	Helium.Publish( files )

end