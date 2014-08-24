<a href="http://heliumproject.org/">![Helium Game Engine](https://raw.github.com/HeliumProject/Helium/master/Documentation/Helium.png)</a>

# Architecture #

## Fully Native Compilation ##

At the current time Helium is fully compiled to native code.  Our long term goal is to build system such that implementing new features is easy to add via compiled code instead of extended through a scripting interface.  Scripting (as a game tools concept) is planned to be implemented as basic building blocks that are placed within the 3D game world that are connected together via a message-queue based communication system.  Due to this parallelism should be easier to achieve.  Prefab structures will allow for reusable chunks of logic to be used through the game to speed content creation.  These reusable chunks expose ports for input and output within the larger context of the game world.

## Reflection ##

Helium makes extensive use of [C++ Reflection](https://github.com/HeliumProject/Reflect) to automate rote tasks that would otherwise require boilerplate code.

## Tasks ##

Your logic can be placed in the "game loop" without modifying the engine. Just add a task and define constraints: For example "run after physics" and "run before rendering". The scheduler will make sure your logic gets run at the right time. Tasks should be very small pieces of work that operate over a number of components. This will allow us to have very aggressive concurrency in the future.

The following describes an architecture that allows downstream code to non-intrusively inject completely new functionality into the engine. The *Engine* project is comprised of code that allows for loading and management of assets. The *Framework* project is comprised of high-level gameplay concepts such as World, Entity, and Component, as well as task scheduling.

## World ##

Any number of worlds may be created to contain entities and world-level systems. World-level systems will be implemented via components.

Worlds are composed of 1..n slices, and each slice has 0..n entities. Slices are intended to allow streaming parts of the world in/out.

## Entities ##

Entities at runtime will be light, deriving only from Object. This will allow them to be compatible with our smart pointers and reflect. However, runtime objects would never be saved as an asset, and don't have a name or path. Reflect would still be capable of introspecting a runtime object, but this is only intended to conveniently inspect the contents of a runtime object, not to allow permanent serialization.

In addition, there will be no object hierarchy. Having a hierarchy encourages compounding responsibilities into a single class which leads to code that is difficult to understand and reuse. All extensions to add data or functionality to a game object will be accomplished with components.

## Components ##

Components are attached to worlds and entities. Components are pooled by type and pre-allocated. When a world is constructed, all components are created contiguously in memory.

Game logic can query for sets of components that are on the same world or entity. Precisely how the search occurs is optimized by how many components are allocated of each type. The intention is that you can use a simple function to pump data between components. The tasking system allows you to set up new data flows between components.

## Tasks ##

The tasking system allows you to non-invasively add logic to the game loop. Your new logic can be injected anywhere in the loop by specifying other tasks that your task must run before or after. When the game runs, a schedule is calculated that satisfies the requirements of all the tasks. In the case where no schedule can fulfill all the requirements, you will receive an error message.

Ultimately, the scheduling boils down to an array of pointers to all task functions. Your function can do absolutely anything. But most of the time, it is used to call functions on components. In the future, the tasking system could support parallel execution by having tasks declare which types of components it reads and writes.

## Asset ##

Any data required by the game to run will be loaded through the asset system. Assets represent data that is required to run the game. They include both structured data (reflection-driven) and arbitrary binary data (such as compressed textures). Assets are stored in a tree of packages. Some assets will correspond to art assets such as textures or shaders. In those cases, the asset describes how to import the data, and at runtime holds the processed data. One example would be a shader, which might expose named configurable settings in structured data and also carry the compiled shader binary. Other assets might simply be structured data.

Assets have a type, template, and path. The type specifies the C++ class that the asset is an instance of. The template is a reference to any other asset of the same type. When an asset is constructed, the template is constructed, and then fields are overridden. Templates allows for data inheritance. The path of the asset is simply the package the asset is in, as well as the name of the asset.

Assets will be flattened at runtime. This way, construction of an asset can be done by reading a chunk of memory, rather than having to recursively walk a list of templates, and then iterate from base to extended asset resetting all the fields. Additionally, assets will be processed once and stored in a cache. The asset will be rebuilt only when it changes (either structured data or binary data).

The serialization system for assets is smart enough to deal with references, including cyclical references. Loading an asset always forces other assets it references to also be loaded. Assets are always singletons, in that if you request an object by a path, you will always get the same pointer. This pointer will be the same pointer as might be in some other asset that refers to this asset.

## Definitions ##

A common solution to saving/loading world state is to allow all runtime objects to serialize/deserialize themselves. Unfortunately, this can waste significant memory as an object instantiated to serve as a template to make more objects necessarily includes runtime memory that is not in use. Additionally, the runtime representation carries the weight of data that might only be needed at construction. In addition, it can becomes difficult to understand what parts of the objects are "alive" and "dead" at runtime.

To solve this waste, we will separate runtime and design-time representations of objects. All runtime objects will be constructed by a definition object. Definitions are assets.

The engine includes tools to allow reuse of definitions to manufacture objects at runtime. For example, a set of component definitions can be specified in data (as an asset). The set allows parameters to be injected and components to have references to each other. In this way, you can define complex behaviors as component definitions and reuse the data. Additionally, an EntityDefinition can be used to construct an entity with components. A scene definition can describe many entities (as entity definitions) to construct to compose a scene.

Runtime objects may keep a const pointer to their definition in order to avoid duplicating static data across all runtime instances. In cases where we care about speed, the component author can choose to cache data from the definition inline with the component and NOT keep a pointer to the definition, allowing it to unload.

The editor will only save and load the definitions used to compose an entity, never the runtime instance.

## Proxy Objects ##

We wish to avoid having editor code in "shipping" runtime code. This necessitates that the editor track the existence of entities non-intrusively. At edit-time, when the engine loads an entity, a proxy object will be created. A proxy object exists only in the editor. When editing, every scene or entity that exists will have a corresponding proxy object owned by the editor. The proxy object maintains the relationship between the runtime representation, definition asset, and the editor. 

For example, when a scene definition is loaded, a scene proxy would be created to track the runtime and design-time state of the scene. Additionally, entity proxies would be created for every entity in the scene. If we wanted to simulate within the editor, every entity would be observed by their corresponding entity proxy. Even if the runtime instance was destroyed when simulating, the proxy would continue to exist so that the editor could track that the object HAD existed before. When resetting the simulation, the proxy object would handle resetting the object to the initial state using the definition that created it.

The proxy object would also handle edit-time code, such as drawing design-time visual aids, such as paths.
