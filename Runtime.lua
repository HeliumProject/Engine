require "Dependencies/Helium"
require "Helium"

prefix = "Helium-Runtime-"

if Helium.GetPremakeVersion() > 4 then
	group "Runtime"
end

dofile "Core.lua"
dofile "Shared.lua"
