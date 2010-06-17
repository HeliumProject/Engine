#include "ParametricKeyControl.h"
#include "Application/Inspect/Content/ParametricKeyData.h"
#include "Application/Inspect/Content/KeyControl.h"

#include "Application/Inspect/Widgets/Canvas.h"
#include "Application/Inspect/Widgets/Container.h"

#include "Pipeline/Content/ParametricKey/ParametricKey.h"

using namespace Inspect;

///////////////////////////////////////////////////////////////////////////////
// Color gradient widget.
// 
class InternalKeyControl : public KeyControl
{
private:
  ParametricKeyControl* m_KeyControl;
  i32 m_Override;

public:
  InternalKeyControl( wxWindow* parent, ParametricKeyControl* control )
    : KeyControl( parent )
    , m_KeyControl( control )
    , m_Override( 0 )
  {
    GetKeyArray()->AddFreezeListener( KeySignature::Delegate( this, &InternalKeyControl::OnFreeze ) );
    GetKeyArray()->AddThawListener( KeySignature::Delegate( this, &InternalKeyControl::OnThaw ) );
    GetKeyArray()->AddKeyCreatedListener( KeySignature::Delegate( this, &InternalKeyControl::OnChanged ) );
    GetKeyArray()->AddKeyDeletedListener( KeySignature::Delegate( this, &InternalKeyControl::OnChanged ) );
    GetKeyArray()->AddKeyMovingListener( KeySignature::Delegate( this, &InternalKeyControl::OnChanging ) );
    GetKeyArray()->AddKeyMovedListener( KeySignature::Delegate( this, &InternalKeyControl::OnChanged ) );
    GetKeyArray()->AddKeyColorChangedListener( KeySignature::Delegate( this, &InternalKeyControl::OnChanged ) );
  }

  void IncrementOverride()
  {
    m_Override++;
  }

  void DecrementOverride()
  {
    m_Override--;
  }

  void OnFreeze( const KeyArgs& args )
  {
    m_Override++;
  }

  void OnThaw( const KeyArgs& args )
  {
    NOC_ASSERT( m_Override > 0 );
    m_Override--;
    OnChanged( args );
  }

  void OnChanging( const KeyArgs& args )
  {
    if ( m_Override == 0 )
    {
      // Nothing to do yet...
    }
  }

  void OnChanged( const KeyArgs& args )
  {
    if ( m_Override == 0 )
    {
      m_KeyControl->Write();
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ParametricKeyControl::ParametricKeyControl()
: m_BlockRead( false )
{
  m_FixedHeight = true;
}

///////////////////////////////////////////////////////////////////////////////
// Creates the control.  Only call this function once.
// 
void ParametricKeyControl::Realize( Inspect::Container* parent )
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if (m_Window != NULL)
    return;

  m_Window = new InternalKeyControl( parent->GetWindow(), this );
  
  wxSize size( -1, m_Canvas->GetStdSize( Math::SingleAxes::Y ) );
  m_Window->SetSize( size );
  m_Window->SetMinSize( size );
  m_Window->SetMaxSize( size );

  __super::Realize(parent);
}

///////////////////////////////////////////////////////////////////////////////
// Reads information from the bound data and refreshes the UI.
// 
void ParametricKeyControl::Read()
{
  if ( m_BlockRead )
  {
    return;
  }

  if ( IsRealized() && IsBound() )
  {
    ParametricKeyData* data = CastData< ParametricKeyData, DataTypes::Custom >( GetData() );
    Content::V_ParametricKeyPtr keys;
    data->Get( keys );

    InternalKeyControl* control = Inspect::Control::Cast< InternalKeyControl >( this );
    control->IncrementOverride();
    control->GetKeyArray()->Clear();
    Content::V_ParametricKeyPtr::const_iterator itr = keys.begin();
    Content::V_ParametricKeyPtr::const_iterator end = keys.end();
    for ( ; itr != end; ++itr )
    {
      Content::ParametricKey* key = Reflect::AssertCast< Content::ParametricKey >( *itr );
      control->GetKeyArray()->CreateKey( key->GetColor(), key->m_Param );
    }
    control->DecrementOverride();
  }

  __super::Read();
}

///////////////////////////////////////////////////////////////////////////////
// Takes the information displayed in the UI and writes it back to the bound
// data.
// 
bool ParametricKeyControl::Write()
{
  bool result = false;
  m_BlockRead = true;

  if ( IsRealized() && IsBound() )
  {
    ParametricKeyData* data = CastData< ParametricKeyData, DataTypes::Custom >( GetData() );
    Reflect::V_Element keys;
    InternalKeyControl* control = Control::Cast< InternalKeyControl >( this );

    const u32 numKeys = control->GetKeyArray()->GetCount();
    for ( u32 keyIndex = 0; keyIndex < numKeys; ++keyIndex )
    {
      const Key* key = control->GetKeyArray()->GetKey( keyIndex );
      Content::ParametricKeyPtr paramKey = data->CreateParametricKey();
      if ( paramKey )
      {
        paramKey->SetColor( key->GetColor() );
        paramKey->m_Param = key->GetLocation();

        keys.push_back( paramKey );
      }
    }

    result = WriteTypedData< Reflect::V_Element >( keys, (DataTemplate< Reflect::V_Element >*)data ) && __super::Write();
  }

  m_BlockRead = false;
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the control is actually created.  Adjusts the height of this 
// control.
// 
void ParametricKeyControl::Create()
{
}
