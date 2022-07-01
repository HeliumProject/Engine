local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/premake' )

Helium.PublishD3dx = function( bin )

      local files = {}

      if os.host() == "windows" then
		if Helium.Build32Bit() then
            table.insert( files, { file="D3DX9_43.dll",  source="d3dx/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/bin/x86/", target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
            table.insert( files, { file="D3DX9_43.dll",  source="d3dx/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/bin/x86/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
            table.insert( files, { file="D3DX9_43.dll",  source="d3dx/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/bin/x86/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
            table.insert( files, { file="D3DX9_43.dll",  source="d3dx/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/bin/x86/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
		else
            table.insert( files, { file="D3DX9_43.dll",  source="d3dx/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/bin/x64/", target=bin .. "Debug/" .. Helium.GetBundleExecutablePath() } )
            table.insert( files, { file="D3DX9_43.dll",  source="d3dx/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/bin/x64/", target=bin .. "Intermediate/" .. Helium.GetBundleExecutablePath() } )
            table.insert( files, { file="D3DX9_43.dll",  source="d3dx/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/bin/x64/", target=bin .. "Profile/" .. Helium.GetBundleExecutablePath() } )
            table.insert( files, { file="D3DX9_43.dll",  source="d3dx/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/bin/x64/", target=bin .. "Release/" .. Helium.GetBundleExecutablePath() } )
		end
	end

	Helium.Publish( files )

end
