require "Helium"

Helium.PublishFBX = function( bin )

    local files = {}
	
	if os.get() == "windows" then
		files[1]  = { file="fbxsdk-2012.2d.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Debug" }
		files[2]  = { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Intermediate" }
		files[3]  = { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Profile" }
		files[4]  = { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86", target=bin .. "/x32/Release" }		
		if Helium.Build64Bit() then
			files[5]  = { file="fbxsdk-2012.2d.dll", source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Debug" }
			files[6]  = { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Intermediate" }
			files[7]  = { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Profile" }
			files[8]  = { file="fbxsdk-2012.2.dll",  source=Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64", target=bin .. "/x64/Release" }
		end
	elseif os.get() == "macosx" then
		files[1]  = { file="libfbxsdk-2012.2d.dylib", source=Helium.GetFbxSdkLocation() .. "/lib/gcc4", target=bin .. "/x32/Debug" }
		files[2]  = { file="libfbxsdk-2012.2.dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4", target=bin .. "/x32/Intermediate" }
		files[3]  = { file="libfbxsdk-2012.2.dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4", target=bin .. "/x32/Profile" }
		files[4]  = { file="libfbxsdk-2012.2.dylib",  source=Helium.GetFbxSdkLocation() .. "/lib/gcc4", target=bin .. "/x32/Release" }	
	else
		print("Implement support for " .. os.get() .. " to PublishFBX()")
		os.exit(1)
	end

	Helium.Publish( files )

end
