solution "Helium"
	configurations { "Debug", "Debug Unicode", "Release", "Release Unicode" }

	configuration "Debug"
		targetdir "Bin/Debug"
	configuration "Debug Unicode"
		targetdir "Bin/Debug Unicode"
	configuration "Release"
		targetdir "Bin/Release"
	configuration "Release Unicode"
		targetdir "Bin/Release Unicode"
    
    platforms { "Native", "x32", "x64" }
	location "Build"

	project "Platform"
		kind "SharedLib"
		language "C++"
		files { "Platform/*.h", "Platform/*.cpp" }
		
		configuration "windows"
			files{ "Platform/Windows/*.h", "Platform/Windows/*.cpp" }

		configuration "Debug*"
			defines { "_DEBUG", "DEBUG" }
			flags { "Symbols" }

		configuration "Release*"
			defines { "NDEBUG" }
			flags { "Optimize" }

		configuration "*Unicode"
			defines { "UNICODE=1" }
