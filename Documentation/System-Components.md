<a href="http://heliumproject.github.io/">![Helium Game Engine](https://raw.github.com/HeliumProject/Engine/master/Documentation/Helium.png)</a>

# Component System #

This document describes the design of the component system and rationale of design choices behind it. Apologies for first-person writing style, but much of this will be editorial rather than objective. It will be more natural to write this way!

## First Principles ##

Goals

* Non-intrusively inject new data and behavior onto objects
** Allow game-specific code to stay out of "engine"
** Allow end users to easily integrate existing middleware with engine
* Allow for broad reuse of very small, simple behaviors (such as wait 3 seconds, then do X)
** Patterns for reusing logic represented by components are poorly understood. In traditional designs, logic/data all housed in the same class can easily interact. The existence of the struct implies all the functionality contained within it as all the code is implemented inline, and not optional. We must provide a facility to allow components to use each other.
** For behaviors strictly defined by multiple interacting components to work, the components interacting as a group must have robust communication.
* Avoid having unnecessary logic/data in objects that don't need it
* As this system is so often touched at runtime, it must be minimal and fast

How do components get used?

* Allocate/Deallocate
* Tick
* Count/Iterate
* Get data (resolve owning object, world, or peer components)
* Cooperate to perform logic

There are actually several scenarios for counting/iterating

*** All allocated components vs All allocated components in particular set
*** Components of an exact type vs components implementing a types

### Basic design ###

* Preallocate all componenents in a contiguous pool
** Makes constant time allocation/deallocation possible
** Allows fast iteration as components may be adjacent to each other in iteration
* Bookkeeping data for a component is partially stored inline in the component, partially in a parallel array (based on frequency of use). Inline information is stored in smaller handles to keep Component as small as possible.
* Provide a typesafe API with templates

### Component Communication ###

One obvious solution to allowing components to communicate is to use messages. This approach has the following problems:

* Scales poorly as a message has to be delivered to every single component instead of just the components needed
* Difficult to debug
* Significant maintenance is spent on messaging bookkeeping

Ultimately, a message and function call are the same thing. Except function calls are faster (no serializing messages or dispatching) and safer (type safety!) So there is no reason to use messaging! One solution might be for one component to find another peer component and register itself for a function call. But none of this leads to execution that is code-cache friendly.

Instead of messaging, we will view our components as nodes in a large data flow.

.. TODO: Finish