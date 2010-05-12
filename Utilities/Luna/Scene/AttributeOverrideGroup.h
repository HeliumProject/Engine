#pragma once

#include "AttributeGroup.h"
#include "Common/Automation/Property.h"
#include "Inspect/Group.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Template class for making a UI group that contains a checkbox and a panel
  // displaying all members of a specific attribute type.  The checkbox is 
  // meant to enable and disable the sub-panel.
  // 
  template < class TAttrib, class TContainer >
  class AttributeOverrideGroup : public Inspect::Group
  {
  public:
    typedef bool ( TContainer::*Getter )() const;
    typedef void ( TContainer::*Setter )( bool );

  protected:
    typedef AttributeOverrideGroup< TAttrib, TContainer > ThisGroup;

  protected:
    Enumerator* m_Enumerator;
    OS_SelectableDumbPtr m_Selection;
    std::string m_LabelText;
    AttributeGroup< TAttrib >* m_AttributePanel;
    Inspect::CheckBox* m_CheckBox;
    Getter m_Get;
    Setter m_Set;
    

  public:
    ///////////////////////////////////////////////////////////////////////////
    // Constructor
    // 
    AttributeOverrideGroup( const std::string& checkBoxLabel, Enumerator* enumerator, const OS_SelectableDumbPtr& selection, Getter get, Setter set )
      : m_Selection( selection )
      , m_LabelText( checkBoxLabel )
      , m_AttributePanel( NULL )
      , m_CheckBox( NULL )
      , m_Get( get )
      , m_Set( set )
    {
      m_Interpreter = m_Enumerator = enumerator;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Destructor
    // 
    virtual ~AttributeOverrideGroup()
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    // Creates the control for the first time.
    // 
    virtual void Create() NOC_OVERRIDE
    {
      m_Enumerator->PushContainer();
      {
        m_Enumerator->AddLabel( m_LabelText );

        m_CheckBox = m_Enumerator->AddCheckBox< TContainer, bool >( m_Selection, m_Get, m_Set );
        m_CheckBox->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate( this, &ThisGroup::OnOverrideCheckBox ) );
      }
      m_Enumerator->Pop();

      Refresh( false );

      __super::Create();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Callback for when the checkbox is clicked.  Refreshes the contained panel
    // in case it needs to change its enable/expansion state.
    // 
    void OnOverrideCheckBox( const Inspect::ChangeArgs& args )
    {
      Refresh( true );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Enables or disables the inner panel as appropriate.
    // 
    void Refresh(bool layout)
    {
      Freeze();

      if ( m_CheckBox->GetChecked() && !m_AttributePanel )
      {
        m_AttributePanel = new AttributeGroup< TAttrib >( m_Interpreter, m_Selection );

        m_Enumerator->Push( m_AttributePanel );
        {
          m_AttributePanel->SetCanvas( m_Enumerator->GetContainer()->GetCanvas() );
          m_AttributePanel->Create();
        }
        m_Enumerator->Pop();
      }
      else if ( !m_CheckBox->GetChecked() && m_AttributePanel )
      {
        m_AttributePanel->GetParent()->RemoveControl( m_AttributePanel );
        m_AttributePanel = NULL;
      }

      if ( m_AttributePanel )
      {
        m_AttributePanel->Refresh();
      }

      if ( layout )
      {
        GetCanvas()->Layout();
      }

      Thaw();
    }
  };
}