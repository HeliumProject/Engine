<a href="http://heliumproject.github.io/">![Helium Game Engine](https://raw.github.com/HeliumProject/Engine/master/Documentation/Helium.png)</a>

# Code Organization #

Currently Helium modules are organized into three categories: Core, Tools, and Runtime.  This is done due to allow the HELIUM\_TOOLS macro to conditionally compile tools-only code at any level of the codebase (except for Core modules, which are agnostic to HELIUM\_TOOLS).
* The Tools modules aim to yield a monolithic, integrated editor for the creation of game content, as well as any command line utilities necessary to streamline production.
* The Runtime modules aim to be an asset pipeline-free build of the game engine fit for bundling on a console disk, or distributed via app store.

## Libraries ##

Helium's source code is organized into many modules (generally the former depend on the prior):

### Core ###

* Platform - abstracts base platform-specific functionality, with little time taken for adding convenience
* Foundation - platform agnostic code built on top of Platform to aid in convenience of programming
* Reflect - a powerful implementation of C++ reflection
* Persist - object persistence using Reflect
* Inspect - object property UI generation using Reflect
* Application - utility library for workstation applications
* Math/MathSimd - math utility code, bare C++ and Simd-optimized

### Engine: Tools + Runtime ### 

* Engine/EngineJobs
** Base asset pipeline implementation. Asset types are in downstream projects.
** Cache-based asset loading (intended to be used in shipping products or embedded devices, not when developing on PC)
** Jobs (I don't think we use these now, or we use them very minimally. Not to be confused with tasks!)
* Framework/FrameworkImpl
** Component system
** World and Entity classes
** Task system

### Components/Middleware Integrations ###

* Bullet - Bullet Physics
* OIS - OIS Input system
* Rendering, RenderingD3D9, RenderingGL - Rendering abstraction layer and D3D9 implementation
* Graphics, GraphicsJobs, GraphicsTypes
** Most of our pipeline is implemented here (textures, shaders, etc.)
** High-level rendering (sits on top of Rendering project)
* Windowing - Makes windows so we can draw in them
* ExampleGame - Currently under construction, this is currently a sandbox for all examples under development. These will be migrated back to the components project as they mature
* FrameworkWin - Implementation classes that set up the engine to run on PC. We will add these as we require more platforms (android or iOS, for example)

## Applications ##

### Game ###

App to run the final game that will load fully-processed game data (see ExampleGame/ExampleMain for now).

### Editor ###

Editor is a [wxWidgets](http://wxwidgets.org) application that is the user-facing content creation app.  All wxWidgets dependent code is confined to the Editor project, and as much as possible we farm out core logic to libraries in case we ever want to build a Qt or platform-locked UI application (iPad, Android, etc..).

* Inspect - Reflection integration for the editor
* Editor - The editor
* EditorSupport - In TOOLS builds, most of the classes here are registered to process your raw content (like .fbx files) and cache them for use at runtime
* EditorScene - Old logic for scene management. A lot of this may be cut, or at least heavily modified to fit a component workflow
* Application - Not sure, but this looks like semi low level utilities that all tools would want (like triggering perforce checkouts or watching disk for file changes)
* PreprocessingPc - This should probably be moved into EditorSupport

## Game Modules ##

* NOTE: Demo-specific code is in ExampleGame project
* ExampleMain_PhysicsDemo - Drops some boxes and spheres on a plane to demo bullet integration and rendering
* ExampleMain_ShapeShooter - Demo of player-controlled avatar where you can shoot at stuff by clicking (work in progress)
* TestApp - Just a scratchpad for code while testing. Not important.
* EmptyGame and EmptyMain - If you want to start building on top of helium, the quickest thing to do would be to copy these projects (or just use them if you want). This will get you started with the gameplay system up and running, ready for you to add your own components, tasks, and art assets.

## FAQ ##

'''What platforms are supported?''' - We fully support Windows. We want to add support for mac and linux, and our code compiles with clang and GCC. However, we need an OpenGL renderer! This is very high on our list, but help would be appreciated! We also would like to target mobile platforms.

'''How mature is the code?''' - Almost all of the projects in the core engine section is stable although Framework may still receive significant changes. Many of the components outside of ExampleGame are unlikely to drastically change beyond evolving as we improve the component system. The editor does not work currently as we have focussed on the core engine for the last year. This is something we hope to address soon. Examples are currently being created.

'''Where do I start?''' - Look at the example code AND the data used to drive it. When the application starts, a list of all tasks the engine will perform is printed to STDOUT. You should be able to search for these tasks and see how they work. Then try to use some of the existing components to do something interesting. That should give you a good primer on where things are and what the workflow is like to add new gameplay logic.

'''Can I just use parts of the engine?''' - Yes. Some very common scenarios might be:
* Platform/Foundation - Useful for just about any project
* Math/MathSimd - Could easily be used in any project
* Platform/Foundation/Reflect/Persist - If you need a reflection framework, you could just grab these as well as Platform/Foundation.
* Platform/Foundation/Reflect/Persist/Engine - If you wanted to build your own game engine but want to keep it very light or experiement with your own engine designs, you could easily start with just Engine and everything below it. You'd probably want to take PcSupport as well.

'''Where is all the code in the examples?''' - Almost all of the logic is accomplished through data. See the .json files in Helium/ExampleGames.

'''I want to use my own physics/rendering/etc.''' - Create a world-level component for the manager. Create entity-level component(s) as needed for per-game-object things. For example, meshes or physical bodies. The bullet integration is a good example of how to inject your own middleware and is actually not very much code. Remember, use the asset pipeline to your advantage! The data-management tools provided by helium can make existing middleware extremely to fast to work with.

'''I have a question and it's not on this list''' - Join freenode, channel #helium, and ask away. If you're still stuck and think something is broken, you could also create a github issue. https://github.com/HeliumProject/Engine/issues
