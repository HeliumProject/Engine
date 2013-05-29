local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/Helium' )

--[[

NOTE:
We set cwd to be the location of make because gmake on windows (or mingw) will auto-complete the path to make to a full path
if this full path has spaces then tbb's make logic will fail.  -C will cwd to the tbb root before it starts processing
This is mainly to support building from user folders in XP -Geoff

--]]

function FlipSlashes( cmd )
	return string.gsub(cmd, "/", "\\")
end

function CallMake( arch, cmd )
	local path = os.getenv("TMP") .. os.tmpname() .. ".bat";
	print( "Creating temp bat file for make.exe: " .. path )

	local bat = io.open( path, "w+")		
	bat.write( bat, "@call \"%VCINSTALLDIR%\"\\vcvarsall.bat " .. arch .. "\n" )
	bat.write( bat, "@" .. FlipSlashes( os.getcwd() ) .. "\\..\\..\\Utilities\\Win32\\make.exe " .. cmd .. "\n" )
	io.close( bat )

	local result = os.execute( "cmd.exe /c \"call \"" .. path .. "\"" )
	--os.execute( "cmd.exe /c \"del " .. path .. "\"" )
	return result
end

Helium.BuildTbb = function()

	local cwd = os.getcwd()

	if os.get() == "windows" then

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
		
		os.chdir( "tbb" )

		local result
		if Helium.Build32Bit() then
	        result = CallMake( "x86", "tbb arch=ia32" )
	        if result ~= 0 then os.exit( 1 ) end
	    end
        if Helium.Build64Bit() then
            result = CallMake( "x86_amd64", "tbb arch=intel64" )
            if result ~= 0 then os.exit( 1 ) end
        end
	elseif os.get() == "macosx" then
		os.chdir( "tbb" )

		local result
		if Helium.Build32Bit() then
	        result = os.execute( "make tbb arch=ia32" )
	        if result ~= 0 then os.exit( 1 ) end
	    end
        if Helium.Build64Bit() then
            result = os.execute( "make tbb arch=intel64" )
            if result ~= 0 then os.exit( 1 ) end
        end
	else
		print("Implement support for " .. os.get() .. " to BuildTBB()")
		os.exit(1)
	end

	os.chdir( cwd )

	local file = io.open("../.git/modules/Dependencies/ThreadBuildingBlocks/info/exclude", "w");
	file:write("build/*\n");
	file:close();

end

Helium.CleanTbb = function()

	local cwd = os.getcwd()

	local files = {}
	
	if os.get() == "windows" then

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
		
		os.chdir( "tbb" )
		
		local result
		if Helium.Build32Bit() then
	        result = CallMake( "x86", "clean arch=ia32" )
	        if result ~= 0 then os.exit( 1 ) end
	    end
        if Helium.Build64Bit() then
            result = CallMake( "x86_amd64", "clean arch=intel64" )
            if result ~= 0 then os.exit( 1 ) end
        end

	elseif os.get() == "macosx" then
		os.chdir( "tbb" )

		if Helium.Build32Bit() then
			local result
	        result = os.execute( "make clean arch=ia32" )
	        if result ~= 0 then os.exit( 1 ) end
	    end
        if Helium.Build64Bit() then
            result = os.execute( "make clean arch=intel64" )
            if result ~= 0 then os.exit( 1 ) end
        end
	else
		print("Implement support for " .. os.get() .. " to CleanTBB()")
		os.exit(1)
	end

	os.chdir( cwd )

end

