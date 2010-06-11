#include "StdAfx.h"
#include "Foundation/InitializerStack.h"
#include "InspectInit.h"

#include "Control.h"
#include "Container.h"
#include "Group.h"
#include "Panel.h"
#include "Canvas.h"

#include "Action.h"
#include "Button.h"
#include "CheckBox.h"
#include "CheckList.h"
#include "ColorPicker.h"
#include "Label.h"
#include "List.h"
#include "Slider.h"

#include "Choice.h"
#include "Items.h"
#include "Value.h"

#include "Script.h"

#include "ClipboardDataWrapper.h"
#include "ClipboardElementArray.h"
#include "ClipboardFileList.h"
#include "ReflectClipboardData.h"

using namespace Inspect;

EditFilePathSignature::Event Inspect::g_EditFilePath;

namespace Inspect
{
    i32 g_InitCount = 0;
    Nocturnal::InitializerStack g_InitializerStack;
}

void Inspect::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Reflect::RegisterClass<Control>( "InspectControl" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Container>( "InspectContainer" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Group>( "InspectGroup" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Panel>( "InspectPanel" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Canvas>( "InspectCanvas" ) );

    g_InitializerStack.Push( Reflect::RegisterClass<Button>( "InspectButton" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Action>( "InspectAction" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<CheckBox>( "InspectCheckBox" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<CheckList>( "InspectCheckList" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<ColorPicker>( "InspectColorPicker" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Label>( "InspectLabel" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<List>( "InspectList" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Slider>( "InspectSlider" ) );

    g_InitializerStack.Push( Reflect::RegisterClass<Items>( "InspectItems" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Choice>( "InspectChoice" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Value>( "InspectValue" ) );

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
