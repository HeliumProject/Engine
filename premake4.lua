dofile "Base.lua"

solution "Dependencies"
Helium.DoDefaultSolutionSettings()
dofile "Dependencies.lua"

if _ACTION ~= "clean" then
	Helium.BuildWxWidgets( "." )
else
	Helium.CleanWxWidgets( "." )
end

solution "Helium"
Helium.DoDefaultSolutionSettings()
dofile "Helium.lua"
