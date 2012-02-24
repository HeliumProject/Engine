require "Helium"

Helium.PublishFBX = function( bin )

    local files = {}
	
	if os.get() == "windows" then
		if Helium.Build32Bit() then
			table.insert( files, { file="fbxsdk-2012.2d.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Debug" } )
			table.insert( files, { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Profile" } )
			table.insert( files, { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Release" }	)
		end
		if Helium.Build64Bit() then
			table.insert( files, { file="fbxsdk-2012.2d.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Debug" } )
			table.insert( files, { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Profile" } )
			table.insert( files, { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Release" } )
		end
	elseif os.get() == "macosx" then
		table.insert( files, { file="libfbxsdk-2012.2d.dylib", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/ub", target=bin .. "/x32/Debug" } )
		table.insert( files, { file="libfbxsdk-2012.2.dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/ub", target=bin .. "/x32/Intermediate" } )
		table.insert( files, { file="libfbxsdk-2012.2.dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/ub", target=bin .. "/x32/Profile" } )
		table.insert( files, { file="libfbxsdk-2012.2.dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/ub", target=bin .. "/x32/Release" }	)
	else
		print("Implement support for " .. os.get() .. " to PublishFBX()")
		os.exit(1)
	end

	Helium.Publish( files )

end
