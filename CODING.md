Helium Coding Guidelines
========================

Naming Conventions
==================
In general, code should be as self documenting as possible. All names should be
composed of complete and descriptive words; words should not be shortened or
abbreviated except in rare and obvious (to the average developer) cases.

Namespaces
----------
Namespaces should be used sparingly to logically delineate code.  Namespace
names should start with a capital letter and use camel casing, eg:

```cpp
namespace ExampleNamespace
{
}
```

Structs and Classes
-------------------
Both structs and classes should using camel casing and begin with a capital
letter, eg:

```cpp
class ExampleClass
{
}

struct ExampleStruct
{
}
```

Enums
-----
* Enums should be placed in their own explicit namespace - so as not to contaminate the containing namespace with extraneous symbols
 * namespace name is plural - as the container for all of the enum symbols; the namespace name is used to refer to individual enum values as FileAccessFlags::Append rather than just Append
* enum – used to declare variables; the name is
 * plural if an enum variable may contain more than one of the enum flags: and cannot share the same name as the encapsulating namespace: FindFileFlags::FindFlags flags
 * singular if the enum values are mutually exclusive, allowing you to declare a variable that is ONE of many enum values: FileAccessFlags::FileMode mode
* enum member names should be CamelCase, with leading uppercase character; they should NOT be all caps or prefixed with a character indicator like “k_”
* use a typedef to shorten enum variable declaration – shortening FileAccessFlags::FileModes modes to FileModes modes; enum typedef shares the same name as the enum

Note: When you refer to an enum inside a type, you do not need to specify the name of the enum – which results in the following compiler warning:

```
 warning C4482: nonstandard extension used: enum 'enum' used in qualified name
```

```cpp
namespace FileAccessFlags
{
    enum FileAccessFlag
    {
        Read              = 1 << 0,
        Write             = 1 << 1,
        Append            = 1 << 2,
        Truncate          = 1 << 3,
        RandomAccess      = 1 << 4,
        SequentialAccess  = 1 << 5,
    };
}
typedef u32 FileAccessFlag;

void MyFunc( FileAccessFlag flags )
{
    if ( flags & FileAccessFlags::Append )
    {
        return OPEN_EXISTING;
    }
}

FileAccessFlag flags = ( FileAccessFlag ) ( FileAccessFlags::Append | FileAccessFlags::Write );
```
  
```cpp
namespace BrowserDisplayModes
{
    enum BrowserDisplayMode
    {
        Details = 0,
        List,
        Thumbnail,
    };
}
typedef BrowserDisplayModes::BrowserDisplayMode BrowserDisplayMode;

void MyFunc( BrowserDisplayMode modes )
{
    if ( modes & BrowserDisplayModes::Append )
    {
       return OPEN_EXISTING;
    }
}

BrowserDisplayMode modes = ( BrowserDisplayMode ) ( BrowserDisplayModes::Append | BrowserDisplayModes::Write );
```

Variables
---------

Local variables and function parameters should be camel case and begin with a lower case letter, eg:

```cpp
int localVariable = 0;
```

Member variables should be defined with the "m_" prefix and use camel case, beginning with a capital letter following the member prefix, eg:

```cpp
int m_MemberVariable;
```
    
Global variables, externalized from the source file, should be defined in the header files with the "g_" prefix, use camel case and begin with a capital letter following their prefix, eg:

```cpp
extern MODULE_API int g_GlobalVariable;
```

Static variables should be defined in the source file with the "s_" prefix, use camel case and begin with a capital letter following their prefix, eg:

```cpp
static const char* s_StaticVariable = "Hello world\n";
```
    
Constants
---------

Externalized constants, defined in the header file, should be made using either Enums (see above) or the C convention of #define, written in all caps, and underscore separated, eg:

```cpp
#define POUND_DEFINED_CONSTANT 1024
```
      
If a constant is never externalized from a source file, the C++ const modifier may be used instead, and the constant should be defined as a static variable (see above).

Macros
------

In general, C-style macros should not be used when it is possible to use a C++ inline function instead. Where C-style macros are necessary, they should be written in all caps with underscores to separate each word.  Additionally, to denote them as belonging to the Helium project, they should begin with the 'HELIUM_' prefix, eg:

```cpp
HELIUM_ASSERT( ... )
```

Files & Fodlers
---------------
A file should be named after the class that it contains, and placed under a
folder related to its functionality or the module it belongs to, eg:

```
  Module/Bar.h
```

```cpp
namespace Helium
{
    class Bar
    {
    public:
        Bar();
        ~Bar();

        void Func();
        ...
    }
}
```

