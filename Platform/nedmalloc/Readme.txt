nedalloc v1.06 ?:
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

by Niall Douglas (http://www.nedprod.com/programs/portable/nedmalloc/)

Enclosed is nedalloc, an alternative malloc implementation for multiple
threads without lock contention based on dlmalloc v2.8.4. It is more
or less a newer implementation of ptmalloc2, the standard allocator in
Linux (which is based on dlmalloc v2.7.0) but also contains a per-thread
cache for maximum CPU scalability.

It is licensed under the Boost Software License which basically means
you can do anything you like with it. This does not apply to the malloc.c.h
file which remains copyright to others.

It has been tested on win32 (x86), win64 (x64), Linux (x64), FreeBSD (x64)
and Apple Mac OS X (x86). It works very well on all of these and is very
significantly faster than the system allocator on Windows and FreeBSD. If
you are using a recent Apple Mac OS X then you probably won't see much
improvement (and kudos to Apple for adopting an excellent allocator).

By literally dropping in this allocator as a replacement for your system
allocator, you can see real world improvements of up to three times in normal
code!

Table of Contents:
  A: How to use
  B: Notes
  C: Speed Comparisons
  D: Troubleshooting
  E: Changelog

A. To use:
-=-=-=-=-=
Drop in nedmalloc.h, nedmalloc.c and malloc.c.h into your project.
Configure using the instructions in nedmalloc.h. Make sure that you call
neddisablethreadcache() for every pool you use on thread exit, and don't
forget neddisablethreadcache(0) for the system pool if necessary. Run and
enjoy!

To test, compile test.c. It will run a comparison between your system
allocator and nedalloc and tell you how much faster nedalloc is. It also
serves as an example of usage.

If you'd like nedmalloc as a Windows DLL or ELF shared object, the easiest
thing to do is to use scons (http://www.scons.org/) to build nedmalloc (or
use the enclosed MSVC project files).

Windows-only features:
-=-=-=-=-=-=-=-=-=-=-=
If you are running on Windows, there are quite a few extra options available
thanks to work generously sponsored by Applied Research Associates (USA).
Firstly you can build nedmalloc as a DLL and link that into your application
- this has the particular advantage that the DLL can trap thread exits in
your application and therefore call neddisablethreadcache() on all currently
existing nedpool's for you.

If you define REPLACE_SYSTEM_ALLOCATOR when building the DLL then the DLL
will replace most usage of the MSVCRT allocator within any process it is
loaded into with nedmalloc's routines instead, whilst remaining able to
handle the odd free() of a MSVCRT allocated block allocated during CRT init -
this very conveniently allows you to simply link with the nedmalloc DLL
and your application magically now uses it with no code changes required.
The following code is suggested:

#pragma comment(lib, "nedmalloc.lib") 

This auto-patching feature can also be combined with Microsoft's Detours
(http://research.microsoft.com/en-us/projects/detours/) to run any
arbitrary application using nedmalloc instead of the system allocator:

withdll /d:nedmalloc.dll program.exe

For those not able to use Microsoft Detours, there is an enclosed
nedmalloc_loader program which does one variant of the same thing. It may
or may not be useful to you - it is not intended to be maintained.

When building the nedmalloc DLL for the purposes of DLL insertion, you NEED
to match MSVCRT versions or you will have a CRT heap conflict. In other words,
if the program using nedmalloc is linked against MSVCRTD, then so must be
nedmalloc or vice versa. As a result of this issue, by default nedmalloc
ALWAYS LINKS TO MSVCRT EVEN IN DEBUG BUILDS unless configured otherwise.
This allows problem-free usage with release build applications which is
where nedmalloc tends to be most commonly deployed.

Lastly for some applications defining ENABLE_LARGE_PAGES can give a 10-15%
performance increase by having nedmalloc allocate using large pages only.
Large pages take much less space in the TLB cache and can greatly benefit
programs with a large working set. For this to work, your computer must
have the "Lock pages in memory" local security setting enabled for the
process' user as well as be running on Windows Server 2003/Vista or later.
If you are using the DLL then the DLL attempts to enable the
SeLockMemoryPrivilege during initialisation - therefore if you are not
using the DLL you will have to do this manually yourself.

B. Notes:
-=-=-=-=-
If you want the very latest version of this allocator, get it from the
TnFOX SVN repository at svn://svn.berlios.de/viewcvs/tnfox/trunk/src/nedmalloc

Because of how nedalloc allocates an mspace per thread, it can cause
severe bloating of memory usage under certain allocation patterns.
You can substantially reduce this wastage by setting MAXTHREADSINPOOL
or the threads parameter to nedcreatepool() to a fraction of the number of
threads which would normally be in a pool at once. This will reduce
bloating at the cost of an increase in lock contention. If allocated size
is less than THREADCACHEMAX, locking is avoided 90-99% of the time and
if most of your allocations are below this value, you can safely set
MAXTHREADSINPOOL to one.

You will suffer memory leakage unless you call neddisablethreadcache()
per pool for every thread which exits. This is because nedalloc cannot
portably know when a thread exits and thus when its thread cache can
be returned for use by other code. Don't forget pool zero, the system pool.
This of course is not an issue if you use nedmalloc as a DLL on Windows.
On some POSIX threads implementations there exists a pthread_atexit() which
registers a termination handler for thread exit - if you don't have one of
these then you'll have to do it manually.

Equally if you use nedmalloc from a dynamically loaded DLL or shared object
which you later kick out of memory, you will leak memory if you don't disable
all thread caches for all pools (as per the preceding paragraph), destroy all
thread pools using neddestroypool() and destroy the system pool using
neddestroysyspool().

For C++ type allocation patterns (where the same sizes of memory are
regularly allocated and deallocated as objects are created and destroyed),
the threadcache always benefits performance. If however your allocation
patterns are different, searching the threadcache may significantly slow
down your code - as a rule of thumb, if cache utilisation is below 80%
(see the source for neddisablethreadcache() for how to enable debug
printing in release mode) then you should disable the thread cache for
that thread. You can compile out the threadcache code by setting
THREADCACHEMAX to zero.

C. Speed comparisons:
-=-=-=-=-=-=-=-=-=-=-
See Benchmarks.xls for details.

The enclosed test.c can do two things: it can be a torture test or a speed
test. The speed test is designed to be a representative synthetic
memory allocator test. It works by randomly mixing allocations with frees
with half of the allocation sizes being a two power multiple less than
512 bytes (to mimic C++ stack instantiated objects) and the other half
being a simple random value less than 16Kb. 

The real world code results are from Tn's TestIO benchmark. This is a
heavily multithreaded and memory intensive benchmark with a lot of branching
and other stuff modern processors don't like so much. As you'll note, the
test doesn't show the benefits of the threadcache mostly due to the saturation
of the memory bus being the limiting factor.

D. Troubleshooting:
-=-=-=-=-==-=-=-=-=
I get a quite a few bug reports about code not working properly under nedmalloc.
I do not wish to sound presumptuous, however in an overwhelming majority of cases the
problem is in your application code and not nedmalloc (see below for all the bugs
reported and fixed since 2006). Some of the largest corporations and IT deployments
in the world use nedmalloc, and it has been very heavily stress tested on everything
from 32 processor SMP clusters right through to root DNS servers, ATM machine networks
and embedded operating systems requiring a very high uptime.

In particular, just because it just happens to appear to work under the system
allocator does not mean that your application is not riddled with memory corruption
and non-ANSI usage of the API! And usually this is not your code's fault, but rather
the third party libraries being used.

Even though debugging an application for memory errors is a true black art made
possible only with a great deal of patience, intuition and skill, here is a checklist
for things to do before reporting a bug in nedmalloc:

1. Make SURE you try nedmalloc from SVN HEAD. For around six months of 2007 I kept
getting the same report of a bug long fixed in SVN HEAD.

2. Make use of nedmalloc's internal debug routines. Try turning on full sanity
checks by #define FULLSANITYCHECKS 1. Also make use of all the assertion checking
performed when DEBUG is defined. A lot of bug reports are made before running under
a debug build where an assertion trip clearly showed the problem. Lastly, try
changing the thread cache by #defining THREADCACHEMAX - this fundamentally changes
how the memory allocator behaves: if everything is fine with the thread cache fully
on or fully off, then this strongly suggests the source of your problem.

3. Make SURE you are matching allocations and frees belonging to nedmalloc if you
are not defining REPLACE_SYSTEM_ALLOCATOR. Attempting to free a block not allocated
by nedmalloc will end badly, similarly passing one of nedmalloc's blocks to another
allocator will likely also end badly. I have inserted as many assertion and debug checks
for this possibility as I can think of (further suggestions are welcome), but no system
can ever be watertight. If you're using C++, I would use its strong template type
system to have the compiler guarantee membership of a memory pointer - see the Boost
libraries, or indeed my own TnFOX portability toolkit (http://www.nedprod.com/TnFOX/).

4. If you're still having problems, or more likely your code runs absolutely fine
under debug builds but trips up under release which suggests a timing bug, it is
time to deploy heavyweight tools. Under Linux, you should use valgrind. Under Windows,
there is an excellent commercial tool called Glowcode (http://www.glowcode.com/).
Any programming team serious on quality should ALWAYS run their projects through
these tools before each and every release anyway - you would be amazed at what you
miss during all other testing.

5. Lastly, in the worst case scenario, consider hiring in a memory debugging
expert. There are quite a few on the market and they often are authors of memory
allocators. Wolfram Gloger (the author of ptmalloc) provides consulting services.
My own consulting company ned Productions Ltd. may be able to provide such a
service depending on our current workload: see http://www.nedproductions.biz/.

I hope that these tips help. And I urge anyone considering simply dropping
back to the system allocator as a quick fix to reconsider: squashing memory bugs
often brings with it significant extra benefits in performance and reliability.
It may cost a lot extra now but it usually will save itself many times its cost
over the next few years. I know of one large multinational corporation who saved
hundreds of millions of dollars due to the debugging of their system software
performed when trying to get it working with nedmalloc - they found one bug in
nedmalloc but over a hundred in their own code, and in the process improved
performance threefold which saved an expensive hardware upgrade and deployment.
Fixing memory bugs now tends to be worth it in the long run.

E. ChangeLog:
-=-=-=-=-=-=-
v1.06 beta 2 21st March 2010:
 * { 1153 } Added detection of whether host process is using MSVCRT or MSVCRTD
and the fixing up of which runtime tolerant nedmalloc should use if nedmalloc
was linked differently. This ought to save a great deal of hassle later on by
preventing failed-to-RTM user bug reports :)
 * { 1154 } Fixed nedmalloc trying to use MLOCK_T even when USE_LOCKS=0. Thanks
to Ariel Manzur for reporting this.
 * { 1155 } Fixed USE_SPIN_LOCKS=0 not compiling on Windows.
 * { 1157 } Fixed bug where foreign blocks entering the threadcache weren't
being marked as such, thus typically causing a segfault on process exit.
 * { 1158 } Fixed compilation problems on mingw. Thanks to Amanieu d'Antras for
reporting these.
 * { 1159 } Released as beta2.

v1.06 beta 1 13th January 2010:
 * { 1079 } Fixed misdeclaration of struct mallinfo as C++ type. Thanks to
James Mansion for reporting this.
 * { 1082 } Fixed dlmalloc bug which caused header corruption to mmap()
allocations when running under multiple threads
 * { 1088 } Fixed assertion failure for nedblksize() with latest dlmalloc.
Thanks to Anteru for reporting this.
 * { 1088 } Added neddestroysyspool(). Thanks to Lars Wehmeyer for
suggesting this.
 * { 1088 } Fixed thread id high bit set bug causing SIGABRT on Mac OS X.
Thanks to Chris Dillman for reporting this.
 * { 1094 } Integrated dlmalloc v2.8.4 final.
 * { 1095 } Added nedtrimthreadcache(). Thanks to Hayim Hendeles for
suggesting this.
 * { 1095 } Fixed silly assertion of null pointer dereference. Thanks to
Ullrich Heinemann for reporting this.
 * { 1096 } Fixed lots of level 4 warnings on MSVC. Thanks to Anteru for
suggesting this.
 * { 1098 } Improved non-nedmalloc block detection to 6.25% probability
of being wrong. Thanks to Applied Research Associates for sponsoring this.
 * { 1099 } Added USE_MAGIC_HEADERS which allows nedmalloc to handle freeing
a system allocated block. Added USE_ALLOCATOR which allows the changing of
which backend allocator to use (with choices between the system allocator and
dlmalloc - choosing the system allocator is intended for debug situations only
e.g. valgrind). Thanks to Applied Research Associates for sponsoring this.
 * { 1105 } Added ability to build nedmalloc as a DLL. Added support for a run
time PE binary patcher which can patch all usage of the system allocator
replacing it with nedmalloc. Thanks to Applied Research Associates for
sponsoring this.
 * { 1108 } Added patcher loader which can load any arbitrary program
injecting the nedmalloc DLL which then patches in its replacement for the
system allocator. Doesn't work on all programs, but does on most e.g. Microsoft
Word. Thanks to Applied Research Associates for sponsoring this.
 * { 1116 } Finished debugging and optimising the latest additions to the
codebase. The patcher now works well on x64 as well as x86. Added support for
large pages on Windows. Thanks to Applied Research Associates for
sponsoring this.
 * { 1125 } Added nedpoollist() which returns a snapshot of the nedpool's
currently existing. The Windows DLL thread exit code now disables the thread
cache for all currently existing nedpool's. Thanks to Applied Research
Associates for sponsoring this.
 * { 1126 } Added ENABLE_TOLERANT_NEDMALLOC which allows nedmalloc to
recognise system allocator blocks and to do the right thing with them.
 * { 1139 } Added link time code generation support for Windows builds. This
currently has zero performance improvement on x64 (on MSVC9) but can add 15%
to x86 performance (on MSVC9). Also added scons SConstruct and SConscript files.

v1.05 15th June 2008:
 * { 1042 } Added error check for TLSSET() and TLSFREE() macros. Thanks to
Markus Elfring for reporting this.
 * { 1043 } Fixed a segfault when freeing memory allocated using
nedindependent_comalloc(). Thanks to Pavel Vozenilek for reporting this.

v1.04 14th July 2007:
 * Fixed a bug with the new optimised implementation that failed to lock
on a realloc under certain conditions.
 * Fixed lack of thread synchronisation in InitPool() causing pool corruption
 * Fixed a memory leak of thread cache contents on disabling. Thanks to Earl
Chew for reporting this.
 * Added a sanity check for freed blocks being valid.
 * Reworked test.c into being a torture test.
 * Fixed GCC assembler optimisation misspecification

v1.04alpha_svn915 7th October 2006:
 * Fixed failure to unlock thread cache list if allocating a new list failed.
Thanks to Dmitry Chichkov for reporting this. Futher thanks to Aleksey Sanin.
 * Fixed realloc(0, <size>) segfaulting. Thanks to Dmitry Chichkov for
reporting this.
 * Made config defines #ifndef so they can be overriden by the build system.
Thanks to Aleksey Sanin for suggesting this.
 * Fixed deadlock in nedprealloc() due to unnecessary locking of preferred
thread mspace when mspace_realloc() always uses the original block's mspace
anyway. Thanks to Aleksey Sanin for reporting this.
 * Made some speed improvements by hacking mspace_malloc() to no longer lock
its mspace, thus allowing the recursive mutex implementation to be removed
with an associated speed increase. Thanks to Aleksey Sanin for suggesting this.
 * Fixed a bug where allocating mspaces overran its max limit. Thanks to
Aleksey Sanin for reporting this.

v1.03 10th July 2006:
 * Fixed memory corruption bug in threadcache code which only appeared with >4
threads and in heavy use of the threadcache.

v1.02 15th May 2006:
 * Integrated dlmalloc v2.8.4, fixing the win32 memory release problem and
improving performance still further. Speed is now up to twice the speed of v1.01
(average is 67% faster).
 * Fixed win32 critical section implementation. Thanks to Pavel Kuznetsov
for reporting this.
 * Wasn't locking mspace if all mspaces were locked. Thanks to Pavel Kuznetsov
for reporting this.
 * Added Apple Mac OS X support.

v1.01 24th February 2006:
 * Fixed multiprocessor scaling problems by removing sources of cache sloshing
 * Earl Chew <earl_chew <at> agilent <dot> com> sent patches for the following:
   1. size2binidx() wasn't working for default code path (non x86)
   2. Fixed failure to release mspace lock under certain circumstances which
      caused a deadlock

v1.00 1st January 2006:
 * First release
