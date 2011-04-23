require "Base"

Helium.PublishFBX = function( bin )

    local files = {}
	
	if os.get() == "windows" then
		files[1]  = { file="fbxsdk_20113_1.dll",       source=Helium.GetFbxSdkLocation() .. "/lib/vs2008", target=bin .. "/x32/Debug" }
		files[2]  = { file="fbxsdk_20113_1.dll",       source=Helium.GetFbxSdkLocation() .. "/lib/vs2008", target=bin .. "/x32/Intermediate" }
		files[3]  = { file="fbxsdk_20113_1.dll",       source=Helium.GetFbxSdkLocation() .. "/lib/vs2008", target=bin .. "/x32/Profile" }
		files[4]  = { file="fbxsdk_20113_1.dll",       source=Helium.GetFbxSdkLocation() .. "/lib/vs2008", target=bin .. "/x32/Release" }
		if Helium.Build64Bit() then
			files[5]  = { file="fbxsdk_20113_1_amd64.dll", source=Helium.GetFbxSdkLocation() .. "/lib/vs2008", target=bin .. "/x64/Debug" }
			files[6]  = { file="fbxsdk_20113_1_amd64.dll", source=Helium.GetFbxSdkLocation() .. "/lib/vs2008", target=bin .. "/x64/Intermediate" }
			files[7]  = { file="fbxsdk_20113_1_amd64.dll", source=Helium.GetFbxSdkLocation() .. "/lib/vs2008", target=bin .. "/x64/Profile" }
			files[8]  = { file="fbxsdk_20113_1_amd64.dll", source=Helium.GetFbxSdkLocation() .. "/lib/vs2008", target=bin .. "/x64/Release" }
		end
	elseif os.get() == "macosx" then
		files[1]  = { file="libfbxsdk_20113_1d.dylib",      source=Helium.GetFbxSdkLocation() .. "/lib/gcc4", target=bin .. "/x32/Debug" }
		files[2]  = { file="libfbxsdk_20113_1.dylib",       source=Helium.GetFbxSdkLocation() .. "/lib/gcc4", target=bin .. "/x32/Intermediate" }
		files[3]  = { file="libfbxsdk_20113_1.dylib",       source=Helium.GetFbxSdkLocation() .. "/lib/gcc4", target=bin .. "/x32/Profile" }
		files[4]  = { file="libfbxsdk_20113_1.dylib",       source=Helium.GetFbxSdkLocation() .. "/lib/gcc4", target=bin .. "/x32/Release" }	
	else
		print("Implement support for " .. os.get() .. " to PublishFBX()")
		os.exit(1)
	end

	Helium.Publish( files )

end