```
  Module/Bar.cpp
```

```cpp
using namespace Helium;

Bar::Bar()
{
}

Bar~Bar()
{
}

void Bar::Func()
{
}
```

```
  Module/Subsystem/Blah.h
```

```cpp
namespace Helium
{
    class Blah
    {
    public:
        Blah();
        ~Blah();
    }
}
```

```
  Module/Subsystem/Blah.cpp
```

```cpp
using namespace Helium;

Blah::Blah()
{
}

Blah::~Blah();
{
}
```

Status/Error/Warning Message Formatting
=======================================

It is important that messages to the user (console printing, message box text, and exception message bodies) be homogeneous and in good form:

- Use complete sentences, and do not abbreviate words (eg: "vert", "jnt")
- Use periods. Phrases are bad for code that amend other module's messages.
- Don't use exclamation points. Errors are raised with color coding so they are unnecessary.
- Don't put code in messages. It only makes the messages harder to understand for users.
- *Under no circumstances* should a message tell the user to go tell a particular named programmer about a problem, or to get help directly from them. (eg: "Tell Reddy", "Grab Sam")
- All references to assets and files should be surrounded in single quotes (', not \`). 
- Do not use newline characters in exception message text.

Good
----

```cpp
throw Helium::Exception( "Triangle vertex limit exceeded. The limit is 256 triangles." );
```
  
Bad
---

```cpp
throw Helium::Exception( "Tri vert limit too far!!\nIncrease limit in TriPool.cpp (go tell Reddy or Fred)\n" );
throw Helium::Exception( "mTracks[AT_ROTATE_X]->isSampled() && mTracks[AT_ROTATE_Y]->isSampled() && mTracks[AT_ROTATE_Z]->isSampled()\n" );
```

Spacing, Tabs, Newlines, etc.
=============================

- Use spaces instead of tab characters.
- Use 4-space indenting.
- Place curly braces on their own line.
- Put spaces after open parentheses and before close parentheses.
- Put spaces after open brackets and before close brackets.
- Prefer to add spacing to code to help make it more easily readable.
- Append reference/pointer qualifier to the data type as opposed to the variable name, as it is more descriptive of the type of data that one expects to be in the variable as opposed to the variable itself.
- Do *not* attempt to align variable declarations or function declarations in classes using spaces, these tend to rot over time and are more trouble than they are worth.

```cpp
void SomeClass::SomeFunction( const OtherClass& otherClass, FooClass* foo )
{
    HELIUM_ASSERT( otherClass.IsOk() );
    
    if ( !otherClass.IsReady() )
    {
        otherClass.MakeReady();
    }
    
    int buffer[ 1234 ];
    otherClass.DoIt( &buffer, 1234 );
    
    foo->Process( &buffer, 1234 );
}
```
  
Variable Declarations
---------------------
  
Good
----

```cpp
int m_SomeInt;
float m_SomeFloat;

void SomeFunction();
int SomeOtherFunction();
```

Bad
---

```cpp
// this will rot when someone changes a type, or it will take someone
// unnecessary time when they add a new variable that changes the spacing
int   m_SomeInt; // note the unnecessary spaces following 'int' here
float m_SomeFloat;

void SomeFunction();
int  SomeOtherFunction(); // again, note the unnecessary spaces
```

File Paths
==========

All internal paths are expected to contain only forward slashes; however, they will be displayed to the user with the native platform's slashes.  This makes it easier for the user to cut/copy/paste the paths into a command prompt.

Internal folder paths should always have a trailing slash.

There are several functions in the File subsystem to help manage paths.  The Helium::Path object is the preferred way to work with a path (as opposed to using a bare string).  The Path object has a variety of useful functions and static methods for working with (and modifying) paths.

Hints for Writing Clean Class Declarations
==========================================

  - Write the header file to be easy to see what the interface is; write for humans reading your code to see how to use it.
  - Try to group similar functions (that work on similar data or support related functionality) next to each other.
  - Getter and setter functions should be adjoint.
  - Since we read top-to-bottom, put publicly accessible things at the top.

```cpp
class FOUNDATION_API Example : public Helium::RefCountBase< Example >
{
public: 
    Example();
    virtual ~Example();
  
    virtual bool IsCool();
    bool ConvertToInts( int* ints );
    void DoTheThing();

protected:
    virtual bool DoSomethingElse(); 

public:
    std::string m_SomeString;
    int32_t m_SomeInt;
  
private:
    int32_t m_SomePrivateInt;
  
    void Validate(); 
};
```