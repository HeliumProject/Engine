require "Base"

Helium.BuildWxWidgets = function()

	local cwd = os.getcwd()

	if os.get() == "windows" then
		-- This is not yet ready for prime time (as of jom 1.0.3)
		-- local make = "jom.exe -j " .. os.getenv("NUMBER_OF_PROCESSORS") .. " /f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"
		local make = "nmake.exe /f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( "Dependencies/wxWidgets/build/msw" );

		local result
        if _OPTIONS["no-unicode"] then
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=0\"" )
			if result ~= 0 then os.exit( 1 ) end
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=0\"" )
			if result ~= 0 then os.exit( 1 ) end
            if Helium.Build64Bit() then
				result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=0\"" )
				if result ~= 0 then os.exit( 1 ) end
				result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=0\"" )
				if result ~= 0 then os.exit( 1 ) end
			end
        else
            result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=1\"" )
            if result ~= 0 then os.exit( 1 ) end
            result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=1\"" )
            if result ~= 0 then os.exit( 1 ) end
            if Helium.Build64Bit() then
				result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"" )
				if result ~= 0 then os.exit( 1 ) end
				result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"" )
				if result ~= 0 then os.exit( 1 ) end
			end
        end
    elseif os.get() == "macosx" then
		os.chdir( "Dependencies/wxWidgets" );
		
		local flags = " --enable-monolithic --with-osx_cocoa --with-macosx-version-min=10.6 --with-opengl --with-libjpeg=builtin --with-libpng=builtin --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin --with-expat=builtin"
		
		local arch32 = "-arch i386"
		local archFlags32 = " CFLAGS=\"" .. arch32 .. "\" CXXFLAGS=\"" .. arch32 .. "\" CPPFLAGS=\"" .. arch32 .. "\" LDFLAGS=\"" .. arch32 .. "\" OBJCFLAGS=\"" .. arch32 .. "\" OBJCXXFLAGS=\"" .. arch32 .. "\""

		local arch64 = "-arch x86_64"
		local archFlags64 = " CFLAGS=\"" .. arch64 .. "\" CXXFLAGS=\"" .. arch64 .. "\" CPPFLAGS=\"" .. arch64 .. "\" LDFLAGS=\"" .. arch64 .. "\" OBJCFLAGS=\"" .. arch64 .. "\" OBJCXXFLAGS=\"" .. arch64 .. "\""

		function Build( dirName, flags )
			os.mkdir( dirName )
			os.chdir( dirName )

			local result
			result = os.execute( "../configure " .. flags )
			if result ~= 0 then
				os.exit( 1 )
			end

			result = os.execute( "make" )		
			if result ~= 0 then
				os.exit( 1 )
			end
			
			os.chdir( ".." )
		end

		local result
        if _OPTIONS["no-unicode"] then
			Build( "macbuild-debug-32", "--enable-debug --disable-unicode" .. flags .. archFlags32 )
			Build( "macbuild-release-32", "--disable-unicode" .. flags .. archFlags32 )
            if Helium.Build64Bit() then
				Build( "macbuild-debug-64", "--enable-debug --disable-unicode" .. flags .. archFlags64 )
				Build( "macbuild-release-64", "--disable-unicode" .. flags .. archFlags64 )
			end
        else
			Build( "macbuild-debug-unicode-32", "--enable-debug --enable-unicode" .. flags .. archFlags32 )
			Build( "macbuild-release-unicode-32", "--enable-unicode" .. flags .. archFlags32 )
            if Helium.Build64Bit() then
				Build( "macbuild-debug-unicode-64", "--enable-debug --enable-unicode" .. flags .. archFlags64 )
				Build( "macbuild-release-unicode-64", "--enable-unicode" .. flags .. archFlags64 )
			end
        end
	else
		print("Implement support for " .. os.get() .. " to BuildWxWidgets()")
		os.exit(1)
	end
	
	os.chdir( cwd )

end

