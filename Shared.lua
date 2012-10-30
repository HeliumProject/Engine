require "Helium"

configuration {}

defines
{
	"WXUSINGDLL=1",
	"wxNO_EXPAT_LIB=1",
	"wxNO_JPEG_LIB=1",
	"wxNO_PNG_LIB=1",
	"wxNO_TIFF_LIB=1",
	"wxNO_ZLIB_LIB=1",
}

includedirs
{
	".",
	"Dependencies/boost-preprocessor/include",
	"Dependencies/expat/lib",
	"Dependencies/freetype/include",
	"Dependencies/nvtt",
	"Dependencies/nvtt/src",
	"Dependencies/nvtt/src/nvtt/squish",
	"Dependencies/p4api/include",
	"Dependencies/png",
	"Dependencies/tbb/include",
	"Dependencies/wxWidgets/include",
	"Dependencies/zlib",

	Helium.GetFbxSdkLocation() .. "/include",
}

configuration "windows"
	includedirs
	{
		"Dependencies/nvtt/project/vc8",
		"Dependencies/wxWidgets/include/msvc",
	}
	
configuration { "windows", "x32" }
	libdirs
	{
		Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x86",
	}
configuration { "windows", "x64" }
	libdirs
	{
		Helium.GetFbxSdkLocation() .. "/lib/" .. _ACTION .. "/x64",
	}

configuration {}

if haveGranny then
	includedirs
	{
		"Integrations/Granny",
		"Integrations/Granny/granny_sdk/include",
	}

	defines
	{
		"HELIUM_HAVE_GRANNY=1",
	}
else
	defines
	{
		"HELIUM_HAVE_GRANNY=0",
	}
end

