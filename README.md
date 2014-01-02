<a href="http://heliumproject.org/">![Helium Game Engine](https://raw.github.com/HeliumProject/Helium/master/Data/Textures/Helium.png)</a>

Helium aspires to be a fully-featured open-source game engine:
* Permissively licensed (BSD-style)
* Designed to scale to desktop, console, and mobile
* Utilize familiar industry-standard DCC user interface and usability design
* Import content using production proven interchange formats (like FBX)

# Documentation #

Introductions
* [History](Documentation/Intro-History.md)
* [Architecture](Documentation/Intro-Architecture.md)
* [Organization](Documentation/Intro-Organization.md)

Systems
* [AssetLoader](Documentation/System-AssetLoader.md)
* [Components](Documentation/System-Components.md)

# Resources #

* Website: [http://heliumproject.org](http://heliumproject.org)
* Wiki: [http://heliumproject.org/wiki/](http://heliumproject.org/wiki/)
* GitHub: [http://github.com/HeliumProject/Helium](http://github.com/HeliumProject/Helium)
* IRC: #helium @ irc.freenode.net

# Building #

Helium is built using [premake](http://industriousone.com/premake).  Premake interprets lua script and generates platform-specific IDE project files.

## Prerequisites ##

All Platforms
* [FBX SDK 2014.2](http://usa.autodesk.com/adsk/servlet/pc/item?siteID=123112&id=10775847)

Windows
* [Visual Studio](http://www.visualstudio.com) 2010 or 2012. (Visual Studio 2008 SP1 will probably work, but is not actively supported)
* [DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=23549)

OSX
* [XCode](https://developer.apple.com/xcode) Command Line Tools (install from within XCode preferences):

    xcode-select --install

Linux
* Run our script:

    sudo Dependencies/install-packages.sh

## Compile ##

First, grab our source tree from git and ensure that you fetch all the submodules by doing:

    git submodule update --init --recursive

Next, generate the project files using premake.  An appropriate build of premake is includedin the repository.

On Windows, generate Visual Studio 201x projects (replace 201x with your desired version):

    cd Dependencies
    ..\premake vs201[x]
    start Dependencies.sln
    
    cd ..
    premake vs201[x]
    start Helium.sln

On OSX and Linux, use premake to generate makefiles (Xcode support inside premake is on hold as of late):

    cd Dependencies
    ../premake.sh gmake
    make -j8
    
    cd ..
    ./premake.sh gmake
    make -j8

## Caveats ##

* On OSX you may get a dependency check error about your FBX SDK containing spaces, to work around it just make a symlink without spaces and set the FBK_SDK environment variable.  See mklink and export (amending your .bash_profile file) for help setting those up.
