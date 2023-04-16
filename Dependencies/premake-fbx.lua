local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/premake' )

Helium.RequiredFbxVersion = '2018.1.1'

Helium.GetFbxSdkLocation = function()

	local fbxLocation = thisFileLocation

	if os.host() == "windows" then
		fbxLocation = fbxLocation .. "\\fbx\\"
	else
		fbxLocation = fbxLocation .. "/fbx/"
	end

	os.mkdir( fbxLocation )

	return fbxLocation
end

Helium.DownloadFbx = function()

	print( "Downloading fbxsdk to " .. Helium.GetFbxSdkLocation() )

	if os.host() == "linux" then
		http.download( "https://www.autodesk.com/content/dam/autodesk/www/adn/fbx/20192/fbx20192_fbxsdk_linux.tar.gz", Helium.GetFbxSdkLocation() .. "fbxsdk.tar.gz" )
		os.exit( 1 )
	elseif os.host() == "macosx" then
		http.download( "https://www.autodesk.com/content/dam/autodesk/www/adn/fbx/20192/fbx20192_fbxsdk_clang_mac.pkg.tgz", Helium.GetFbxSdkLocation() .. "fbxsdk.pkg.gz" )
		os.exit( 1 )
	elseif os.host() == "windows" then
		http.download( "https://www.autodesk.com/content/dam/autodesk/www/adn/fbx/20192/fbx20192_fbxsdk_vs2017_win.exe", Helium.GetFbxSdkLocation() .. "fbxsdk.exe" )
		os.exit( 1 )
	else
		print("Implement support for " .. os.host() .. " to PublishFBX()")
		os.exit( 1 )
	end
end

Helium.PublishFbx = function( bin )

	local files = {}

	if os.host() == "linux" then
		if Helium.Build32Bit() then
			table.insert( files, { file="libfbxsdk.so", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/gcc4/x86/debug/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.so", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/gcc4/x86/release/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.so", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/gcc4/x86/release/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.so", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/gcc4/x86/release/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
		else
			table.insert( files, { file="libfbxsdk.so", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/gcc4/x64/debug/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.so", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/gcc4/x64/release/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.so", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/gcc4/x64/release/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.so", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/gcc4/x64/release/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
		end
	elseif os.host() == "macosx" then
		table.insert( files, { file="libfbxsdk.dylib", source=Helium.GetFbxSdkLocation() .. "lib/clang/debug/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
		table.insert( files, { file="libfbxsdk.dylib", source=Helium.GetFbxSdkLocation() .. "lib/clang/release/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
		table.insert( files, { file="libfbxsdk.dylib", source=Helium.GetFbxSdkLocation() .. "lib/clang/release/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
		table.insert( files, { file="libfbxsdk.dylib", source=Helium.GetFbxSdkLocation() .. "lib/clang/release/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
	elseif os.host() == "windows" then
		if Helium.Build32Bit() then
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "lib/" .. _ACTION .. "/x86/debug/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "lib/" .. _ACTION .. "/x86/release/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "lib/" .. _ACTION .. "/x86/release/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "lib/" .. _ACTION .. "/x86/release/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
		else
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "lib/" .. _ACTION .. "/x64/debug/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "lib/" .. _ACTION .. "/x64/release/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "lib/" .. _ACTION .. "/x64/release/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "lib/" .. _ACTION .. "/x64/release/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
		end
	else
		print("Implement support for " .. os.host() .. " to PublishFBX()")
		os.exit(1)
	end

	Helium.Publish( files )

end

Helium.DoFbxProjectSettings = function( bin )

	configuration {}

	includedirs
	{
		Helium.GetFbxSdkLocation() .. "/include",
	}

	configuration { "linux", "x86", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/debug",
		}
	configuration { "linux", "x86_64", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/debug",
		}

	configuration { "linux", "x86", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/release",
		}
	configuration { "linux", "x86_64", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/release",
		}

	configuration { "macosx", "x86", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/debug",
		}
	configuration { "macosx", "x86_64", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/debug",
		}

	configuration { "macosx", "x86", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/release",
		}
	configuration { "macosx", "x86_64", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/release",
		}

	configuration { "windows", "x86", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/debug",
		}
	configuration { "windows", "x86_64", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/debug",
		}

	configuration { "windows", "x86", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/release",
		}
	configuration { "windows", "x86_64", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/release",
		}

	configuration { "windows", "SharedLib or *App" }
		links
		{
			"libfbxsdk",
		}

	configuration { "not windows", "SharedLib or *App" }
		links
		{
			"fbxsdk",
		}

	configuration {}

end