Helium.PublishTbb = function( bin )

	local files = {}
	
	if os.get() == "windows" then
		if _ACTION == "vs2008" then
			if Helium.Build32Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="tbb/build/windows_ia32_cl_vc9_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="tbb/build/windows_ia32_cl_vc9_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Release" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Release" } )
			end
			if Helium.Build64Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="tbb/build/windows_intel64_cl_vc9_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="tbb/build/windows_intel64_cl_vc9_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Release" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Release" } )
			end
		elseif _ACTION == "vs2010" then 
			if Helium.Build32Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="tbb/build/windows_ia32_cl_vc10_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="tbb/build/windows_ia32_cl_vc10_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Release" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Release" } )
			end
			if Helium.Build64Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="tbb/build/windows_intel64_cl_vc10_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="tbb/build/windows_intel64_cl_vc10_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc10_release",		target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc10_release",		target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc10_release",		target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc10_release",		target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc10_release",		target=bin .. "/x64/Release" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc10_release",		target=bin .. "/x64/Release" } )
			end
		elseif _ACTION == "vs2012" then 
			if Helium.Build32Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="tbb/build/windows_ia32_cl_vc11_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="tbb/build/windows_ia32_cl_vc11_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc11_release",	    target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc11_release",	    target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc11_release",	    target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc11_release",	    target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_ia32_cl_vc11_release",	    target=bin .. "/x32/Release" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_ia32_cl_vc11_release",	    target=bin .. "/x32/Release" } )
			end
			if Helium.Build64Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="tbb/build/windows_intel64_cl_vc11_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="tbb/build/windows_intel64_cl_vc11_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc11_release",		target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc11_release",		target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc11_release",		target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc11_release",		target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.dll",			source="tbb/build/windows_intel64_cl_vc11_release",		target=bin .. "/x64/Release" } )
				table.insert( files, { file="tbb.pdb",			source="tbb/build/windows_intel64_cl_vc11_release",		target=bin .. "/x64/Release" } )
			end
		end
	elseif os.get() == "macosx" then
		local ver = os.getversion()
		local str = string.format("%d.%d.%d", ver.majorversion, ver.minorversion, ver.revision)
		if Helium.Build32Bit() then
			table.insert( files, { file="libtbb_debug.dylib",	source="tbb/build/macos_ia32_gcc_cc4.2.1_os" .. str .. "_debug",		    target=bin .. "/x32/Debug" } )
			table.insert( files, { file="libtbb.dylib",			source="tbb/build/macos_ia32_gcc_cc4.2.1_os" .. str .. "_release",         	target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="libtbb.dylib",			source="tbb/build/macos_ia32_gcc_cc4.2.1_os" .. str .. "_release",         	target=bin .. "/x32/Profile" } )
			table.insert( files, { file="libtbb.dylib",			source="tbb/build/macos_ia32_gcc_cc4.2.1_os" .. str .. "_release",         	target=bin .. "/x32/Release" } )
		end
		if Helium.Build64Bit() then		
			table.insert( files, { file="libtbb_debug.dylib",	source="tbb/build/macos_intel64_gcc_cc4.2.1_os" .. str .. "_debug",	    	target=bin .. "/x64/Debug" } )
			table.insert( files, { file="libtbb.dylib",			source="tbb/build/macos_intel64_gcc_cc4.2.1_os" .. str .. "_release",	    target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file="libtbb.dylib",			source="tbb/build/macos_intel64_gcc_cc4.2.1_os" .. str .. "_release",	    target=bin .. "/x64/Profile" } )
			table.insert( files, { file="libtbb.dylib",			source="tbb/build/macos_intel64_gcc_cc4.2.1_os" .. str .. "_release",	    target=bin .. "/x64/Release" } )
		end       
	else
		print("Implement support for " .. os.get() .. " to PublishTBB()")
		os.exit(1)
	end

	Helium.Publish( files )

end

Helium.DoTbbProjectSettings = function( bin )

	configuration {}

	includedirs
	{
		"Dependencies/tbb/include",
	}


	if _ACTION == "vs2008" then
		configuration { "windows", "x32", "Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_ia32_cl_vc9_debug",
			}

		configuration { "windows", "x32", "not Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_ia32_cl_vc9_release",
			}

		configuration { "windows", "x64", "Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_intel64_cl_vc9_debug",
			}

		configuration { "windows", "x64", "not Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_intel64_cl_vc9_release",
			}
	elseif _ACTION == "vs2010" then
		configuration { "windows", "x32", "Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_ia32_cl_vc10_debug",
			}

		configuration { "windows", "x32", "not Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_ia32_cl_vc10_release",
			}

		configuration { "windows", "x64", "Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_intel64_cl_vc10_debug",
			}

		configuration { "windows", "x64", "not Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_intel64_cl_vc10_release",
			}
	elseif _ACTION == "vs2012" then
		configuration { "windows", "x32", "Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_ia32_cl_vc11_debug",
			}

		configuration { "windows", "x32", "not Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_ia32_cl_vc11_release",
			}

		configuration { "windows", "x64", "Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_intel64_cl_vc11_debug",
			}

		configuration { "windows", "x64", "not Debug" }
			libdirs
			{
				"Dependencies/tbb/build/windows_intel64_cl_vc11_release",
			}
	elseif _ACTION == "xcode4" then
		configuration { "windows", "x32", "Debug" }
			libdirs
			{
				"Dependencies/tbb/build/macos_ia32_gcc_cc4.2.1_os10.6.8_debug",
			}

		configuration { "windows", "x32", "not Debug" }
			libdirs
			{
				"Dependencies/tbb/build/macos_ia32_gcc_cc4.2.1_os10.6.8_release",
			}

		configuration { "windows", "x64", "Debug" }
			libdirs
			{
				"Dependencies/tbb/build/macos_intel64_gcc_cc4.2.1_os10.6.8_debug",
			}

		configuration { "windows", "x64", "not Debug" }
			libdirs
			{
				"Dependencies/tbb/build/macos_intel64_gcc_cc4.2.1_os10.6.8_release",
			}
	else
		print("Implement support for " .. _ACTION .. " to tbb lib dir in Helium.lua")
		os.exit(1)
	end

	configuration {}

end