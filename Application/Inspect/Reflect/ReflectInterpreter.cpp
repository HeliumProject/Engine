#include "ReflectInterpreter.h"
#include "Application/Inspect/Reflect/ReflectValueInterpreter.h"

#include "Application/Inspect/Script.h"
#include "Application/Inspect/Widgets/Button Controls/InspectButton.h"
#include "Application/Inspect/Widgets/Text Controls/Value.h"
#include "Application/Inspect/Widgets/Other Controls/List.h"
#include "Application/Inspect/Widgets/InspectPanel.h"

using namespace Reflect;
using namespace Inspect;
using namespace Inspect;

ReflectFieldInterpreterFactory::M_Creator ReflectFieldInterpreterFactory::m_Map;

ReflectInterpreter::ReflectInterpreter (Container* container)
: Interpreter (container)
{

}

void ReflectInterpreter::Reset()
{
  // Clear out the controls that belong to this interpreter.
  V_Control controlsToRemove;
  V_Control::const_iterator itr = m_Container->GetControls().begin();
  V_Control::const_iterator end = m_Container->GetControls().end();
  for ( ; itr != end; ++itr )
  {
    Control* control = *itr;
    if ( control->GetInterpreter() == this )
    {
      controlsToRemove.push_back( control );
    }
  }

  V_Control::iterator removeItr = controlsToRemove.begin();
  V_Control::iterator removeEnd = controlsToRemove.end();
  for ( ; removeItr != removeEnd; ++removeItr )
  {
    m_Container->RemoveControl( *removeItr );
  }

  m_Instances.clear();
  m_Serializers.clear();
  m_Interpreters.clear();
}

void ReflectInterpreter::Interpret(const std::vector<Reflect::Element*>& instances, i32 includeFlags, i32 excludeFlags, bool expandPanel)
{
  m_Instances = instances;

  InterpretType(instances, m_Container, includeFlags, excludeFlags, expandPanel);
}

