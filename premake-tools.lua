require "Dependencies/premake"
require "Dependencies/premake-fbx"
require "Dependencies/premake-wx"
require "premake"

tools = true
prefix = "Helium-Tools-"
group "Tools"

dofile "premake-core.lua"

if _OPTIONS[ "core" ] then
	return
end

dofile "premake-shared.lua"

project( prefix .. "PcSupport" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "PcSupport", "PC_SUPPORT" )

	files
	{
		"Source/Engine/PcSupport/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "PreprocessingPc", "PREPROCESSING_PC" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/PreprocessingPc/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "EditorSupport", "EDITOR_SUPPORT" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoFbxProjectSettings()

	files
	{
		"Source/Engine/EditorSupport/*",
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

project( prefix .. "EditorScene" )

	Helium.DoModuleProjectSettings( "Source/Tools", "HELIUM", "EditorScene", "EDITOR_SCENE" )

	files
	{
		"Source/Tools/EditorScene/*",
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

	if _OPTIONS[ "gfxapi" ] == "direct3d" then
		links
		{
			prefix .. "RenderingD3D9",
		}
	elseif _OPTIONS[ "gfxapi" ] == "opengl" then
		links
		{
			prefix .. "RenderingGL",
		}
	end

project( prefix .. "Editor" )

	kind "ConsoleApp"

	configuration { "windows", "Release" }
		kind "WindowedApp"
	configuration {}

	Helium.DoBasicProjectSettings()
	Helium.DoGraphicsProjectSettings()
	Helium.DoFbxProjectSettings()

	files
	{
		"Source/Tools/Editor/**.h",
		"Source/Tools/Editor/**.c",
		"Source/Tools/Editor/**.cpp",
	}

	entrypoint "WinMainCRTStartup"

	defines
	{
		"HELIUM_HEAP=1",
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
		"Source/Tools/Editor",
		"Dependencies/freetype/include",
		"Dependencies/p4api/include",
		"Dependencies/wxWidgets/include",
		"Dependencies/bullet/src",
		"Example",
	}

	if _OPTIONS["pch"] then
		pchheader( "Precompile.h" )
		pchsource( "Source/Tools/Editor/Precompile.cpp" )
	end

	if _OPTIONS[ "gfxapi" ] == "direct3d" then
		links
		{
			prefix .. "RenderingD3D9",
		}
	elseif _OPTIONS[ "gfxapi" ] == "opengl" then
		links
		{
			prefix .. "RenderingGL",
		}
	end

	links
	{
		prefix .. "EditorScene",
		prefix .. "Ois",
		prefix .. "Bullet",
		prefix .. "Components",
		prefix .. "FrameworkImpl",
		prefix .. "PreprocessingPc",
		prefix .. "PcSupport",
		prefix .. "EditorSupport",
		prefix .. "Framework",
		prefix .. "Graphics",
		prefix .. "GraphicsJobs",
		prefix .. "GraphicsTypes",
		prefix .. "Rendering",
		prefix .. "Windowing",
		prefix .. "EngineJobs",
		prefix .. "Engine",

		-- core
		prefix .. "MathSimd",
		prefix .. "Math",
		prefix .. "Inspect",
		prefix .. "Persist",
		prefix .. "Reflect",
		prefix .. "Application",
		prefix .. "Foundation",
		prefix .. "Platform",

		"bullet",
		"mongo-c",
		"ois",
	}

	configuration "linux"
		defines
		{
			"_FILE_OFFSET_BITS=64",
			"__WXGTK__",
			"HAVE_TYPE_TRAITS",
		}
		includedirs
		{
			"/usr/include/gtk-2.0",
			"/usr/lib/x86_64-linux-gnu/gtk-2.0/include",
			"/usr/include/gio-unix-2.0/",
			"/usr/include/cairo",
			"/usr/include/pango-1.0",
			"/usr/include/atk-1.0",
			"/usr/include/pixman-1",
			"/usr/include/libpng12",
			"/usr/include/gdk-pixbuf-2.0",
			"/usr/include/harfbuzz",
			"/usr/include/glib-2.0",
			"/usr/lib/x86_64-linux-gnu/glib-2.0/include",
			"/usr/include/freetype2",
		}

	configuration { "linux", "x32", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-32/lib/wx/include/gtk2-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-32/lib",
		}

	configuration { "linux", "x32", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-32/lib/wx/include/gtk2-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-32/lib",
		}

	configuration { "linux", "x64", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-64/lib/wx/include/gtk2-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-64/lib",
		}

	configuration { "linux", "x64", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-64/lib/wx/include/gtk2-unicode-" .. wxVersion,
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
			"wx_gtk2u_gl-" .. wxVersion,
			"wx_gtk2u-" .. wxVersion,
			"gtk-x11-2.0", --gtk
			"gdk-x11-2.0", --gtk
			"pangocairo-1.0", --gtk
			"atk-1.0", --gtk
			"cairo", --gtk
			"gdk_pixbuf-2.0", --gtk
			"gio-2.0", --gtk
			"pangoft2-1.0", --gtk
			"pango-1.0", --gtk
			"gobject-2.0", --gtk
			"glib-2.0", --gtk
			"fontconfig", --gtk
			"freetype", --gtk
			"GL",
			"X11",
			"Xrandr",
			"Xi",
			"pthread",
			"dl",
			"rt",
			"m",
			"stdc++",
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
			"Dependencies/wxWidgets/macbuild-debug-unicode-32/lib/wx/include/osx_cocoa-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-32/lib",
		}

	configuration { "macosx", "x32", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-32/lib/wx/include/osx_cocoa-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-32/lib",
		}

	configuration { "macosx", "x64", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-64/lib/wx/include/osx_cocoa-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-64/lib",
		}

	configuration { "macosx", "x64", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-64/lib/wx/include/osx_cocoa-unicode-" .. wxVersion,
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
			"wx_osx_cocoau_gl-" .. wxVersion,
			"wx_osx_cocoau-" .. wxVersion,
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
			"-framework AGL",
		}

	configuration "windows"
		files
		{
			"Source/Tools/Editor/Editor.rc",
		}
		includedirs
		{
			"Dependencies/wxWidgets/include/msvc",
		}
		linkoptions -- We build monolithic wx, so ignore all the legacy non-monolithic #pragma comment directives (on windows only)
		{
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "ud",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "d",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "u",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter,
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "ud_net",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "d_net",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "u_net",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "_net",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "ud_xml",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "d_xml",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "u_xml",
			"/NODEFAULTLIB:wxbase" .. wxVersionShorter .. "_xml",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_core",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_core",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_core",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_core",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_adv",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_adv",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_adv",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_adv",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_html",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_html",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_html",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_html",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_qa",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_qa",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_qa",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_qa",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_xrc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_xrc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_xrc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_xrc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_aui",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_aui",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_aui",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_aui",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_propgrid",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_propgrid",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_propgrid",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_propgrid",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_ribbon",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_ribbon",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_ribbon",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_ribbon",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_richtext",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_richtext",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_richtext",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_richtext",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_media",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_media",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_media",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_media",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_stc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_stc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_stc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_stc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_gl",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_gl",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_gl",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_gl",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "ud_webview",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "d_webview",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "u_webview",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShorter .. "_webview",
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
			"wxmsw" .. wxVersionShorter .. "ud"
		}
	configuration { "windows", "not Debug" }
		links
		{
			"wxmsw" .. wxVersionShorter .. "u"
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

Helium.DoGameModuleProjectSettings( "PhysicsDemo" )
Helium.DoGameModuleProjectSettings( "ShapeShooter" )
Helium.DoGameModuleProjectSettings( "SideScroller" )
Helium.DoGameModuleProjectSettings( "TestBull" )
