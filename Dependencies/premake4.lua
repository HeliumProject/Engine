local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/Helium' )
require( thisFileLocation .. '/fbx' )
require( thisFileLocation .. '/tbb' )
require( thisFileLocation .. '/wxWidgets' )

newoption {
   trigger     = "no-wxwidgets",
   description = "Skip building wxWidgets, use system installed version"
}

newoption {
   trigger     = "no-tbb",
   description = "Skip building tbb, use system installed version"
}

newoption {
   trigger     = "no-fbx",
   description = "Skip fbx"
}

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	-- Check prereqs
	Helium.CheckEnvironment()

	if _ACTION == "xcode3" then
        print("XCode 3 is not supported")
        os.exit(1)
	end

    if _ACTION == "vs2002" then
        print("Visual Studio 2002 is not supported")
        os.exit(1)
    end

    if _ACTION == "vs2003" then
        print("Visual Studio 2003 is not supported")
        os.exit(1)
    end

	if _ACTION == "vs2005" then
        print("Visual Studio 2005 is not supported")
        os.exit(1)
	end

	if _ACTION ~= "clean" then
	
		local bin = "../Bin"

        if not _OPTIONS["no-wxwidgets"] then
    		Helium.BuildWxWidgets()
    		Helium.PublishWxWidgets( bin )
        end

        if not _OPTIONS["no-tbb"] then
    		Helium.BuildTbb()
    		Helium.PublishTbb( bin )
        end

        if not _OPTIONS["no-fbx"] then
    		Helium.PublishFbx( bin )
        end
		
	else
	
        if not _OPTIONS["no-wxwidgets"] then
    		Helium.CleanWxWidgets()
        end

        if not _OPTIONS["no-tbb"] then
    		Helium.CleanTbb()
        end
	
	end

	solution "Dependencies"
	Helium.DoBasicSolutionSettings()
	dofile "Dependencies.lua"

end
--]]