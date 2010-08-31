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
        i32 g_InitCount = 0;
        Helium::InitializerStack g_InitializerStack;
    }
}

void Inspect::Initialize()
{
    if ( ++g_InitCount == 1 )
    {
        g_InitializerStack.Push( Reflect::RegisterClassType<Control>( TXT( "InspectControl" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Container>( TXT( "InspectContainer" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Canvas>( TXT( "InspectCanvas" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Button>( TXT( "InspectButton" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<CheckBox>( TXT( "InspectCheckBox" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<ColorPicker>( TXT( "InspectColorPicker" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Label>( TXT( "InspectLabel" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<List>( TXT( "InspectList" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Slider>( TXT( "InspectSlider" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<FileDialogButton>( TXT( "FileDialogButton" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Choice>( TXT( "InspectChoice" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Value>( TXT( "InspectValue" ) ) );

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
