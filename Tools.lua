require "Dependencies/Helium"
require "Helium"

prefix = "Helium-Tools-"
group "Tools"

dofile "Core.lua"
dofile "Shared.lua"

project( prefix .. "PcSupport" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "PcSupport", "PC_SUPPORT" )
	Helium.DoTbbProjectSettings()

	files
	{
		"PcSupport/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "PreprocessingPc" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "PreprocessingPc", "PREPROCESSING_PC" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"PreprocessingPc/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "PcSupport",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "EditorSupport" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "EditorSupport", "EDITOR_SUPPORT" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoFbxProjectSettings()

	files
	{
		"EditorSupport/*",
	}

	includedirs
	{
		"Dependencies/nvtt",
		"Dependencies/nvtt/extern/poshlib",
		"Dependencies/nvtt/src",
		"Dependencies/nvtt/src/nvtt/squish",
		"Dependencies/freetype/include",
		"Dependencies/libpng",
	}

	configuration "SharedLib"
		links
		{
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

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",

			"freetype",
			"libpng",
			"nvtt",
			"zlib",
			"mongo-c",
		}

project( prefix .. "SceneGraph" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "SceneGraph", "SCENE_GRAPH" )

	files
	{
		"SceneGraph/*",
	}

	configuration "SharedLib"
		links
		{
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
			prefix .. "EditorSupport",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Application",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Inspect",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

project( prefix .. "Editor" )

	kind "ConsoleApp"

	Helium.DoTbbProjectSettings()

	files
	{
		"Editor/**.h",
		"Editor/**.c",
		"Editor/**.cpp",
	}

	flags
	{
		"WinMain"
	}

	defines
	{
		"HELIUM_MODULE=Editor",
		"WXUSINGDLL=1",
		"wxNO_EXPAT_LIB=1",
		"wxNO_JPEG_LIB=1",
		"wxNO_PNG_LIB=1",
		"wxNO_TIFF_LIB=1",
		"wxNO_ZLIB_LIB=1",
	}

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/p4api/include",
		"Dependencies/wxWidgets/include",
		"Dependencies/bullet/src",
		"Example",
	}

	if os.get() == "windows" then
		pchheader( "EditorPch.h" )
		pchsource( "Editor/EditorPch.cpp" )
	else
		includedirs
		{
			"Editor",
		}
	end

	Helium.DoBasicProjectSettings()
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()
	Helium.DoFbxProjectSettings()

	links
	{
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
		prefix .. "EditorSupport",
		prefix .. "SceneGraph",
		prefix .. "Components",
		prefix .. "Bullet",
		prefix .. "ExampleGame",

		-- core
		prefix .. "Platform",
		prefix .. "Foundation",
		prefix .. "Application",
		prefix .. "Reflect",
		prefix .. "Persist",
		prefix .. "Math",
		prefix .. "MathSimd",
		prefix .. "Inspect",

		"bullet",
		"mongo-c",
		"ois",
	}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	-- We build monolithic wx, so ignore all the legacy non-monolithic #pragma comment directives (on windows only)

	configuration "windows"
		files
		{
			"Editor/Editor.rc",
		}
		includedirs
		{
			"Dependencies/wxWidgets/include/msvc",
		}
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

	-- per architecture
	configuration { "windows", "x32" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_dll",
		}
	configuration { "windows", "x64" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_x64_dll",
		}

	-- per configuration
	configuration { "windows", "Debug" }
		links
		{
			"wxmsw29ud"
		}
	configuration { "windows", "not Debug" }
		links
		{
			"wxmsw29u"
		}

	-- per architecture, per configuration
	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/Win32/Debug",
		}
	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/Win32/Release",
		}
	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/x64/Debug",
		}
	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/x64/Release",
		}

	configuration "windows"
		links
		{
			"libclient",
			"librpc",
			"libsupp",
			"libp4sslstub",
		}

	configuration "macosx"
		defines
		{
			"_FILE_OFFSET_BITS=64",
			"__WXMAC__",
			"__WXOSX__",
			"__WXOSX_COCOA__",
			"HAVE_TYPE_TRAITS",
		}

	configuration { "macosx", "x32", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-32/lib/wx/include/osx_cocoa-unicode-2.9",
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-32/lib",
		}

	configuration { "macosx", "x32", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-32/lib/wx/include/osx_cocoa-unicode-2.9",
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-32/lib",
		}

	configuration { "macosx", "x64", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-64/lib/wx/include/osx_cocoa-unicode-2.9",
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-64/lib",
		}

	configuration { "macosx", "x64", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-64/lib/wx/include/osx_cocoa-unicode-2.9",
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-64/lib",
		}

	configuration { "macosx", "x32" }
		libdirs
		{
			"Dependencies/p4api/lib/macosx/x32",
		}

	configuration { "macosx", "x64" }
		libdirs
		{
			"Dependencies/p4api/lib/macosx/x64",
		}

	configuration "macosx"
		links
		{
			"client",
			"rpc",
			"supp",
			"p4sslstub",
			"wx_osx_cocoau_gl-2.9.dylib",
			"wx_osx_cocoau-2.9.dylib",
		}
		linkoptions
		{
			"-framework Foundation",
			"-framework IOKit",
			"-framework Carbon",
			"-framework Cocoa",
			"-framework AudioToolbox",
			"-framework System",
			"-framework OpenGL",
		}

	configuration "linux"
		defines
		{
			"_FILE_OFFSET_BITS=64",
			"__WXGTK__",
			"HAVE_TYPE_TRAITS",
		}

	configuration { "linux", "x32", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-32/lib/wx/include/gtk2-unicode-2.9",
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-32/lib",
		}

	configuration { "linux", "x32", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-32/lib/wx/include/gtk2-unicode-2.9",
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-32/lib",
		}

	configuration { "linux", "x64", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-64/lib/wx/include/gtk2-unicode-2.9",
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-64/lib",
		}

	configuration { "linux", "x64", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-64/lib/wx/include/gtk2-unicode-2.9",
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-64/lib",
		}

	configuration { "linux", "x32" }
		libdirs
		{
			"Dependencies/p4api/lib/linux/x32",
		}

	configuration { "linux", "x64" }
		libdirs
		{
			"Dependencies/p4api/lib/linux/x64",
		}

	configuration "linux"
		links
		{
			"client",
			"rpc",
			"supp",
			"p4sslstub",
			"wx_gtk2u_gl-2.9.dylib",
			"wx_gtk2u-2.9.dylib",
		}
