dofile "Base.lua"
dofile "WxWidgets.lua"

local wx = "Dependencies/wxWidgets"

if _ACTION ~= "clean" then
	Helium.BuildWxWidgets( wx )
	Helium.PublishWxWidgets( wx )
else
	Helium.CleanWxWidgets( wx )
end

solution "Dependencies"
Helium.DoDefaultSolutionSettings()
dofile "Dependencies.lua"

for i, platform in ipairs( platforms() ) do
	for j, config in ipairs( configurations() ) do
		configuration( { config, platform } )
			targetdir( "Bin/" .. platform .. "/" .. config )
			objdir( "Intermediate/" .. platform .. "/" .. config )
	end
end

solution "Helium"
Helium.DoDefaultSolutionSettings()
dofile "Helium.lua"

for i, platform in ipairs( platforms() ) do
	for j, config in ipairs( configurations() ) do
		configuration( { config, platform } )
			targetdir( "Bin/" .. platform .. "/" .. config )
			objdir( "Intermediate/" .. platform .. "/" .. config )
	end
end