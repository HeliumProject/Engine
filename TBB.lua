require "Helium"

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
	bat.write( bat, "@set PATH=%PATH%;" .. FlipSlashes( os.getcwd() ) .. "\\..\\..\\Utilities\\Win32\n" )
	bat.write( bat, "@make.exe " .. cmd .. "\n" )
	io.close( bat )

	local result = os.execute( "cmd.exe /c \"call \"" .. path .. "\"" )
	--os.execute( "cmd.exe /c \"del " .. path .. "\"" )
	return result
end

Helium.BuildTBB = function()

	local cwd = os.getcwd()

	if os.get() == "windows" then

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
		
		os.chdir( "Dependencies/tbb" )

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
		os.chdir( "Dependencies/tbb" )

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

end

Helium.CleanTBB = function()

	local cwd = os.getcwd()

	local files = {}
	
	if os.get() == "windows" then

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
		
		os.chdir( "Dependencies/tbb" )
		
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
		os.chdir( "Dependencies/tbb" )

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

Helium.PublishTBB = function( bin )

	local files = {}
	
	if os.get() == "windows" then
		if _ACTION == "vs2008" then
			if Helium.Build32Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="Dependencies/tbb/build/windows_ia32_cl_vc9_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="Dependencies/tbb/build/windows_ia32_cl_vc9_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Release" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc9_release",        target=bin .. "/x32/Release" } )
			end
			if Helium.Build64Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="Dependencies/tbb/build/windows_intel64_cl_vc9_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="Dependencies/tbb/build/windows_intel64_cl_vc9_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Release" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc9_release",	    target=bin .. "/x64/Release" } )
			end       
		elseif _ACTION == "vs2010" then 
			if Helium.Build32Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="Dependencies/tbb/build/windows_ia32_cl_vc10_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="Dependencies/tbb/build/windows_ia32_cl_vc10_debug",		    target=bin .. "/x32/Debug" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Profile" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Release" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_ia32_cl_vc10_release",	    target=bin .. "/x32/Release" } )
			end
			if Helium.Build64Bit() then
				table.insert( files, { file="tbb_debug.dll",	source="Dependencies/tbb/build/windows_intel64_cl_vc10_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb_debug.pdb",	source="Dependencies/tbb/build/windows_intel64_cl_vc10_debug",	    target=bin .. "/x64/Debug" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Intermediate" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Profile" } )
				table.insert( files, { file="tbb.dll",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Release" } )
				table.insert( files, { file="tbb.pdb",			source="Dependencies/tbb/build/windows_intel64_cl_vc10_release",	target=bin .. "/x64/Release" } )
			end
		end
	elseif os.get() == "macosx" then
		if Helium.Build32Bit() then
			table.insert( files, { file="libtbb_debug.dylib",	source="Dependencies/tbb/build/macos_ia32_gcc_cc4.2.1_os10.6.7_debug",		    target=bin .. "/x32/Debug" } )
			table.insert( files, { file="libtbb.dylib",			source="Dependencies/tbb/build/macos_ia32_gcc_cc4.2.1_os10.6.7_release",        target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="libtbb.dylib",			source="Dependencies/tbb/build/macos_ia32_gcc_cc4.2.1_os10.6.7_release",        target=bin .. "/x32/Profile" } )
			table.insert( files, { file="libtbb.dylib",			source="Dependencies/tbb/build/macos_ia32_gcc_cc4.2.1_os10.6.7_release",        target=bin .. "/x32/Release" } )
		end
		if Helium.Build64Bit() then		
			table.insert( files, { file="libtbb_debug.dylib",	source="Dependencies/tbb/build/macos_intel64_gcc_cc4.2.1_os10.6.7_debug",	    target=bin .. "/x64/Debug" } )
			table.insert( files, { file="libtbb.dylib",			source="Dependencies/tbb/build/macos_intel64_gcc_cc4.2.1_os10.6.7_release",	    target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file="libtbb.dylib",			source="Dependencies/tbb/build/macos_intel64_gcc_cc4.2.1_os10.6.7_release",	    target=bin .. "/x64/Profile" } )
			table.insert( files, { file="libtbb.dylib",			source="Dependencies/tbb/build/macos_intel64_gcc_cc4.2.1_os10.6.7_release",	    target=bin .. "/x64/Release" } )
		end       
	else
		print("Implement support for " .. os.get() .. " to CleanWxWidgets()")
		os.exit(1)
	end

	Helium.Publish( files )

end