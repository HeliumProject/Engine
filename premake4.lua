dofile "Base.lua"

solution "Dependencies"
Helium.DoDefaultSolutionSettings()
dofile "Dependencies.lua"

solution "Helium"
Helium.DoDefaultSolutionSettings()
dofile "Helium.lua"
