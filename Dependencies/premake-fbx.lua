local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/premake' )

Helium.RequiredFbxVersion = '2020.2'

Helium.GetFbxSdkLocation = function()

	local fbxLocation = os.getenv( 'FBX_SDK' )

	if not fbxLocation then
		if os.host() == "macosx" then
			fbxLocation = "/Applications/Autodesk/FBX SDK/" .. Helium.RequiredFbxVersion
		else
			fbxLocation = thisFileLocation .."/fbx/"
		end
	end

	if os.host() == "windows" then
		fbxLocation = fbxLocation .. "\\"
	else
		fbxLocation = fbxLocation .. "/"
	end

	return fbxLocation
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

	filter {}

	includedirs
	{
		Helium.GetFbxSdkLocation() .. "/include",
	}

	filter { "system:linux", "architecture:x86", "configurations:Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/debug",
		}
	filter { "system:linux", "architecture:x86_64", "configurations:Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/debug",
		}

	filter { "system:linux", "architecture:x86", "configurations:not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/release",
		}
	filter { "system:linux", "architecture:x86_64", "configurations:not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/release",
		}

	filter { "system:macosx", "architecture:x86", "configurations:Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/debug",
		}
	filter { "system:macosx", "architecture:x86_64", "configurations:Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/debug",
		}

	filter { "system:macosx", "architecture:x86", "configurations:not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/release",
		}
	filter { "system:macosx", "architecture:x86_64", "configurations:not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/clang/release",
		}

	filter { "system:windows", "architecture:x86", "configurations:Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/debug",
		}
	filter { "system:windows", "architecture:x86_64", "configurations:Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/debug",
		}

	filter { "system:windows", "architecture:x86", "configurations:not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/release",
		}
	filter { "system:windows", "architecture:x86_64", "configurations:not Debug" }
		libdirs
		{
			Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/release",
		}

	filter { "system:windows", "kind:SharedLib or *App" }
		links
		{
			"libfbxsdk",
		}

	filter { "system:not windows", "kind:SharedLib or *App" }
		links
		{
			"fbxsdk",
		}

	filter {}

end