void ReflectInterpreter::InterpretType(const std::vector<Reflect::Element*>& instances, Container* parent, i32 includeFlags, i32 excludeFlags, bool expandPanel)
{
  const Class* typeInfo = instances[0]->GetClass();
  
  // create a panel
  PanelPtr panel = m_Container->GetCanvas()->Create<Panel>(this);

  // parse
  ContainerPtr scriptOutput = m_Container->GetCanvas()->Create<Container>(this);

  tstring typeInfoUI;
  typeInfo->GetProperty( "UIScript", typeInfoUI );
  bool result = Script::Parse(typeInfoUI, this, parent->GetCanvas(), scriptOutput);

  // compute panel label
  std::string labelText;
  if (result)
  {
    V_Control::const_iterator itr = scriptOutput->GetControls().begin();
    V_Control::const_iterator end = scriptOutput->GetControls().end();
    for( ; itr != end; ++itr )
    {
      Label* label = Reflect::ObjectCast<Label>( *itr );
      if (label)
      {
          bool converted = Platform::ConvertString( label->GetText(), labelText );
          NOC_ASSERT( converted );
            
        if ( !labelText.empty() )
        {
          break;
        }
      }
    }
  }

  if (labelText.empty())
  {
    std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Element*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
      Reflect::Element* instance = *itr;

      if ( labelText.empty() )
      {
        labelText = instance->GetTitle();
      }
      else
      {
        if ( labelText != instance->GetTitle() )
        {
          labelText.clear();
          break;
        }
      }
    }

    if ( labelText.empty() )
    {
      labelText = typeInfo->m_UIName;
    }
  }

  tstring temp;
  bool converted = Platform::ConvertString( labelText, temp );
  NOC_ASSERT( converted );

  panel->SetText( temp );

  M_Panel panelsMap;
  panelsMap.insert( std::make_pair("", panel) );

  // don't bother including Element's fields
  int offset = Reflect::GetClass<Element>()->m_LastFieldID;

  // for each field in the type
  M_FieldIDToInfo::const_iterator itr = typeInfo->m_FieldIDToInfo.find(offset + 1);
  M_FieldIDToInfo::const_iterator end = typeInfo->m_FieldIDToInfo.end();
  for ( ; itr != end; ++itr )
  {
    const Field* field = itr->second;

    bool noFlags = ( field->m_Flags == 0 && includeFlags == 0xFFFFFFFF );
    bool doInclude = ( field->m_Flags & includeFlags ) != 0;
    bool dontExclude = ( excludeFlags == 0 ) || !(field->m_Flags & excludeFlags );
    bool hidden = (field->m_Flags & Reflect::FieldFlags::Hide) != 0; 

    // if we don't have flags (or we are included, and we aren't excluded) then make UI
    if ( ( noFlags || doInclude ) && ( dontExclude ) )
    {
      //
      // Handle sub panels for grouping content
      // 

      bool groupExpanded = false;
      field->GetProperty( "UIGroupExpanded", groupExpanded );

      tstring fieldUIGroup;
      field->GetProperty( "UIGroup", fieldUIGroup );
      if ( !fieldUIGroup.empty() )
      {
        M_Panel::iterator itr = panelsMap.find( fieldUIGroup );
        if ( itr == panelsMap.end() )
        {
          // This panel isn't in our list so make a new one
          PanelPtr newPanel = m_Container->GetCanvas()->Create<Panel>(this);
          panelsMap.insert( std::make_pair(fieldUIGroup, newPanel) );

          PanelPtr parent;
          tstring groupName;
          size_t idx = fieldUIGroup.find_last_of( TXT( "/" ) );
          if ( idx != tstring::npos )
          {
            tstring parentName = fieldUIGroup.substr( 0, idx );
            groupName = fieldUIGroup.substr( idx+1 );
            if ( panelsMap.find( parentName ) == panelsMap.end() )
            {          
              parent = m_Container->GetCanvas()->Create<Panel>(this);

              // create the parent hierarchy since it hasn't already been made
              tstring currentParent = parentName;
              for (;;)
              {
                idx = currentParent.find_last_of( TXT( "/" ) );
                if ( idx == tstring::npos )
                {
                  // no more parents so we add it to the root
                  panelsMap.insert( std::make_pair(currentParent, parent) );
                  parent->SetText( currentParent );
                  panelsMap[ TXT( "" ) ]->AddControl( parent );
                  break;
                }
                else
                {
                  parent->SetText( currentParent.substr( idx+1 ) );
                  
                  if ( panelsMap.find( currentParent ) != panelsMap.end() )
                  {
                    break;
                  }
                  else
                  {
                    PanelPtr grandParent = m_Container->GetCanvas()->Create<Panel>(this);
                    grandParent->AddControl( parent );
                    panelsMap.insert( std::make_pair(currentParent, parent) );
                    
                    parent = grandParent;
                  }
                  currentParent = currentParent.substr( 0, idx );
                }
              }
              panelsMap.insert( std::make_pair(parentName, parent) );
            }
            parent = panelsMap[parentName];
          }
          else
          {
            parent = panelsMap[ TXT( "" )];
            groupName = fieldUIGroup;
          }
          newPanel->SetText( groupName );
          if( groupExpanded )
          {
            newPanel->SetExpanded( true );
          }
          parent->AddControl( newPanel );
        }
        
        panel = panelsMap[fieldUIGroup];
      }
      else
      {
        panel = panelsMap[ TXT( "" )];
      }


      //
      // Pointer support
      //

      if (field->m_SerializerID == Reflect::GetType<Reflect::PointerSerializer>())
      {
        if (hidden)
        {
          continue; 
        }        

        std::vector<Reflect::Element*> fieldInstances;

        std::vector<Reflect::Element*>::const_iterator elementItr = instances.begin();
        std::vector<Reflect::Element*>::const_iterator elementEnd = instances.end();
        for ( ; elementItr != elementEnd; ++elementItr )
        {
          uintptr fieldAddress = (uintptr)(*elementItr) + itr->second->m_Offset;

          Element* element = *((ElementPtr*)(fieldAddress));

          if ( element )
          {
            fieldInstances.push_back( element );
          }
        }

        if ( !fieldInstances.empty() && fieldInstances.size() == instances.size() )
        {
          InterpretType(fieldInstances, panel);
        }

        continue;
      }


      //
      // Attempt to find a handler via the factory
      //

      ReflectFieldInterpreterPtr fieldInterpreter;

      for ( const Reflect::Class* type = Registry::GetInstance()->GetClass( field->m_SerializerID );
            type != Reflect::GetClass<Reflect::Element>() && !fieldInterpreter;
            type = Reflect::Registry::GetInstance()->GetClass( type->m_Base ) )
      {
        fieldInterpreter = ReflectFieldInterpreterFactory::Create( type->m_TypeID, field->m_Flags, m_Container );
      }

      if ( fieldInterpreter.ReferencesObject() )
      {
        Interpreter::ConnectInterpreterEvents( this, fieldInterpreter );
        fieldInterpreter->InterpretField( field, instances, panel );
        m_Interpreters.push_back( fieldInterpreter );
        continue;
      }


      //
      // ElementArray support
      //

#pragma TODO("Move this out to an interpreter")
      if (field->m_SerializerID == Reflect::GetType<ElementArraySerializer>())
      {
        if (hidden)
        {
          continue;
        }

        if ( instances.size() == 1 )
        {
          uintptr fieldAddress = (uintptr)(instances.front()) + itr->second->m_Offset;

          V_Element* elements = (V_Element*)fieldAddress;

          if ( elements->size() > 0 )
          {
            PanelPtr childPanel = panel->GetCanvas()->Create<Panel>( this );

               tstring temp;
              bool converted = Platform::ConvertString( field->m_UIName, temp );
              NOC_ASSERT( converted );

              childPanel->SetText( temp );

            V_Element::const_iterator elementItr = elements->begin();
            V_Element::const_iterator elementEnd = elements->end();
            for ( ; elementItr != elementEnd; ++elementItr )
            {
              std::vector<Reflect::Element*> childInstances;
              childInstances.push_back(*elementItr);
              InterpretType(childInstances, childPanel);
            }

            panel->AddControl( childPanel );
          }
        }

        continue;
      }


      //
      // Lastly fall back to the value interpreter
      //

      const Reflect::Class* type = Registry::GetInstance()->GetClass( field->m_SerializerID );
      if ( !type->HasType( Reflect::GetType<Reflect::ContainerSerializer>() ) )
      {
        fieldInterpreter = CreateInterpreter< ReflectValueInterpreter >( m_Container );
        fieldInterpreter->InterpretField( field, instances, panel );
        m_Interpreters.push_back( fieldInterpreter );
        continue;
      }
    }
  }

  // Make sure we have the base panel
  panel = panelsMap[TXT( "" )];

  if (parent == m_Container)
  {
    panel->SetExpanded(expandPanel);
  }

  if ( !panel->GetControls().empty() )
  {
    parent->AddControl(panel);
  }
}


