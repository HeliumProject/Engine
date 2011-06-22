#include "FoundationPch.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Inspect/Inspect.h"
#include "Foundation/Inspect/Script.h"
#include "Foundation/Inspect/Controls.h"

#include "Foundation/Reflect/Registry.h"

using namespace Helium;
using namespace Helium::Inspect;

EditFilePathSignature::Event Inspect::g_EditFilePath;

static Helium::InitializerStack g_InspectInitStack;

void Inspect::Initialize()
{
    if ( g_InspectInitStack.Increment() == 1 )
    {
        g_InspectInitStack.Push( Reflect::RegisterClassType<Widget>( TXT( "Inspect::Widget" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<Control>( TXT( "Inspect::Control" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<Container>( TXT( "Inspect::Container" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<Canvas>( TXT( "Inspect::Canvas" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<Button>( TXT( "Inspect::Button" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<CheckBox>( TXT( "Inspect::CheckBox" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<ColorPicker>( TXT( "Inspect::ColorPicker" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<Label>( TXT( "Inspect::Label" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<List>( TXT( "Inspect::List" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<Slider>( TXT( "Inspect::Slider" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<FileDialogButton>( TXT( "Inspect::FileDialogButton" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<Choice>( TXT( "Inspect::Choice" ) ) );
        g_InspectInitStack.Push( Reflect::RegisterClassType<Value>( TXT( "Inspect::Value" ) ) );

        g_InspectInitStack.Push( Script::Initialize, Script::Cleanup );
    }
}

void Inspect::Cleanup()
{
    g_InspectInitStack.Decrement();
}
