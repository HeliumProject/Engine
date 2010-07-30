#include "Foundation/InitializerStack.h"
#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Controls/Controls.h"

#include "Application/Inspect/Script.h"

#include "Application/Inspect/DragDrop/ClipboardDataWrapper.h"
#include "Application/Inspect/DragDrop/ClipboardElementArray.h"
#include "Application/Inspect/DragDrop/ClipboardFileList.h"
#include "Application/Inspect/DragDrop/ReflectClipboardData.h"

#include "Foundation/Reflect/Registry.h"

#pragma TODO( "Why is RegisterClass defined as RegisterClassA here? That is ridiculous." )
#undef RegisterClass

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
        g_InitializerStack.Push( Reflect::RegisterClass<Control>( TXT( "InspectControl" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Container>( TXT( "InspectContainer" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Group>( TXT( "InspectGroup" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Panel>( TXT( "InspectPanel" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Canvas>( TXT( "InspectCanvas" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClass<Button>( TXT( "InspectButton" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Action>( TXT( "InspectAction" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<CheckBox>( TXT( "InspectCheckBox" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<CheckList>( TXT( "InspectCheckList" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<ColorPicker>( TXT( "InspectColorPicker" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Label>( TXT( "InspectLabel" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<List>( TXT( "InspectList" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Slider>( TXT( "InspectSlider" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClass<Items>( TXT( "InspectItems" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Choice>( TXT( "InspectChoice" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClass<Value>( TXT( "InspectValue" ) ) );

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