--[[
We build monolithic wx, so ignore all the legacy non-monolithic
#pragma comment directives (on windows only)
--]]
if os.get() == "windows" then

	configuration "windows"
		linkoptions
		{
			"/NODEFAULTLIB:wxbase29ud",
			"/NODEFAULTLIB:wxbase29d",
			"/NODEFAULTLIB:wxbase29u",
			"/NODEFAULTLIB:wxbase29",
			"/NODEFAULTLIB:wxbase29ud_net",
			"/NODEFAULTLIB:wxbase29d_net",
			"/NODEFAULTLIB:wxbase29u_net",
			"/NODEFAULTLIB:wxbase29_net",
			"/NODEFAULTLIB:wxbase29ud_xml",
			"/NODEFAULTLIB:wxbase29d_xml",
			"/NODEFAULTLIB:wxbase29u_xml",
			"/NODEFAULTLIB:wxbase29_xml",
			"/NODEFAULTLIB:wxmsw29ud_core",
			"/NODEFAULTLIB:wxmsw29d_core",
			"/NODEFAULTLIB:wxmsw29u_core",
			"/NODEFAULTLIB:wxmsw29_core",
			"/NODEFAULTLIB:wxmsw29ud_adv",
			"/NODEFAULTLIB:wxmsw29d_adv",
			"/NODEFAULTLIB:wxmsw29u_adv",
			"/NODEFAULTLIB:wxmsw29_adv",
			"/NODEFAULTLIB:wxmsw29ud_html",
			"/NODEFAULTLIB:wxmsw29d_html",
			"/NODEFAULTLIB:wxmsw29u_html",
			"/NODEFAULTLIB:wxmsw29_html",
			"/NODEFAULTLIB:wxmsw29ud_qa",
			"/NODEFAULTLIB:wxmsw29d_qa",
			"/NODEFAULTLIB:wxmsw29u_qa",
			"/NODEFAULTLIB:wxmsw29_qa",
			"/NODEFAULTLIB:wxmsw29ud_xrc",
			"/NODEFAULTLIB:wxmsw29d_xrc",
			"/NODEFAULTLIB:wxmsw29u_xrc",
			"/NODEFAULTLIB:wxmsw29_xrc",
			"/NODEFAULTLIB:wxmsw29ud_aui",
			"/NODEFAULTLIB:wxmsw29d_aui",
			"/NODEFAULTLIB:wxmsw29u_aui",
			"/NODEFAULTLIB:wxmsw29_aui",
			"/NODEFAULTLIB:wxmsw29ud_propgrid",
			"/NODEFAULTLIB:wxmsw29d_propgrid",
			"/NODEFAULTLIB:wxmsw29u_propgrid",
			"/NODEFAULTLIB:wxmsw29_propgrid",
			"/NODEFAULTLIB:wxmsw29ud_ribbon",
			"/NODEFAULTLIB:wxmsw29d_ribbon",
			"/NODEFAULTLIB:wxmsw29u_ribbon",
			"/NODEFAULTLIB:wxmsw29_ribbon",
			"/NODEFAULTLIB:wxmsw29ud_richtext",
			"/NODEFAULTLIB:wxmsw29d_richtext",
			"/NODEFAULTLIB:wxmsw29u_richtext",
			"/NODEFAULTLIB:wxmsw29_richtext",
			"/NODEFAULTLIB:wxmsw29ud_media",
			"/NODEFAULTLIB:wxmsw29d_media",
			"/NODEFAULTLIB:wxmsw29u_media",
			"/NODEFAULTLIB:wxmsw29_media",
			"/NODEFAULTLIB:wxmsw29ud_stc",
			"/NODEFAULTLIB:wxmsw29d_stc",
			"/NODEFAULTLIB:wxmsw29u_stc",
			"/NODEFAULTLIB:wxmsw29_stc",
			"/NODEFAULTLIB:wxmsw29ud_gl",
			"/NODEFAULTLIB:wxmsw29d_gl",
			"/NODEFAULTLIB:wxmsw29u_gl",
			"/NODEFAULTLIB:wxmsw29_gl",
		}
		if _ACTION == "vs2010" or _ACTION == "vs2008" then
			includedirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Include"
			}
		end

	configuration { "windows", "x32" }
		if _ACTION == "vs2010" or _ACTION == "vs2008" then
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Lib/x86",
			}
		end

		if haveGranny then
			libdirs
			{
				"Integrations/Granny/granny_sdk/lib/win32",
			}
		end

	configuration { "windows", "x64" }
		if _ACTION == "vs2010" or _ACTION == "vs2008" then
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Lib/x64",
			}
		end
		
		if haveGranny then
			libdirs
			{
				"Integrations/Granny/granny_sdk/lib/win64",
			}
		end
end

if _ACTION == "vs2008" then
	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_ia32_cl_vc9_debug",
		}

	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_ia32_cl_vc9_release",
		}

	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_intel64_cl_vc9_debug",
		}

	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_intel64_cl_vc9_release",
		}
elseif _ACTION == "vs2010" then
	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_ia32_cl_vc10_debug",
		}

	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_ia32_cl_vc10_release",
		}

	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_intel64_cl_vc10_debug",
		}

	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_intel64_cl_vc10_release",
		}
elseif _ACTION == "vs2012" then
	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_ia32_cl_vc11_debug",
		}

	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_ia32_cl_vc11_release",
		}

	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_intel64_cl_vc11_debug",
		}

	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_intel64_cl_vc11_release",
		}
elseif _ACTION == "xcode3" or _ACTION == "xcode4" then
	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/macos_ia32_gcc_cc4.2.1_os10.6.8_debug",
		}

	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/macos_ia32_gcc_cc4.2.1_os10.6.8_release",
		}

	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/macos_intel64_gcc_cc4.2.1_os10.6.8_debug",
		}

	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/macos_intel64_gcc_cc4.2.1_os10.6.8_release",
		}
else
	print("Implement support for " .. _ACTION .. " to tbb lib dir in Helium.lua")
	os.exit(1)
end

local prefix = solution().name .. '.'

