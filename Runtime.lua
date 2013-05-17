require "Helium"

print( "Temporarily defining HELIUM_TOOLS in Runtime" )
defines
{
    "HELIUM_TOOLS=1",
}

prefix = "Runtime."

dofile "Shared.lua"
