require "Dependencies/Helium"
require "Helium"

prefix = "Helium-Runtime-"
group "Runtime"

dofile "Core.lua"
dofile "Shared.lua"

Helium.DoGameMainProjectSettings( "PhysicsDemo" )
Helium.DoGameMainProjectSettings( "ShapeShooter" )
Helium.DoGameMainProjectSettings( "SideScroller" )