project( prefix .. "Platform" )
	uuid "E4A1F8FC-A93A-46E2-9CA8-40C2CE1B163E"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Platform", "PLATFORM" )

	files
	{
		"Platform/*.cpp",
		"Platform/*.h",
		"Platform/*.inl",
	}

	configuration "windows"
		excludes
		{
			"Platform/*Posix.*",
			"Platform/*Mac.*",
			"Platform/*Lin.*",
		}

	configuration "macosx"
		excludes
		{
			"Platform/*Win.*",
			"Platform/*Lin.*",
		}

	configuration "linux"
		excludes
		{
			"Platform/*Win.*",
			"Platform/*Mac.*",
		}

project( prefix .. "Foundation" )
	uuid "9708463D-9698-4BB6-A911-37354AF0E21E"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Foundation", "FOUNDATION" )

	files
	{
		"Foundation/**",
	}

    pchheader( "FoundationPch.h" )
    pchsource( "Foundation/FoundationPch.cpp" )

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			"expat",
		}

project( prefix .. "Buffers" )
	uuid "F8A00DD4-2BAF-4409-A713-366AA79386AB"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Buffers", "BUFFERS" )

	files
	{
		"Buffers/**",
	}

    pchheader( "BuffersPch.h" )
    pchsource( "Buffers/BuffersPch.cpp" )

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Math",
		}

project( prefix .. "Reflect" )
	uuid "6488751F-220A-4E88-BA5B-A1BE5E3124EC"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Reflect", "REFLECT" )

	files
	{
		"Reflect/**",
	}

    pchheader( "ReflectPch.h" )
    pchsource( "Reflect/ReflectPch.cpp" )

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			"zlib",
		}

project( prefix .. "Math" )
	uuid "8F42DBD6-75E3-4A16-A3B6-77381600009D"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Math", "MATH" )

	files
	{
		"Math/**",
	}

    pchheader( "MathPch.h" )
    pchsource( "Math/MathPch.cpp" )

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
		}

project( prefix .. "Engine" )
	uuid "CDD089F1-EC6E-469B-BF06-8DF56C5B1489"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Engine", "ENGINE" )

	files
	{
		"Engine/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
		}

project( prefix .. "EngineJobs" )
	uuid "65CFFE89-3111-4D58-95DC-5DB6D3F28935"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "EngineJobs", "ENGINE_JOBS" )

	files
	{
		"EngineJobs/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
		}

project( prefix .. "Windowing" )
	uuid "B68268DF-3942-432F-89B1-DBC82C21218E"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Windowing", "WINDOWING" )

	files
	{
		"Windowing/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
		}

project( prefix .. "Rendering" )
	uuid "3F1BD209-272C-4833-AF8E-35C317F21452"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Rendering", "RENDERING" )

	files
	{
		"Rendering/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
		}

project( prefix .. "GraphicsTypes" )
	uuid "4A13A4F6-6860-4F52-A217-B0C3943E7025"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "GraphicsTypes", "GRAPHICS_TYPES" )

	files
	{
		"GraphicsTypes/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
		}

project( prefix .. "GraphicsJobs" )
	uuid "4D83346D-DCB2-40E6-AAF1-508341728E57"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "GraphicsJobs", "GRAPHICS_JOBS" )

	files
	{
		"GraphicsJobs/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
		}

project( prefix .. "Graphics" )
	uuid "3342921C-F6C7-4A81-A6FF-1C93373AF285"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Graphics", "GRAPHICS" )

	files
	{
		"Graphics/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
		}

project( prefix .. "Framework" )
	uuid "6DB6B383-76E6-4361-8CFE-F08F1CFE24BE"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Framework", "FRAMEWORK" )

	files
	{
		"Framework/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
		}

project( prefix .. "WindowingWin" )
	uuid "1D7B65F8-6A31-4E8C-AF91-C1D2FA73AD12"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "WindowingWin", "WINDOWING_WIN" )

	files
	{
		"WindowingWin/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
		}

