local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/Helium' )

wxVersion = "3.1"
wxVersionShort = "310"
wxVersionShorter = "31"
wxVersionFull = "3.1.0.0.0"
wxVersionBuild = "0"

Helium.BuildWxWidgets = function(debug, release)

	local cwd = os.getcwd()

	if os.get() == "linux" then

		os.chdir( "wxWidgets" );
		
		local flags = " --disable-mediactrl --enable-monolithic --with-opengl --with-libjpeg=builtin --with-libpng=builtin --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin --with-expat=builtin"
		
		local arch32 = " -m32"
		local archFlags32 = " CFLAGS=\"" .. arch32 .. "\" CXXFLAGS=\"-std=c++11 " .. arch32 .. "\" CPPFLAGS=\"" .. arch32 .. "\" LDFLAGS=\"" .. arch32 .. "\""

		local arch64 = " -m64"
		local archFlags64 = " CFLAGS=\"" .. arch64 .. "\" CXXFLAGS=\"-std=c++11 " .. arch64 .. "\" CPPFLAGS=\"" .. arch64 .. "\" LDFLAGS=\"" .. arch64 .. "\""

		function Build( dirName, flags )
			os.mkdir( dirName )
			os.chdir( dirName )

			if not os.isfile( "Makefile" ) then
				Helium.ExecuteAndExpect( "../configure " .. flags, 0 )
			end

			print( "Building with " .. Helium.GetProcessorCount() .. " concurrent jobs" )
			Helium.ExecuteAndExpect( "make -j " .. Helium.GetProcessorCount(), 0 )

			os.chdir( ".." )
		end

		local result

		if Helium.Build32Bit() then
			if debug then
				Build( "linuxbuild-debug-unicode-32", "--enable-debug --enable-unicode" .. flags .. archFlags32 )
			end
			if release then
				Build( "linuxbuild-release-unicode-32", "--enable-unicode" .. flags .. archFlags32 )
			end
		else
			if debug then
				Build( "linuxbuild-debug-unicode-64", "--enable-debug --enable-unicode" .. flags .. archFlags64 )
			end
			if release then
				Build( "linuxbuild-release-unicode-64", "--enable-unicode" .. flags .. archFlags64 )
			end
		end

	elseif os.get() == "macosx" then

		os.chdir( "wxWidgets" );
		
		local osMajor = ''
		local osMinor = ''
		local swVersOutput = Helium.ExecuteAndCapture( "sw_vers" )
		for major, minor, build in string.gmatch( swVersOutput, "ProductVersion:	(%d+).(%d+)" ) do
			osMajor = major
			osMinor = minor
		end

		local flags = " --disable-mediactrl --enable-monolithic --with-osx_cocoa --with-macosx-version-min=" .. osMajor .. "." .. osMinor .. " --with-macosx-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX" .. osMajor .. "." .. osMinor .. ".sdk --with-opengl --with-libjpeg=builtin --with-libpng=builtin --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin --with-expat=builtin CC=clang CXX=clang++"
		
		local arch32 = "-arch i386"
		local archFlags32 = " CFLAGS=\"" .. arch32 .. "\" CXXFLAGS=\"-stdlib=libc++ -std=c++11 " .. arch32 .. "\" CPPFLAGS=\"" .. arch32 .. "\" LDFLAGS=\"-stdlib=libc++ " .. arch32 .. "\" OBJCFLAGS=\"" .. arch32 .. "\" OBJCXXFLAGS=\"-stdlib=libc++ -std=c++11 " .. arch32 .. "\""

		local arch64 = "-arch x86_64"
		local archFlags64 = " CFLAGS=\"" .. arch64 .. "\" CXXFLAGS=\"-stdlib=libc++ -std=c++11 " .. arch64 .. "\" CPPFLAGS=\"" .. arch64 .. "\" LDFLAGS=\"-stdlib=libc++ " .. arch64 .. "\" OBJCFLAGS=\"" .. arch64 .. "\" OBJCXXFLAGS=\"-stdlib=libc++ -std=c++11 " .. arch64 .. "\""

		function Build( dirName, flags )
			os.mkdir( dirName )
			os.chdir( dirName )

			if not os.isfile( "Makefile" ) then
				Helium.ExecuteAndExpect( "../configure " .. flags, 0 )
			end

			print( "Building with " .. Helium.GetProcessorCount() .. " concurrent jobs" )
			Helium.ExecuteAndExpect( "make -j " .. Helium.GetProcessorCount(), 0 )

			os.chdir( ".." )
		end

		local result

		if Helium.Build32Bit() then
			if debug then
				Build( "macbuild-debug-unicode-32", "--enable-debug --enable-unicode" .. flags .. archFlags32 )
				Helium.ExecuteAndExpect( "install_name_tool -id @executable_path/libwx_osx_cocoau-" .. wxVersion .. ".dylib macbuild-debug-unicode-32/lib/libwx_osx_cocoau-" .. wxVersion .. ".dylib", 0 )
				Helium.ExecuteAndExpect( "install_name_tool -id @executable_path/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib macbuild-debug-unicode-32/lib/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib", 0 )
				Helium.ExecuteAndExpect( "install_name_tool -change " .. os.getcwd() .. "/macbuild-debug-unicode-32/lib/libwx_osx_cocoau-" .. wxVersionFull .. ".dylib @executable_path/libwx_osx_cocoau-" .. wxVersion .. ".dylib macbuild-debug-unicode-32/lib/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib", 0 )
			end
			if release then
				Build( "macbuild-release-unicode-32", "--enable-unicode" .. flags .. archFlags32 )
				Helium.ExecuteAndExpect( "install_name_tool -id @executable_path/libwx_osx_cocoau-" .. wxVersion .. ".dylib macbuild-release-unicode-32/lib/libwx_osx_cocoau-" .. wxVersion .. ".dylib", 0 )
				Helium.ExecuteAndExpect( "install_name_tool -id @executable_path/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib macbuild-release-unicode-32/lib/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib", 0 )
				Helium.ExecuteAndExpect( "install_name_tool -change " .. os.getcwd() .. "/macbuild-release-unicode-32/lib/libwx_osx_cocoau-" .. wxVersionFull .. ".dylib @executable_path/libwx_osx_cocoau-" .. wxVersion .. ".dylib macbuild-release-unicode-32/lib/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib", 0 )
			end
		else
			if debug then
				Build( "macbuild-debug-unicode-64", "--enable-debug --enable-unicode" .. flags .. archFlags64 )
				Helium.ExecuteAndExpect( "install_name_tool -id @executable_path/libwx_osx_cocoau-" .. wxVersion .. ".dylib macbuild-debug-unicode-64/lib/libwx_osx_cocoau-" .. wxVersion .. ".dylib", 0 )
				Helium.ExecuteAndExpect( "install_name_tool -id @executable_path/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib macbuild-debug-unicode-64/lib/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib", 0 )
				Helium.ExecuteAndExpect( "install_name_tool -change " .. os.getcwd() .. "/macbuild-debug-unicode-64/lib/libwx_osx_cocoau-" .. wxVersionFull .. ".dylib @executable_path/libwx_osx_cocoau-" .. wxVersion .. ".dylib macbuild-debug-unicode-64/lib/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib", 0 )
			end
			if release then
				Build( "macbuild-release-unicode-64", "--enable-unicode" .. flags .. archFlags64 )
				Helium.ExecuteAndExpect( "install_name_tool -id @executable_path/libwx_osx_cocoau-" .. wxVersion .. ".dylib macbuild-release-unicode-64/lib/libwx_osx_cocoau-" .. wxVersion .. ".dylib", 0 )
				Helium.ExecuteAndExpect( "install_name_tool -id @executable_path/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib macbuild-release-unicode-64/lib/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib", 0 )
				Helium.ExecuteAndExpect( "install_name_tool -change " .. os.getcwd() .. "/macbuild-release-unicode-64/lib/libwx_osx_cocoau-" .. wxVersionFull .. ".dylib @executable_path/libwx_osx_cocoau-" .. wxVersion .. ".dylib macbuild-release-unicode-64/lib/libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib", 0 )
			end
		end

	elseif os.get() == "windows" then

		-- This is not yet ready for prime time (as of jom 1.0.5)
		-- local make = "jom.exe -j " .. os.getenv("NUMBER_OF_PROCESSORS") .. " /f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"
		local make = "nmake.exe /f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end

		os.chdir( "wxWidgets/build/msw" );

		local result
		if Helium.Build32Bit() then
			if debug then
				Helium.ExecuteAndExpect( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " USE_EXCEPTIONS=0 BUILD=debug\"", 0 )
			end
			if release then
				Helium.ExecuteAndExpect( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release\"", 0 )
			end
		else
			if debug then
				Helium.ExecuteAndExpect( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " USE_EXCEPTIONS=0 TARGET_CPU=AMD64 BUILD=debug\"", 0 )
			end
			if release then
				Helium.ExecuteAndExpect( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release\"", 0 )
			end
		end

	else
		print("Implement support for " .. os.get() .. " to BuildWxWidgets()")
		os.exit(1)
	end
	
	os.chdir( cwd )

	local file = io.open("../.git/modules/Dependencies/wxWidgets/info/exclude", "w");
	file:write("build/*\n");
	file:write("linuxbuild-*\n");
	file:write("macbuild-*\n");
	file:write("lib/vc_*/*\n");
	file:write("include/wx/msw/setup.h\n")
	file:write("makefile.bcc\n")
	file:close();

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
				
		os.chdir( "wxWidgets/build/msw" );

		local result
		if Helium.Build32Bit() then
			Helium.ExecuteAndExpect( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=1\"", 0 )
			Helium.ExecuteAndExpect( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=1\"", 0 )
		else
			Helium.ExecuteAndExpect( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"", 0 )
			Helium.ExecuteAndExpect( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86_amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"", 0 )
		end

	elseif os.get() == "macosx" then

		if Helium.Build32Bit() then
			os.rmdir( "wxWidgets/macbuild-debug-32" )
			os.rmdir( "wxWidgets/macbuild-release-32" )
			os.rmdir( "wxWidgets/macbuild-debug-unicode-32" )
			os.rmdir( "wxWidgets/macbuild-release-unicode-32" )
		else
			os.rmdir( "wxWidgets/macbuild-debug-64" )
			os.rmdir( "wxWidgets/macbuild-release-64" )
			os.rmdir( "wxWidgets/macbuild-debug-unicode-64" )
			os.rmdir( "wxWidgets/macbuild-release-unicode-64" )
		end

	elseif os.get() == "macosx" then

		if Helium.Build32Bit() then
			os.rmdir( "wxWidgets/linuxbuild-debug-32" )
			os.rmdir( "wxWidgets/linuxbuild-release-32" )
			os.rmdir( "wxWidgets/linuxbuild-debug-unicode-32" )
			os.rmdir( "wxWidgets/linuxbuild-release-unicode-32" )
		else
			os.rmdir( "wxWidgets/linuxbuild-debug-64" )
			os.rmdir( "wxWidgets/linuxbuild-release-64" )
			os.rmdir( "wxWidgets/linuxbuild-debug-unicode-64" )
			os.rmdir( "wxWidgets/linuxbuild-release-unicode-64" )
		end

	else
		print("Implement support for " .. os.get() .. " to CleanWxWidgets()")
		os.exit(1)
	end

	os.chdir( cwd )

