require "Helium"

Helium.BuildWxWidgets = function()

	local cwd = os.getcwd()

	if os.get() == "windows" then
	
		-- This is not yet ready for prime time (as of jom 1.0.5)
		-- local make = "jom.exe -j " .. os.getenv("NUMBER_OF_PROCESSORS") .. " /f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"
		local make = "nmake.exe /f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end

		os.chdir( "Dependencies/wxWidgets/build/msw" );

		local result
		if Helium.Build32Bit() then
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug\"" )
			if result ~= 0 then os.exit( 1 ) end
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release\"" )
			if result ~= 0 then os.exit( 1 ) end
		end
		if Helium.Build64Bit() then
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug\"" )
			if result ~= 0 then os.exit( 1 ) end
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release\"" )
			if result ~= 0 then os.exit( 1 ) end
		end
		
    elseif os.get() == "macosx" then

		os.chdir( "Dependencies/wxWidgets" );
		
		local flags = " --enable-monolithic --with-osx_cocoa --with-macosx-version-min=10.6 --with-macosx-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk --with-opengl --with-libjpeg=builtin --with-libpng=builtin --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin --with-expat=builtin"
		
		local arch32 = "-arch i386"
		local archFlags32 = " CFLAGS=\"" .. arch32 .. "\" CXXFLAGS=\"" .. arch32 .. "\" CPPFLAGS=\"" .. arch32 .. "\" LDFLAGS=\"" .. arch32 .. "\" OBJCFLAGS=\"" .. arch32 .. "\" OBJCXXFLAGS=\"" .. arch32 .. "\""

		local arch64 = "-arch x86_64"
		local archFlags64 = " CFLAGS=\"" .. arch64 .. "\" CXXFLAGS=\"" .. arch64 .. "\" CPPFLAGS=\"" .. arch64 .. "\" LDFLAGS=\"" .. arch64 .. "\" OBJCFLAGS=\"" .. arch64 .. "\" OBJCXXFLAGS=\"" .. arch64 .. "\""

		function Build( dirName, flags )
			os.mkdir( dirName )
			os.chdir( dirName )

			if not os.isfile( "Makefile" ) then
				local result
				result = os.execute( "../configure " .. flags )
				if result ~= 0 then
					os.exit( 1 )
				end
			end

			result = os.execute( "make" )		
			if result ~= 0 then
				os.exit( 1 )
			end
			
			os.chdir( ".." )
		end

		local result

		if Helium.Build32Bit() then
			Build( "macbuild-debug-unicode-32", "--enable-debug --enable-unicode" .. flags .. archFlags32 )
			Build( "macbuild-release-unicode-32", "--enable-unicode" .. flags .. archFlags32 )
		end
		if Helium.Build64Bit() then
			Build( "macbuild-debug-unicode-64", "--enable-debug --enable-unicode" .. flags .. archFlags64 )
			Build( "macbuild-release-unicode-64", "--enable-unicode" .. flags .. archFlags64 )
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
		if Helium.Build32Bit() then
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=1\"" )
			if result ~= 0 then os.exit( 1 ) end
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=1\"" )
			if result ~= 0 then os.exit( 1 ) end
		end
		if Helium.Build64Bit() then
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"" )
			if result ~= 0 then os.exit( 1 ) end
			result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"" )
			if result ~= 0 then os.exit( 1 ) end
		end

	elseif os.get() == "macosx" then
		if Helium.Build32Bit() then
			os.rmdir( "Dependencies/wxWidgets/macbuild-debug-32" )
			os.rmdir( "Dependencies/wxWidgets/macbuild-release-32" )
			os.rmdir( "Dependencies/wxWidgets/macbuild-debug-unicode-32" )
			os.rmdir( "Dependencies/wxWidgets/macbuild-release-unicode-32" )
		end
		if Helium.Build64Bit() then
			os.rmdir( "Dependencies/wxWidgets/macbuild-debug-64" )
			os.rmdir( "Dependencies/wxWidgets/macbuild-release-64" )
			os.rmdir( "Dependencies/wxWidgets/macbuild-debug-unicode-64" )
			os.rmdir( "Dependencies/wxWidgets/macbuild-release-unicode-64" )
		end
	else
		print("Implement support for " .. os.get() .. " to CleanWxWidgets()")
		os.exit(1)
	end

	os.chdir( cwd )

