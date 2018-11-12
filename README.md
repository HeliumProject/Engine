[![Build Status](https://travis-ci.org/HeliumProject/Engine.svg?branch=master)](https://travis-ci.org/HeliumProject/Engine)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/HeliumProject/Engine?branch=master&svg=true)](https://ci.appveyor.com/project/GeoffEvans/Engine)

<a href="http://heliumproject.org/">![Helium Game Engine](https://raw.github.com/HeliumProject/Engine/master/Documentation/Helium.png)</a>

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
* GitHub: [http://github.com/HeliumProject/Engine](http://github.com/HeliumProject/Engine)
* Slack: [http://heliumproject.slack.com](http://heliumproject.slack.com) (ask @gorlak for an invite)

# Building #

Helium is built using [premake5](https://github.com/premake).  Premake interprets lua script and generates platform-specific IDE project files.

## Prerequisites ##

#### All Platforms ####
* [FBX SDK 2016.1.2](http://usa.autodesk.com/adsk/servlet/pc/item?id=24746731&siteID=123112)

#### Windows ####
* [Visual Studio 2015](http://www.visualstudio.com)
* [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

#### OSX ####
[XCode](https://developer.apple.com/xcode) Command Line Tools (install from within XCode preferences):

    xcode-select --install

Prerequisites can be installed via:

    sudo Dependencies/install-packages-macos.sh

#### Linux ####
[GCC 6](https://gcc.gnu.org/gcc-6/changes.html)

Prerequisites can be installed via:

    sudo Dependencies/install-packages-linux.sh

## Compile ##

First, grab our source tree from git and ensure that you fetch all the submodules by doing:

    git submodule update --init --recursive

Next, generate the project files using premake.  An appropriate build of premake is includedin the repository.

On Windows, generate Visual Studio 201x projects (replace 201x with your desired version):

    cd Dependencies
    ..\premake vs2015
    start Build\Dependencies.sln
    
    cd ..
    premake vs2015
    start Build\Helium.sln

On OSX and Linux, use premake to generate makefiles (Xcode support inside premake is on hold as of late):

    cd Dependencies/Build
    ../premake.sh gmake
    make -j8
    
    cd ../Build
    ./premake.sh gmake
    make -j8

## Caveats ##

* On OSX you may get a dependency check error about your FBX SDK containing spaces, to work around it just make a symlink without spaces and set the FBK_SDK environment variable.  See mklink and export (amending your .bash_profile file) for help setting those up.

