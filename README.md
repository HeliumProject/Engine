<a href="http://heliumproject.org/">![Helium Game Engine](https://raw.github.com/HeliumProject/Helium/master/Data/Textures/Helium.png)</a>
==========================================================================================================================================

Location
========
git://github.com/HeliumProject/Helium.git

Goals
=====

Helium aspires to be a fully-featured open-source game engine:
* Permissively licensed (BSD-style)
* Designed to scale to large-scale game development
* Portable to different architectures, including game consoles, smartphones, and tablets
* Use familiar industry-standard DCC user interface and usability design
* Import content using production proven interchange formats (like FBX)

History
=======

Helium was born directly out of [Insomniac Games][] [Nocturnal Initiative][].  Nocturnal Initiative was created by Geoff Evans, Andy Burke, and Mike Acton as a way of sharing proven game development techniques with the community through the sharing of source code.  It was announced at GDC San Francisco 2008.  All source code shared via Nocturnal was production-tested library code.

After leaving Insomniac, Geoff Evans, Andy Burke, Rachel Mark, and Paul Haile continued working on code released through Nocturnal at [WhiteMoon Dreams][].  WhiteMoon Dreams' generous support necessitated a new name for its independent development of the technology.  Helium, as a project distinct from Nocturnal, was born.  Over the summer and fall of 2010 Helium was refactored and improved to include better support for unicode, improved UI components in the Editor, and better cross-platform support.  WhiteMoon Dreams donated to the Helium Project their entire custom built engine, named Lunar.  Lunar was mostly written by Ted Cipicchio.  Lunar focused primarly on memory allocation, containers, rendering, FBX import.

[Nocturnal Initiative]: http://nocturnal.insomniacgames.com/
[Insomniac Games]: http://www.insomniacgames.com/
[WhiteMoon Dreams]: http://whitemoondreams.com/

Organization
============

Helium is built using [http://industriousone.com/premake premake].  Premake interprets lua script and generates platform-specific IDE project files.  Currently Helium modules are built into three categories: Core, Tools, and Runtime.  This is done due to allow the HELIUM_TOOLS macro to conditionally compile tools-only code at any level of the codebase (except for Core modules, which are agnostic to HELIUM_TOOLS).
* The Tools modules aim to yield a monolithic, integrated editor for the creation of game content, as well as any command line utilities necessary to streamline production.
* The Runtime modules aim to be an asset pipeline-free build of the game engine fit for bundling on a console disk, or distributed via app store.

Helium's source code is organized into many modules:
* Platform - abstracts base platform-specific functionality, with little time taken for adding convenience
* Foundation - platform agnostic code built on top of Platform to aid in convenience of programming
* Reflect - a powerful implementation of C++ reflection
* Persist - object persistence using Reflect
* Inspect - object property UI generation using Reflect
* Math/MathSimd - math utility code, bare C++ and Simd-optimized
* Engine - implements rendering and other critical engine systems
* TestApp - app used to verify that systems are working, helps to aid making major or dangerous changes
* Editor - [http://wxwidgets.org wxWidgets] application that is the user-facing content creation app, all wxWidgets dependent code is confined to the Editor project
* Game - app to run the final game that will load fully-processed game data (see ExampleGame/ExampleMain for now)

Design
======

Fully Native Compilation
----------------------------------------

At the current time Helium is fully compiled to native code.  Our long term goal is to build system such that implementing new features is easy to add via compiled code instead of extended through a scripting interface.  Scripting (as a game tools concept) is planned to be implemented as basic building blocks that are placed within the 3D game world that are connected together via a message-queue based communication system.  Due to this parallelism should be easier to achieve.  Prefab structures will allow for reusable chunks of logic to be used through the game to speed content creation.  These reusable chunks expose ports for input and output within the larger context of the game world.

Reflection
----------------------------------------

Helium makes extensive use of [[System:Reflect|C++ Reflection]] to automate rote tasks that would otherwise require boilerplate code.  [[User:gorlak|Geoff Evans]] has written an article outlining C++ Reflection and its virtues [http://www.gamasutra.com/view/feature/6379/sponsored_feature_behind_the_.php here].

Components
----------------------------------------

Helium uses components exclusively for implementing gameplay and gameplay-level systems. This allows for you to extend the engine non-intrusively with your gameplay, custom systems, or integration of middleware of your choosing.

Tasks
----------------------------------------

Your logic can be placed in the "game loop" without modifying the engine. Just add a task and define constraints: For example "run after physics" and "run before rendering". The scheduler will make sure your logic gets run at the right time. Tasks should be very small pieces of work that operate over a number of components. This will allow us to have very aggressive concurrency in the future.

Online resources:

* Website: [http://heliumproject.org http://heliumproject.org]
* Wiki: [http://heliumproject.org/wiki/ http://heliumproject.org/wiki/]
* GitHub: [http://github.com/HeliumProject/Helium http://github.com/HeliumProject/Helium]
* IRC: #helium @ irc.freenode.net

Building
========

Prerequisites:

All Platforms
 - [FBX SDK 2014.1](http://usa.autodesk.com/adsk/servlet/pc/item?siteID=123112&id=10775847)

Windows
 - Visual Studio 2010 or 2012. (Visual Studio 2008 SP1 will probably work, but is not actively supported; service pack 1 is required for regular expression and compiler fixes.)
 - DirectX SDK (installed to default location)

OSX
 - XCode 4
 - XCode Command Line Tools (install from within XCode preferences)

Linux
 - sudo apt-get install build-essential libgtk2.0-dev libglapi-mesa libbullet-dev libfreetype6-dev libpng12-dev libnvtt2-dev libois-dev libz-dev

First, grab our source tree from git. Ensure that you run "git submodule update --init --recursive".

Next, generate the project files using premake. A premake4 binary is included for Windows. Project files generated by premake will be placed in the `/Premake` directory relative to cwd.

Example for Visual Studio 2010:

    cd Dependencies
    ../premake vs2010
    
    cd ..
    premake vs2010

The first call to premake builds the Dependencies solution, and the second builds the main solution.  Substitute vs2010 for vs2012, or gmake on linux, or xcode4 on macosx.

Once the project files have been generated, open Dependencies.sln.  It will be in Dependencies/Premake/. This solution needs to be built in all configuration and platform combinations that you intend to build Helium in. If you are using Visual
Studio, you may find it convenient to "Batch Build" all combinations now so that you don't need to worry about it later.

After you've built Dependencies.sln, you should be able to build Helium. You will most likely want to build Tools.sln, which includes the editor, whereas Runtime.sln builds only "shipping" code (no asset pipeline or other tools). If you opted not to batch build all Dependencies.sln permutations in the last step, then ensure that our configuration and platform choices are consistent with the ones you made there.