require "Helium"

Helium.RequiredFbxVersion = '2012.2'
Helium.ReleaseFbxLib = "fbxsdk-" .. Helium.RequiredFbxVersion
Helium.DebugFbxLib = Helium.ReleaseFbxLib .. "d"

Helium.GetFbxSdkLocation = function()
    local fbxLocation = os.getenv( 'FBX_SDK' )
    if not fbxLocation then
        if os.get() == "windows" then
            fbxLocation = "C:\\Program Files\\Autodesk\\FBX\\FbxSdk\\" .. Helium.RequiredFbxVersion
        elseif os.get() == "macosx" then
	       	fbxLocation = "/Applications/Autodesk/FBXSDK" .. string.gsub(Helium.RequiredFbxVersion, "%.", "")
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
	
	if os.get() == "windows" then
		if Helium.Build32Bit() then
			table.insert( files, { file=Helium.DebugFbxLib .. ".dll", 	source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Debug" } )
			table.insert( files, { file=Helium.ReleaseFbxLib .. ".dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Intermediate" } )
			table.insert( files, { file=Helium.ReleaseFbxLib .. ".dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Profile" } )
			table.insert( files, { file=Helium.ReleaseFbxLib .. ".dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Release" }	)
		end
		if Helium.Build64Bit() then
			table.insert( files, { file=Helium.DebugFbxLib .. ".dll", 	source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Debug" } )
			table.insert( files, { file=Helium.ReleaseFbxLib .. ".dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Intermediate" } )
			table.insert( files, { file=Helium.ReleaseFbxLib .. ".dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Profile" } )
			table.insert( files, { file=Helium.ReleaseFbxLib .. ".dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Release" } )
		end
	elseif os.get() == "macosx" then
		table.insert( files, { file="lib" .. Helium.ReleaseFbxLib .. ".dylib",	source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/ub", target=bin .. "/x32/Debug" } )
		table.insert( files, { file="lib" .. Helium.ReleaseFbxLib .. ".dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/ub", target=bin .. "/x32/Intermediate" } )
		table.insert( files, { file="lib" .. Helium.ReleaseFbxLib .. ".dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/ub", target=bin .. "/x32/Profile" } )
		table.insert( files, { file="lib" .. Helium.ReleaseFbxLib .. ".dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4/ub", target=bin .. "/x32/Release" }	)
	else
		print("Implement support for " .. os.get() .. " to PublishFBX()")
		os.exit(1)
	end

	Helium.Publish( files )

end
