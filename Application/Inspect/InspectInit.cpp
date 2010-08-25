#include "Foundation/InitializerStack.h"
#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/InspectScript.h"
#include "Application/Inspect/InspectControls.h"
#include "Application/Inspect/Clipboard/ClipboardDataWrapper.h"
#include "Application/Inspect/Clipboard/ClipboardElementArray.h"
#include "Application/Inspect/Clipboard/ClipboardFileList.h"
#include "Application/Inspect/Clipboard/ReflectClipboardData.h"

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
        g_InitializerStack.Push( Reflect::RegisterClassType<Panel>( TXT( "InspectPanel" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Canvas>( TXT( "InspectCanvas" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Button>( TXT( "InspectButton" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Action>( TXT( "InspectAction" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<CheckBox>( TXT( "InspectCheckBox" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<ColorPicker>( TXT( "InspectColorPicker" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Label>( TXT( "InspectLabel" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<List>( TXT( "InspectList" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Slider>( TXT( "InspectSlider" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Items>( TXT( "InspectItems" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Choice>( TXT( "InspectChoice" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Value>( TXT( "InspectValue" ) ) );

        g_InitializerStack.Push( Script::Initialize, Script::Cleanup );

        // Drag and drop
        g_InitializerStack.Push( ReflectClipboardData::InitializeType, ReflectClipboardData::CleanupType );
        g_InitializerStack.Push( ClipboardDataWrapper::InitializeType, ClipboardDataWrapper::CleanupType );
        g_InitializerStack.Push( ClipboardElementArray::InitializeType, ClipboardElementArray::CleanupType );
        g_InitializerStack.Push( ClipboardFileList::InitializeType, ClipboardFileList::CleanupType );
    }
}

void Inspect::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitializerStack.Cleanup();
    }
}