end

Helium.PublishWxWidgets = function( bin )

	local files = {}

	if os.get() == "windows" then
		if Helium.Build32Bit() then
			table.insert( files, { file="wxmsw293ud_vc_custom.dll",  source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Debug" } )
			table.insert( files, { file="wxmsw293ud_vc_custom.pdb",  source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Debug" } )
			table.insert( files, { file="wxmsw293u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="wxmsw293u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="wxmsw293u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Profile" } )
			table.insert( files, { file="wxmsw293u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Profile" } )
			table.insert( files, { file="wxmsw293u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Release" } )
			table.insert( files, { file="wxmsw293u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_dll", 			target=bin .. "/x32/Release" } )
		end
		if Helium.Build64Bit() then
			table.insert( files, { file="wxmsw293ud_vc_custom.dll",  source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Debug" } )
			table.insert( files, { file="wxmsw293ud_vc_custom.pdb",  source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Debug" } )
			table.insert( files, { file="wxmsw293u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file="wxmsw293u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file="wxmsw293u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Profile" } )
			table.insert( files, { file="wxmsw293u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Profile" } )
			table.insert( files, { file="wxmsw293u_vc_custom.dll",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Release" } )
			table.insert( files, { file="wxmsw293u_vc_custom.pdb",   source="Dependencies/wxWidgets/lib/vc_amd64_dll", 		target=bin .. "/x64/Release" } )
		end       
    elseif os.get() == "macosx" then
		if Helium.Build32Bit() then
			table.insert( files, { file="libwx_osx_cocoau-2.9.3.0.0.dylib",  	source="Dependencies/wxWidgets/macbuild-debug-unicode-32/lib",		target=bin .. "/x32/Debug" } )
			table.insert( files, { file="libwx_osx_cocoau_gl-2.9.3.0.0.dylib",  source="Dependencies/wxWidgets/macbuild-debug-unicode-32/lib",		target=bin .. "/x32/Debug" } )
			table.insert( files, { file="libwx_osx_cocoau-2.9.3.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="libwx_osx_cocoau_gl-2.9.3.0.0.dylib",  source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="libwx_osx_cocoau-2.9.3.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Profile" } )
			table.insert( files, { file="libwx_osx_cocoau_gl-2.9.3.0.0.dylib",  source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Profile" } )
			table.insert( files, { file="libwx_osx_cocoau-2.9.3.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Release" } )
			table.insert( files, { file="libwx_osx_cocoau_gl-2.9.3.0.0.dylib",  source="Dependencies/wxWidgets/macbuild-release-unicode-32/lib",	target=bin .. "/x32/Release" } )
		end
		if Helium.Build64Bit() then
			table.insert( files, { file="libwx_osx_cocoau-2.9.3.0.0.dylib",  	source="Dependencies/wxWidgets/macbuild-debug-unicode-64/lib",		target=bin .. "/x64/Debug" } )
			table.insert( files, { file="libwx_osx_cocoau_gl-2.9.3.0.0.dylib",  source="Dependencies/wxWidgets/macbuild-debug-unicode-64/lib",		target=bin .. "/x64/Debug" } )
			table.insert( files, { file="libwx_osx_cocoau-2.9.3.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file="libwx_osx_cocoau_gl-2.9.3.0.0.dylib",  source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file="libwx_osx_cocoau-2.9.3.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Profile" } )
			table.insert( files, { file="libwx_osx_cocoau_gl-2.9.3.0.0.dylib",  source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Profile" } )
			table.insert( files, { file="libwx_osx_cocoau-2.9.3.0.0.dylib",   	source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Release" } )
			table.insert( files, { file="libwx_osx_cocoau_gl-2.9.3.0.0.dylib",  source="Dependencies/wxWidgets/macbuild-release-unicode-64/lib",	target=bin .. "/x64/Release" } )
		end       
	else
		print("Implement support for " .. os.get() .. " to PublishWxWidgets()")
		os.exit(1)
	end

    Helium.Publish( files )

end