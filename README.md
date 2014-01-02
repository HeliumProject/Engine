<a href="http://heliumproject.org/">![Helium Game Engine](https://raw.github.com/HeliumProject/Helium/master/Data/Textures/Helium.png)</a>

Helium aspires to be a fully-featured open-source game engine:
* Permissively licensed (BSD-style)
* Designed to scale to large-scale game development
* Portable to different architectures, including game consoles, smartphones, and tablets
* Use familiar industry-standard DCC user interface and usability design
* Import content using production proven interchange formats (like FBX)

# History #

Helium was born directly out of [Insomniac Games](http://www.insomniacgames.com/) [Nocturnal Initiative](http://nocturnal.insomniacgames.com/).  Nocturnal Initiative was created by Geoff Evans, Andy Burke, and Mike Acton as a way of sharing proven game development techniques with the community through the sharing of source code.  It was announced at GDC San Francisco 2008.  All source code shared via Nocturnal was production-tested library code.

After leaving Insomniac, Geoff Evans, Andy Burke, Rachel Mark, and Paul Haile continued working on code released through Nocturnal at [WhiteMoon Dreams](http://whitemoondreams.com/).  WhiteMoon Dreams' generous support necessitated a new name for its independent development of the technology.  Helium, as a project distinct from Nocturnal, was born.  Over the summer and fall of 2010 Helium was refactored and improved to include better support for unicode, improved UI components in the Editor, and better cross-platform support.  WhiteMoon Dreams donated to the Helium Project their entire custom built engine, named Lunar.  Lunar was mostly written by Ted Cipicchio.  Lunar focused primarly on memory allocation, containers, rendering, FBX import.

# Design #

## Fully Native Compilation ##

At the current time Helium is fully compiled to native code.  Our long term goal is to build system such that implementing new features is easy to add via compiled code instead of extended through a scripting interface.  Scripting (as a game tools concept) is planned to be implemented as basic building blocks that are placed within the 3D game world that are connected together via a message-queue based communication system.  Due to this parallelism should be easier to achieve.  Prefab structures will allow for reusable chunks of logic to be used through the game to speed content creation.  These reusable chunks expose ports for input and output within the larger context of the game world.

## Reflection ##

Helium makes extensive use of [C++ Reflection](https://github.com/HeliumProject/Reflect) to automate rote tasks that would otherwise require boilerplate code.

## Components ##

Helium uses components exclusively for implementing gameplay and gameplay-level systems. This allows for you to extend the engine non-intrusively with your gameplay, custom systems, or integration of middleware of your choosing.

## Tasks ##

Your logic can be placed in the "game loop" without modifying the engine. Just add a task and define constraints: For example "run after physics" and "run before rendering". The scheduler will make sure your logic gets run at the right time. Tasks should be very small pieces of work that operate over a number of components. This will allow us to have very aggressive concurrency in the future.

Online resources:

* Website: [http://heliumproject.org](http://heliumproject.org)
* Wiki: [http://heliumproject.org/wiki/](http://heliumproject.org/wiki/)
* GitHub: [http://github.com/HeliumProject/Helium](http://github.com/HeliumProject/Helium)
* IRC: #helium @ irc.freenode.net

# Building #

Helium is built using [premake](http://industriousone.com/premake).  Premake interprets lua script and generates platform-specific IDE project files.  Currently Helium modules are built into three categories: Core, Tools, and Runtime.  This is done due to allow the HELIUM\_TOOLS macro to conditionally compile tools-only code at any level of the codebase (except for Core modules, which are agnostic to HELIUM\_TOOLS).
* The Tools modules aim to yield a monolithic, integrated editor for the creation of game content, as well as any command line utilities necessary to streamline production.
* The Runtime modules aim to be an asset pipeline-free build of the game engine fit for bundling on a console disk, or distributed via app store.

Prerequisites:

All Platforms
 - [FBX SDK 2014.2](http://usa.autodesk.com/adsk/servlet/pc/item?siteID=123112&id=10775847)

Windows
 - Visual Studio 2010 or 2012. (Visual Studio 2008 SP1 will probably work, but is not actively supported; service pack 1 is required for regular expression and compiler fixes.)
 - DirectX SDK (installed to default location)

OSX
 - Recent XCode Command Line Tools (install from within XCode preferences)

Linux
 - sudo Dependencies/install-packages.sh

First, grab our source tree from git. Ensure that you run "git submodule update --init --recursive".

Next, generate the project files using premake. A premake4 binary is included for Windows. Project files generated by premake will be placed in the `/Premake` directory relative to cwd.

Example for Visual Studio 201x:

    cd Dependencies
    ../premake vs201x
    start Dependencies.sln
    
    cd ..
    premake vs201x
    start Helium.sln

On mac or linux, use premake to generate makefiles (Xcode support inside premake is on hold as of late):

    cd Dependencies
    ../premake.sh gmake
    make -j8
    
    cd ..
    ./premake.sh gmake
    make -j8
    
On mac you may get a dependency check error about your FBX SDK containing spaces, to work around it just make a symlink without spaces and set the FBK_SDK environment variable.  See mklink and export (amending your .bash_profile file) for help setting those up.

# Documentation #

* [Code Organization](Documentation/CodeOrganization.md)
* [Game Organization](Documentation/GameOrganization.md)
* [System - AssetLoader](Documentation/System-AssetLoader.md)
* [System - Components](Documentation/System-Components.md)
