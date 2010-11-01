#include "Foundation/InitializerStack.h"
#include "Foundation/Inspect/Inspect.h"
#include "Foundation/Inspect/Script.h"
#include "Foundation/Inspect/Controls.h"

#include "Foundation/Reflect/Registry.h"

using namespace Helium;
using namespace Helium::Inspect;

EditFilePathSignature::Event Inspect::g_EditFilePath;

namespace Helium
{
    namespace Inspect
    {
        int32_t g_InitCount = 0;
        Helium::InitializerStack g_InitializerStack;
    }
}

void Inspect::Initialize()
{
    if ( ++g_InitCount == 1 )
    {
        g_InitializerStack.Push( Reflect::RegisterClassType<Widget>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<Control>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<Container>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<Canvas>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<Button>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<CheckBox>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<ColorPicker>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<Label>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<List>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<Slider>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<FileDialogButton>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<Choice>() );
        g_InitializerStack.Push( Reflect::RegisterClassType<Value>() );

        g_InitializerStack.Push( Script::Initialize, Script::Cleanup );
    }
}

void Inspect::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitializerStack.Cleanup();
    }
}
