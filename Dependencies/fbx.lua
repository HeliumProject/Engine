local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/Helium' )

Helium.RequiredFbxVersion = '2014.1'

Helium.GetFbxSdkLocation = function()
	local fbxLocation = os.getenv( 'FBX_SDK' )
	if not fbxLocation then
		if os.get() == "windows" then
			fbxLocation = "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\" .. Helium.RequiredFbxVersion
		elseif os.get() == "macosx" then
			fbxLocation = "/Applications/Autodesk/FBX SDK/" .. Helium.RequiredFbxVersion
		elseif os.get() == "linux" then
			fbxLocation = "/opt/fbx/" .. string.gsub(Helium.RequiredFbxVersion, "%.", "")
		else
			print("Implement support for " .. os.get() .. " to Helium.GetFbxSdkLocation()")
			os.exit(1)
		end
		if not os.isdir( fbxLocation ) then
			print("FBX SDK not found at: " .. fbxLocation)
			os.exit(1)
		end
	end

	return fbxLocation
end

Helium.PublishFbx = function( bin )

	local files = {}

	print( string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") )

	if os.get() == "windows" then
		if Helium.Build32Bit() then
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/debug",   target=bin .. "/x32/Debug" } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/release", target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/release", target=bin .. "/x32/Profile" } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86/release", target=bin .. "/x32/Release" }	)
		end
		if Helium.Build64Bit() then
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/debug",   target=bin .. "/x64/Debug" } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/release", target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/release", target=bin .. "/x64/Profile" } )
			table.insert( files, { file="libfbxsdk.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64/release", target=bin .. "/x64/Release" } )
		end
	elseif os.get() == "macosx" then
		table.insert( files, { file="libfbxsdk.dylib", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "/lib/gcc4/ub/debug", 	target=bin .. "/x32/Debug" } )
		table.insert( files, { file="libfbxsdk.dylib", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "/lib/gcc4/ub/release", target=bin .. "/x32/Intermediate" } )
		table.insert( files, { file="libfbxsdk.dylib", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "/lib/gcc4/ub/release", target=bin .. "/x32/Profile" } )
		table.insert( files, { file="libfbxsdk.dylib", source=string.gsub(Helium.GetFbxSdkLocation(), " ", "\\ ") .. "/lib/gcc4/ub/release", target=bin .. "/x32/Release" }	)
	elseif os.get() == "linux" then
		if Helium.Build32Bit() then
			table.insert( files, { file="libfbxsdk.so", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/debug", 	target=bin .. "/x32/Debug" } )
			table.insert( files, { file="libfbxsdk.so", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/release", 	target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="libfbxsdk.so", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/release", 	target=bin .. "/x32/Profile" } )
			table.insert( files, { file="libfbxsdk.so", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/x86/release", 	target=bin .. "/x32/Release" }	)
		end
		if Helium.Build64Bit() then
			table.insert( files, { file="libfbxsdk.so", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/debug",		target=bin .. "/x32/Debug" } )
			table.insert( files, { file="libfbxsdk.so", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/release", 	target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="libfbxsdk.so", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/release", 	target=bin .. "/x32/Profile" } )
			table.insert( files, { file="libfbxsdk.so", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/x64/release", 	target=bin .. "/x32/Release" }	)
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

	configuration { "windows", "Debug" }
		links
		{
			"libfbxsdk",
		}

	configuration {}

end