void ReflectFieldInterpreterFactory::Register(i32 type, u32 mask, Creator creator)
{
  m_Map[ type ].push_back( std::make_pair(mask, creator) );
}

void ReflectFieldInterpreterFactory::Unregister(i32 type, u32 mask, Creator creator)
{
  M_Creator::iterator found = m_Map.find( type );
  if ( found != m_Map.end() )
  {
    std::remove( found->second.begin(), found->second.end(), std::make_pair( mask, creator ) );
  }
}

ReflectFieldInterpreterPtr ReflectFieldInterpreterFactory::Create(i32 type, u32 flags, Container* container)
{
  Creator creator = NULL;

  M_Creator::const_iterator found = m_Map.find( type );
  if ( found != m_Map.end() )
  {
    std::map<u32, Creator> results;

    V_Creator::const_iterator itr = found->second.begin();
    V_Creator::const_iterator end = found->second.end();
    for ( ; itr != end; ++itr )
    {
      if ( flags == itr->first ) // exact match
      {
        creator = itr->second;
      }
      else if ( flags && (flags & itr->first) != 0 ) // it has flags, and at least on flag is in the mask
      {
        u32 value = flags & itr->first;

        // count the number of bits set
        u32 bits = 0;
        while (value)
        {
          value = value & (value - 1);
          bits++;
        }

        // rank it by the number of matching flags
        results[ bits ] = itr->second;
      }
      else if ( itr->first == 0 ) // fall back to folks that don't care about flags
      {
        // rank it at the bottom
        results[ 0x0 ] = itr->second;
      }
    }

    if ( !results.empty() )
    {
      creator = results.rbegin()->second;
    }
  }

  if ( creator )
  {
    return creator( container );
  }
  else
  {
    return NULL;
  }
}

void ReflectFieldInterpreterFactory::Clear()
{
  m_Map.clear();
}