Helium.CleanWxWidgets = function()

	local cwd = os.getcwd()

	local files = {}
	
	if os.get() == "windows" then
		local make = "nmake.exe -f makefile.vc clean SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( "Dependencies/wxWidgets/build/msw" );

		local result
        if _OPTIONS["no-unicode"] then
            result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=0\"" )
            if result ~= 0 then os.exit( 1 ) end
            result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=0\"" )
            if result ~= 0 then os.exit( 1 ) end
            if Helium.Build64Bit() then
				result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=0\"" )
				if result ~= 0 then os.exit( 1 ) end
				result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=0\"" )
				if result ~= 0 then os.exit( 1 ) end
			end
        else
            result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=1\"" )
            if result ~= 0 then os.exit( 1 ) end
            result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=1\"" )
            if result ~= 0 then os.exit( 1 ) end
            if Helium.Build64Bit() then
				result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"" )
				if result ~= 0 then os.exit( 1 ) end
				result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"" )
				if result ~= 0 then os.exit( 1 ) end
			end
        end
	elseif os.get() == "macosx" then
		os.rmdir( "Dependencies/wxWidgets/macbuild-debug-32" )
		os.rmdir( "Dependencies/wxWidgets/macbuild-release-32" )
		os.rmdir( "Dependencies/wxWidgets/macbuild-debug-64" )
		os.rmdir( "Dependencies/wxWidgets/macbuild-release-64" )
		os.rmdir( "Dependencies/wxWidgets/macbuild-debug-unicode-32" )
		os.rmdir( "Dependencies/wxWidgets/macbuild-release-unicode-32" )
		os.rmdir( "Dependencies/wxWidgets/macbuild-debug-unicode-64" )
		os.rmdir( "Dependencies/wxWidgets/macbuild-release-unicode-64" )
	else
		print("Implement support for " .. os.get() .. " to CleanWxWidgets()")
		os.exit(1)
	end

	os.chdir( cwd )

end

