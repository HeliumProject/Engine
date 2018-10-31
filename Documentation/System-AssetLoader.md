<a href="http://heliumproject.org/">![Helium Game Engine](https://raw.github.com/HeliumProject/Engine/master/Documentation/Helium.png)</a>

# WARNING #

This information may be a little stale:
* GameObjects are now called Assets
* Linking process is a little different as it now goes through reflect/persist rather than doing a deep walk across all object fields

# AssetLoader System #

## GameObjects ##

GameObjects are objects defined by data that support inheritance and composition. Currently the Engine project centers around GameObjects and the supporting systems. Some related systems:

* GameObject Paths
* Packages
* GameObjectLoader
** PackageLoader/ObjectLoader implementations can be swapped out to change where and how data is loaded.
*** See [[#GameObjectLoader]] below

## GameObject Paths ##

Format for a package object is

<pre>
/Package/SubPackage:ObjectName*InstanceIndex
</pre>

A game object path includes
* Package
* Object Name
* Object Index

### Packages ###

A package represents a group of resources. During development, a package is represented by a file system directory. For example, the path /Config/DefaultWin:GraphicsConfig implies that there is a folder Config, with a subfolder DefaultWin, that has a file named GraphicsConfig.xml.object

In addition, packages have a !toc.xml file. This allows object metadata to be read without having to touch every file. During development, this file is verified and discarded if objects fall out of sync. It is intended that only binary cached objects are used on an end-user's machine.

When dealing with development-time objects each object file is serialized via Reflect. Exactly two objects are in every file:

* ObjectDescriptor - Includes the type, name, and owner of the object
* Payload Object - The data for the object we have stored

This ObjectDescriptor comes first so that a "blank" object of the proper type, based on the desired template, can be created. Once this object is created, the payload object will overwrite the appropriate fields within the object. Inheritance is supported. During development, we traverse hierarchies of objects and iteratively build properties (slow). When caching an object, we duplicate parent properties into child objects such that creating a new object does not require loading a bunch of separate chunks.

### Special Packages ###
The following special packages/objects are created on startup

<pre>
TODO: Fill this out
</pre>

In addition, the config package is used to load settings objects which can be enumerated by systems that depend on Engine.

### Default Objects ###

Every class has a template object. This template object specifies default properties for any instance of this class. When an object of type X is created, that object instance inherits properties from the X default template object. This template object is created intrinsically when a class is registered and placed in a special package that is created automatically at runtime.

### GameObjectLoader ###

The following object loaders are available

* ArchiveObjectLoader - Loads objects from reflect xml
* EditorObjectLoader - Internally reuses ArchiveObjectLoader. All objects loaded are saved to binary cache. Otherwise, works exactly like the ArchiveObjectLoader
* CacheObjectLoader - Loads objects from binary cache

ArchiveObjectLoader and EditorObjectLoader will only be available in a "tools-enabled" build of the engine. End users will only ever use the CacheObjectLoader.

When using the EditorObjectLoader, the overall process for how objects end up in the cache are as follows:
* When an object is precaching, if it is a resource and the cached resource data is out of date:
** Reprocess it (for all platforms) (this happens in [ResourceType]ResourceHandler::CacheResource
** Save this data on the resource object (see m_preprocessedData on Resource - only available in a tools build)
** Force resource to load it in (this happens in [ResourceType]::SerializePersistentResourceData)
* After an object is totally finished loading
** Write properties to stream
** Write PreprocessedData to stream
*** Write persistentDataBuffer
*** Write subDataBuffers
** Save stream by calling Cache::CacheEntry

### Object loading outline ###

<pre>

==================== GameObjectLoader ======================

BeginLoadObject
 - GameObjectLoader gets package loader
 - GameObjectLoader::LoadRequest is created with:
   - path
   - package loader
   - request count == 1
   - invalid package load request id
   - null reference that will point to the requested object
   - empty link table
   - state flags

   NOTE: Load request remains until TryFinishLoad is called as many times as BeginLoadObject is called

TryFinishLoad
 - If request is not fully loaded, return false
 - Remove the load request if this is the last TryFinishLoad call

On tick
 - Tick package loaders (Defers to ArchiveObjectLoader to call Tick on every ArchivePackageLoader)
 - Foreach load request in map
   - Increment request count
   - TickLoadRequest(request)
   - Decrement request count
   - If its request count == 0 after tick, destroy the request
     - NOTE: Other threads asynchronously could decrease count by calling TryFinishLoad, or a link dependency is satisfied

TickLoadRequest
 - if not preloaded, Preload
 - if preloaded and not linked, Link
 - if linked and not precached, Precache
 - if precached and not finalized, FinalizeLoad

TickPreload
 - Return if package loader TryFinishPreload fails
 - BeginLoadObject on the package if we havn't already
 - TryFinishLoadObject on package
   - If success, mark object as preloaded
   - If fail, we will try again next frame

TickLink
 - Iterate link table
   - If request for object is preloaded, set link table object pointer
 - Bail if any link entries not set
 - Walk the properties and replace link indices with valid pointers
 - Mark object as linked

TickPrecache
 - Wait until every link dependency is fully loaded
   - When each entry is completely loaded, clear the load request index and object pointer
 - Wipe the link table
 - OnPrecacheReady callback
 - Start precache if it's not started yet
 - Bail if precache not finished
 - Mark object as precached

TickFinalizeLoad
 - FinalizeLoad called on object
 - OnLoadComplete callback
 - LOADED flag set

==================== PRELOADING PACKAGE ==================
Package::BeginPreload
 - Kick off loading parent
 - Check TOC and kick off any work to fix TOC
Package::TickPreload
 - Handle reading any files for TOC fixup
 - Wait for parent package to load
 - Create our package using parent package as parent
 - NOTE: Any auto-add of objects to package we would do now
 - Package object flags set to preloaded, linked, loaded, and FinalizeLoad called
 - Package preload is now considered "Finished"
Package::TryFinishPreload
 - Returns false if package preload is not finished
 - Returns true if package preload is finished (no work done here)

================= PACKAGE LOADING OBJECTS =================
Package::BeginLoadObject
 - If asked for package
   - Trivially allocate a package request, set it, and return (so TryFinishLoadObject guaranteed to work next frame)
 - Look for object in TOC
 - Create a request for it
 - If this object exists, mark the request as finished (so TryFinishLoadObject guaranteed to work next frame)
 - Issue load request for template object (used TOC loaded during package preload to know what it is)
 - Load request for parent object

Package::TickDeserialize
 - Finish loading template or bail
 - Finish loading owner or bail
 - Create object (or on later ticks check that the existing object is good)
 - Deserialize
   - Dump raw properties over object
   - Populate package link table
     - table is array of path/object loader load request ids
 - If object is a resource setup to do TickPersistentResourcePreload, otherwise, mark as preloaded

Package::TickPersistantResourcePreload (NOTE: Skipped for objects that do not extend Resource)
 - No clue what this does.. skipping it for now

Package::TryFinishLoadObject
 - Returns false if object is not preloaded
 - If it is preloaded
   - Return false if template not loaded
   - Return false if owner not loaded
   - Cleanup any cache-related stuff that was going on
   - Set broken flag if bad stuff happened
   - Copy package link table to out param
   - Clear out request data
</pre>

### Resource Caching ###

<pre>
GameObjectLoader
 - ArchiveObjectLoader (Uses ArchivePackageLoader)
 - EditorObjectLoader (Uses ArchivePackageLoader) - Adds CacheObject, OnPrecacheReady, OnLoadComplete
   - Uses ObjectPreprocessor
     - Uses Cache
 - CacheObjectLoader (Uses CachePackageLoader)
   - Uses Cache from CacheManager

Resource
 - Font::Serialize (moving to reflect) - Saves config-time info (like font size)
 - Font::SerializePersistentResourceData (moving to Font::PersistentResourceData class, saved via Reflect) - Saves cached info (like max character width)
 - NeedsPrecacheResourceData/BeginPrecacheResourceData/TryFinishPrecacheResourceData - Generally issues BeginLoadSubData calls

ResourceHandler
 - CacheResource: Create/Populate PersistentResourceData object, Create/Populate sub-data buffers (generally would hold data like raw textures)

GameObjectLoader::TickPrecache
 - EditorObjectLoader::OnPrecacheReady
   - If it's a resource ObjectPreprocessor::LoadResourceData
    - If timestamp not in sync or load fails (perhaps resource never been cached) for any platform, ObjectPreprocessor::PreprocessResource
           (Note: Calls LoadCachedResourceData which calls LoadPersistentResourceData)
      - Wipe result of Resource->GetPreprocessedData() for each platform
      - Get Resource Handler
      - ResourceHandler->CacheResource()
      - Reload PersistentResourceData
 - if NeedsPrecacheResourceData()
   - (Once) Object->BeginPrecacheResourceData()
   - (Per Tick) Object->TryFinishPrecacheResourceData()

TODO: We are calling OnPrecacheReady multiple times - seems wasteful and possibly dangerous

GameObjectLoader::TickFinalizeLoad
 - EditorObjectLoader::OnLoadComplete
   - EditorObjectLoader::CacheObject
     - ObjectPreprocessor::CacheObject
       - Foreach Platform
         - Get PlatformPreprocessor
         - Get Cache
         - Write properties to stream
         - If resource
           - Write Resource->GetPreprocessedData() to stream
         - Persist stream by calling Cache::CacheEntry
         - If resource
           - Handle sub data buffers
</pre>
