require "Dependencies/Helium"
require "Helium"

prefix = "Helium-Runtime-"
group "Runtime"

dofile "Core.lua"

if _OPTIONS[ "core" ] then
	return
end

dofile "Shared.lua"