Helium.PublishWxWidgets = function( bin )

	local files = {}

	if os.get() == "windows" then
		if _OPTIONS["no-unicode"] then
			files[1] = { file="wxmsw291d_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Debug" }
			files[2] = { file="wxmsw291d_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Debug" }
			files[3] = { file="wxmsw291_vc_custom.dll",    source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Intermediate" }
			files[4] = { file="wxmsw291_vc_custom.pdb",    source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Intermediate" }
			files[5] = { file="wxmsw291_vc_custom.dll",    source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Profile" }
			files[6] = { file="wxmsw291_vc_custom.pdb",    source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Profile" }
			files[7] = { file="wxmsw291_vc_custom.dll",    source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Release" }
			files[8] = { file="wxmsw291_vc_custom.pdb",    source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Release" }
			if Helium.Build64Bit() then
				files[09] = { file="wxmsw291d_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Debug" }
				files[10] = { file="wxmsw291d_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Debug" }
				files[11] = { file="wxmsw291_vc_custom.dll",    source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Intermediate" }
				files[12] = { file="wxmsw291_vc_custom.pdb",    source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Intermediate" }
				files[13] = { file="wxmsw291_vc_custom.dll",    source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Profile" }
				files[14] = { file="wxmsw291_vc_custom.pdb",    source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Profile" }
				files[15] = { file="wxmsw291_vc_custom.dll",    source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Release" }
				files[16] = { file="wxmsw291_vc_custom.pdb",    source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Release" }
			end
		else
			files[1] = { file="wxmsw291ud_vc_custom.dll",  source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Debug" }
			files[2] = { file="wxmsw291ud_vc_custom.pdb",  source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Debug" }
			files[3] = { file="wxmsw291u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Intermediate" }
			files[4] = { file="wxmsw291u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Intermediate" }
			files[5] = { file="wxmsw291u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Profile" }
			files[6] = { file="wxmsw291u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Profile" }
			files[7] = { file="wxmsw291u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Release" }
			files[8] = { file="wxmsw291u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Release" }
			if Helium.Build64Bit() then
				files[09] = { file="wxmsw291ud_vc_custom.dll",  source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Debug" }
				files[10] = { file="wxmsw291ud_vc_custom.pdb",  source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Debug" }
				files[11] = { file="wxmsw291u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Intermediate" }
				files[12] = { file="wxmsw291u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Intermediate" }
				files[13] = { file="wxmsw291u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Profile" }
				files[14] = { file="wxmsw291u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Profile" }
				files[15] = { file="wxmsw291u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Release" }
				files[16] = { file="wxmsw291u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Release" }
			end       
		end
    elseif os.get() == "macosx" then
		if _OPTIONS["no-unicode"] then
			files[1] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-debug-32/lib",		target=bin .. "/x32/Debug" }
			files[2] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-debug-32/lib",		target=bin .. "/x32/Debug" }
			files[3] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",    	source="Dependencies/wxWidgets/macbuild-release-32/lib",	target=bin .. "/x32/Intermediate" }
			files[4] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",    source="Dependencies/wxWidgets/macbuild-release-32/lib",	target=bin .. "/x32/Intermediate" }
			files[5] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",    	source="Dependencies/wxWidgets/macbuild-release-32/lib",	target=bin .. "/x32/Profile" }
			files[6] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",    source="Dependencies/wxWidgets/macbuild-release-32/lib",	target=bin .. "/x32/Profile" }
			files[7] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",    	source="Dependencies/wxWidgets/macbuild-release-32/lib",	target=bin .. "/x32/Release" }
			files[8] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",    source="Dependencies/wxWidgets/macbuild-release-32/lib",	target=bin .. "/x32/Release" }
			if Helium.Build64Bit() then
				files[09] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-debug-64/lib", 		target=bin .. "/x64/Debug" }
				files[10] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   source="Dependencies/wxWidgets/macbuild-debug-64/lib", 		target=bin .. "/x64/Debug" }
				files[11] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",    	source="Dependencies/wxWidgets/macbuild-release-64/lib",	target=bin .. "/x64/Intermediate" }
				files[12] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   source="Dependencies/wxWidgets/macbuild-release-64/lib",	target=bin .. "/x64/Intermediate" }
				files[13] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",    	source="Dependencies/wxWidgets/macbuild-release-64/lib",	target=bin .. "/x64/Profile" }
				files[14] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   source="Dependencies/wxWidgets/macbuild-release-64/lib",	target=bin .. "/x64/Profile" }
				files[15] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",    	source="Dependencies/wxWidgets/macbuild-release-64/lib",	target=bin .. "/x64/Release" }
				files[16] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   source="Dependencies/wxWidgets/macbuild-release-64/lib",	target=bin .. "/x64/Release" }
			end
		else
			files[1] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",  		source="Dependencies/wxWidgets/macbuild-debug-unicode-32/lib",		target=bin .. "/x32/Debug" }
			files[2] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",  	source="Dependencies/wxWidgets/macbuild-debug-unicode-32/lib",		target=bin .. "/x32/Debug" }
			files[3] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Intermediate" }
			files[4] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Intermediate" }
			files[5] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Profile" }
			files[6] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Profile" }
			files[7] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Release" }
			files[8] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Release" }
			if Helium.Build64Bit() then
				files[09] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",  	source="Dependencies/wxWidgets/macbuild-debug-unicode-64/lib",		target=bin .. "/x64/Debug" }
				files[10] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",  	source="Dependencies/wxWidgets/macbuild-debug-unicode-64/lib",		target=bin .. "/x64/Debug" }
				files[11] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Intermediate" }
				files[12] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Intermediate" }
				files[13] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Profile" }
				files[14] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Profile" }
				files[15] = { file="libwx_osx_cocoau-2.9.1.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Release" }
				files[16] = { file="libwx_osx_cocoau_gl-2.9.1.0.0.dylib",   source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Release" }
			end       
		end   
	else
		print("Implement support for " .. os.get() .. " to PublishWxWidgets()")
		os.exit(1)
	end

    Helium.Publish( files )

end