end

Helium.PublishWxWidgets = function( bin, debug, release )

	local files = {}

	if os.get() == "linux" then
		if Helium.Build32Bit() then
			if debug then
				table.insert( files, { file="libwx_gtk2u-" .. wxVersion .. ".so." .. wxVersionBuild,    source="wxWidgets/linuxbuild-debug-unicode-32/lib/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u_gl-" .. wxVersion .. ".so." .. wxVersionBuild, source="wxWidgets/linuxbuild-debug-unicode-32/lib/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			end
			if release then
				table.insert( files, { file="libwx_gtk2u-" .. wxVersion .. ".so." .. wxVersionBuild,    source="wxWidgets/linuxbuild-release-unicode-32/lib/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u_gl-" .. wxVersion .. ".so." .. wxVersionBuild, source="wxWidgets/linuxbuild-release-unicode-32/lib/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u-" .. wxVersion .. ".so." .. wxVersionBuild,    source="wxWidgets/linuxbuild-release-unicode-32/lib/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u_gl-" .. wxVersion .. ".so." .. wxVersionBuild, source="wxWidgets/linuxbuild-release-unicode-32/lib/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u-" .. wxVersion .. ".so." .. wxVersionBuild,    source="wxWidgets/linuxbuild-release-unicode-32/lib/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u_gl-" .. wxVersion .. ".so." .. wxVersionBuild, source="wxWidgets/linuxbuild-release-unicode-32/lib/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
			end
		else
			if debug then
				table.insert( files, { file="libwx_gtk2u-" .. wxVersion .. ".so." .. wxVersionBuild,    source="wxWidgets/linuxbuild-debug-unicode-64/lib/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u_gl-" .. wxVersion .. ".so." .. wxVersionBuild, source="wxWidgets/linuxbuild-debug-unicode-64/lib/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			end
			if release then
				table.insert( files, { file="libwx_gtk2u-" .. wxVersion .. ".so." .. wxVersionBuild,    source="wxWidgets/linuxbuild-release-unicode-64/lib/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u_gl-" .. wxVersion .. ".so." .. wxVersionBuild, source="wxWidgets/linuxbuild-release-unicode-64/lib/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u-" .. wxVersion .. ".so." .. wxVersionBuild,    source="wxWidgets/linuxbuild-release-unicode-64/lib/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u_gl-" .. wxVersion .. ".so." .. wxVersionBuild, source="wxWidgets/linuxbuild-release-unicode-64/lib/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u-" .. wxVersion .. ".so." .. wxVersionBuild,    source="wxWidgets/linuxbuild-release-unicode-64/lib/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_gtk2u_gl-" .. wxVersion .. ".so." .. wxVersionBuild, source="wxWidgets/linuxbuild-release-unicode-64/lib/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
			end
		end
	elseif os.get() == "macosx" then
		if Helium.Build32Bit() then
			if debug then
				table.insert( files, { file="libwx_osx_cocoau-" .. wxVersion .. ".dylib",     source="wxWidgets/macbuild-debug-unicode-32/lib/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib",  source="wxWidgets/macbuild-debug-unicode-32/lib/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			end
			if release then
				table.insert( files, { file="libwx_osx_cocoau-" .. wxVersion .. ".dylib",     source="wxWidgets/macbuild-release-unicode-32/lib/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib",  source="wxWidgets/macbuild-release-unicode-32/lib/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau-" .. wxVersion .. ".dylib",     source="wxWidgets/macbuild-release-unicode-32/lib/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib",  source="wxWidgets/macbuild-release-unicode-32/lib/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau-" .. wxVersion .. ".dylib",     source="wxWidgets/macbuild-release-unicode-32/lib/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib",  source="wxWidgets/macbuild-release-unicode-32/lib/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
			end
		else
			if debug then
				table.insert( files, { file="libwx_osx_cocoau-" .. wxVersion .. ".dylib",  	  source="wxWidgets/macbuild-debug-unicode-64/lib/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib",  source="wxWidgets/macbuild-debug-unicode-64/lib/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			end
			if release then
				table.insert( files, { file="libwx_osx_cocoau-" .. wxVersion .. ".dylib",     source="wxWidgets/macbuild-release-unicode-64/lib/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib",  source="wxWidgets/macbuild-release-unicode-64/lib/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau-" .. wxVersion .. ".dylib",     source="wxWidgets/macbuild-release-unicode-64/lib/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib",  source="wxWidgets/macbuild-release-unicode-64/lib/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau-" .. wxVersion .. ".dylib",     source="wxWidgets/macbuild-release-unicode-64/lib/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="libwx_osx_cocoau_gl-" .. wxVersion .. ".dylib",  source="wxWidgets/macbuild-release-unicode-64/lib/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
			end
		end
	elseif os.get() == "windows" then
		if Helium.Build32Bit() then
			if debug then
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "ud_vc_custom.dll",  source="wxWidgets/lib/vc_dll/", target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "ud_vc_custom.pdb",  source="wxWidgets/lib/vc_dll/", target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			end
			if release then
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.dll",   source="wxWidgets/lib/vc_dll/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.pdb",   source="wxWidgets/lib/vc_dll/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.dll",   source="wxWidgets/lib/vc_dll/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.pdb",   source="wxWidgets/lib/vc_dll/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.dll",   source="wxWidgets/lib/vc_dll/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.pdb",   source="wxWidgets/lib/vc_dll/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
			end
		else
			if debug then
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "ud_vc_custom.dll",  source="wxWidgets/lib/vc_x64_dll/", target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "ud_vc_custom.pdb",  source="wxWidgets/lib/vc_x64_dll/", target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			end
			if release then
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.dll",   source="wxWidgets/lib/vc_x64_dll/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.pdb",   source="wxWidgets/lib/vc_x64_dll/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.dll",   source="wxWidgets/lib/vc_x64_dll/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.pdb",   source="wxWidgets/lib/vc_x64_dll/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.dll",   source="wxWidgets/lib/vc_x64_dll/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
				table.insert( files, { file="wxmsw" .. wxVersionShort .. "u_vc_custom.pdb",   source="wxWidgets/lib/vc_x64_dll/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
			end
		end
	else
		print("Implement support for " .. os.get() .. " to PublishWxWidgets()")
		os.exit(1)
	end

	Helium.Publish( files )

end
