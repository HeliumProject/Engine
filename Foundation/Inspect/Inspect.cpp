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
        g_InitializerStack.Push( Reflect::RegisterClassType<Widget>( TXT("Inspect::Widget") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Control>( TXT("Inspect::Control") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Container>( TXT("Inspect::Container") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Canvas>( TXT("Inspect::Canvas") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Button>( TXT("Inspect::Button") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<CheckBox>( TXT("Inspect::CheckBox") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<ColorPicker>( TXT("Inspect::ColorPicker") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Label>( TXT("Inspect::Label") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<List>( TXT("Inspect::List") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Slider>( TXT("Inspect::Slider") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<FileDialogButton>( TXT("Inspect::FileDialogButton") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Choice>( TXT("Inspect::Choice") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Value>( TXT("Inspect::Value") ) );

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
