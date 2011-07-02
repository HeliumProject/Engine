-- Pre-build script execution.
Helium.Prebuild = function()

	local python = "python"
	
	if os.get() == "windows" then
		python = python .. ".exe"
    else
		python = python .. "3"
    end

	local pythonPath = os.pathsearch( python, os.getenv( 'PATH' ) )
	if pythonPath == nil then
		error( "\n\nYou must have Python 3.x installed and in your PATH to continue." )
	end

	local commands =
	{
		python .. " Build/JobDefParser.py JobDefinitions . .",
		python .. " Build/TypeParser.py D3D9Rendering EditorSupport Engine EngineJobs Framework FrameworkWin Graphics GraphicsJobs GraphicsTypes PcSupport PreprocessingPc Rendering TestJobs WinWindowing Windowing",
		python .. " Build/TypeParser.py -i Example -s Example -p EXAMPLE_ ExampleGame ExampleMain",
	}

	local result = 0

	for i, commandString in ipairs( commands ) do
		result = os.execute( commandString )
		if result ~= 0 then
			break
		end
	end

	if result ~= 0 then
		error( "An error occurred processing the pre-build scripts." )
	end

end