require "Dependencies/premake"
require "premake"

tools = false
prefix = "Helium-Runtime-"
group "Runtime"

dofile "premake-core.lua"

if _OPTIONS[ "core" ] then
	return
end

dofile "premake-shared.lua"
