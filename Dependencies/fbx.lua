local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/Helium' )

Helium.RequiredFbxVersion = '2016.1.2'

Helium.GetFbxSdkLocation = function()

	local fbxLocation = os.getenv( 'FBX_SDK' )

	if not fbxLocation then
		if os.get() == "windows" then
			fbxLocation = "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\" .. Helium.RequiredFbxVersion
		elseif os.get() == "macosx" then
			fbxLocation = "/Applications/Autodesk/FBX SDK/" .. Helium.RequiredFbxVersion
		elseif os.get() == "linux" then
			fbxLocation = "Dependencies/fbx/" .. string.gsub(Helium.RequiredFbxVersion, "%.", "")
		else
			print("Implement support for " .. os.get() .. " to Helium.GetFbxSdkLocation()")
			return nil
		end
	end

	if os.get() == "windows" then
		fbxLocation = fbxLocation .. "\\"
	else
		fbxLocation = fbxLocation .. "/"
	end

	return fbxLocation
end

Helium.PublishFbx = function( bin )

	local files = {}

	if os.get() == "linux" then
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
	elseif os.get() == "macosx" then
		table.insert( files, { file="libfbxsdk.dylib", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/clang/debug/",   target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
		table.insert( files, { file="libfbxsdk.dylib", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/clang/release/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
		table.insert( files, { file="libfbxsdk.dylib", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/clang/release/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
		table.insert( files, { file="libfbxsdk.dylib", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "lib/clang/release/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
	elseif os.get() == "windows" then
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
		print("Implement support for " .. os.get() .. " to PublishFBX()")
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

	configuration { "linux", "x32", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/debug",
		}
	configuration { "linux", "x64", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/debug",
		}

	configuration { "linux", "x32", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/release",
		}
	configuration { "linux", "x64", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/release",
		}

	configuration { "macosx", "x32", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/debug",
		}
	configuration { "macosx", "x64", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/debug",
		}

	configuration { "macosx", "x32", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/release",
		}
	configuration { "macosx", "x64", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/release",
		}

	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/debug",
		}
	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/debug",
		}

	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/release",
		}
	configuration { "windows", "x64", "not Debug" }
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