project( prefix .. "RenderingD3D9" )
	uuid "4BE28ED4-950D-469B-A6F8-88C09BA479E5"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "RenderingD3D9", "RENDERING_D3D9" )

	files
	{
		"RenderingD3D9/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
		}

project( prefix .. "PcSupport" )
	uuid "2B3B921A-BFF1-4A73-A9DD-3FCACA9D2916"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "PcSupport", "PC_SUPPORT" )

	files
	{
		"PcSupport/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
		}

project( prefix .. "PreprocessingPc" )
	uuid "94E6A151-FC28-41EE-A5F3-D8629F6B8B3B"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "PreprocessingPc", "PREPROCESSING_PC" )

	files
	{
		"PreprocessingPc/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "PcSupport",
		}

project( prefix .. "EditorSupport" )
	uuid "82F12FF0-CA4E-42E5-84A7-92A5C1A8AE26"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "EditorSupport", "EDITOR_SUPPORT" )

	files
	{
		"EditorSupport/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",
		}

project( prefix .. "FrameworkWin" )
	uuid "8F1B5E58-BDA5-447D-9FD4-36A3B23221B8"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "FrameworkWin", "FRAMEWORK_WIN" )

	files
	{
		"FrameworkWin/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "WindowingWin",
			prefix .. "RenderingD3D9",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",
			prefix .. "EditorSupport",
		}
		
project( prefix .. "TestJobs" )-- DEPRECATED
	uuid "12106586-0EB1-4D4C-9DFE-E3C63D3E4013"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "TestJobs", "TEST_JOBS" )
	
	files
	{
		"TestJobs/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "Engine",
			prefix .. "EngineJobs",
		}

project( prefix .. "TestApp" )-- DEPRECATED
	uuid "CB5427DC-CE08-4FA6-B060-F35A902806BA"

	kind "WindowedApp"

	files
	{
		"TestApp/**.cpp",
		"TestApp/**.h",
		"TestApp/**.ico",
		"TestApp/**.rc"
	}

	flags
	{
		"WinMain",
	}

	links
	{
		prefix .. "Platform",
		prefix .. "Foundation",
		prefix .. "Reflect",
		prefix .. "Math",
		prefix .. "Engine",
		prefix .. "EngineJobs",
		prefix .. "Windowing",
		prefix .. "Rendering",
		prefix .. "GraphicsTypes",
		prefix .. "GraphicsJobs",
		prefix .. "Graphics",
		prefix .. "Framework",
		prefix .. "WindowingWin",
		prefix .. "RenderingD3D9",
		prefix .. "PcSupport",
		prefix .. "PreprocessingPc",
		prefix .. "EditorSupport",
		prefix .. "FrameworkWin",
		prefix .. "TestJobs",
	}

	pchheader( "TestAppPch.h" )
	pchsource( "TestApp/TestAppPch.cpp" )

	Helium.DoDefaultProjectSettings()

	-- TestApp is a bit odd because it includes custom game objects and a main().
	-- So we need the dll export #defines. But calling DoModuleProjectSettings(...) above
	-- seems to blow away the libs we try to import when we call DoDefaultProjectSettings()
	configuration { "windows", "Debug" }
	defines
	{
		"HELIUM_TEST_APP_EXPORTS",
	}

	configuration "windows"
		links
		{
			"d3d9",
			"d3d11",
			"wininet",
			"ws2_32",
			"dbghelp",
		}

	configuration { "windows", "Debug" }
		links
		{
			Helium.DebugFbxLib,
		}
	configuration { "windows", "not Debug" }
		links
		{
			Helium.ReleaseFbxLib,
		}
		
	if haveGranny then
		configuration "x32"
			links
			{
				"granny2",
			}
		configuration "x64"
			links
			{
				"granny2_x64",
			}
	end
