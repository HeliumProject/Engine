#include "Application/Inspect/Controls/InspectControl.h"
#include "Application/Inspect/Controls/InspectContainer.h"

#include <wx/dnd.h>

using namespace Helium;
using namespace Helium::Inspect;

#ifdef PROFILE_ACCUMULATION
Profile::Accumulator Inspect::g_RealizeAccumulator( "Inspect Realize Accumulator" );
#endif

Control::Control()
: m_IsEnabled( true )
, m_IsEnabledAttr( m_IsEnabled )
, m_IsReadOnly( false )
, m_IsReadOnlyAttr( m_IsReadOnly )
, m_IsFrozen( false )
, m_IsFrozenAttr( m_IsFrozen )
, m_IsHidden( false )
, m_IsHiddenAttr( m_IsHidden )
, m_ForegroundColor( 0 )
, m_ForegroundColorAttr( m_ForegroundColor )
, m_BackgroundColor( 0 )
, m_BackgroundColorAttr( m_BackgroundColor )
, m_IsFixedWidth( false )
, m_IsFixedWidthAttr( m_IsFixedWidth )
, m_IsFixedHeight( false )
, m_IsFixedHeightAttr( m_IsFixedHeight )
, m_ProportionalWidth( 0.f )
, m_ProportionalWidthAttr( m_ProportionalWidth )
, m_ProportionalHeight( 0.f )
, m_ProportionalHeightAttr( m_ProportionalHeight )
, m_DefaultAttr( m_Default )
, m_ToolTipAttr( m_ToolTip )
, m_Canvas( NULL )
, m_Parent( NULL )
, m_IsWriting( false )
, m_IsRealized( false )
{

}

Control::~Control()
{
    if (m_BoundData)
    {
        m_BoundData->RemoveChangedListener( DataChangedSignature::Delegate ( this, &Control::DataChanged ) );
    }

    m_IsRealized = false;
}

void Control::Create()
{

}

int Control::GetDepth()
{
    int depth = 0;
    Control* parent = m_Parent;

    while (parent != NULL)
    {
        depth++;
        parent = parent->m_Parent;
    }

    return depth;
}

void Control::Bind(const DataPtr& data)
{
    if ( !m_BoundData.ReferencesObject() || !data.ReferencesObject() )
    {
        if ( m_BoundData.ReferencesObject() )
        {
            m_BoundData->RemoveChangedListener( DataChangedSignature::Delegate ( this, &Control::DataChanged ) );
        }

        m_BoundData = data;

        if ( m_BoundData.ReferencesObject() )
        {
            m_BoundData->AddChangedListener( DataChangedSignature::Delegate ( this, &Control::DataChanged ) );
        }
    }
}

bool Control::IsDefault() const
{
    if (m_Default.empty() || m_BoundData == NULL)
    {
        return false;
    }

    StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );
    if ( data )
    {
        tstring val;
        data->Get(val);
        return m_Default == val;
    }

    HELIUM_BREAK(); // you need to HELIUM_OVERRIDE this, your control is using custom data
    return false;
}

bool Control::SetDefault()
{
    if (!m_Default.empty())
    {
        return WriteData(m_Default);
    }
    else
    {
        return false;
    }
}

bool Control::Process(const tstring& key, const tstring& value)
{
    if ( key == ATTR_TOOLTIP )
    {
        ToolTip() = value;

        return true;
    }

    return false;
}

const ContextMenuPtr& Control::GetContextMenu()
{
    return m_ContextMenu;
}

void Control::SetContextMenu(const ContextMenuPtr& contextMenu)
{
    m_ContextMenu = contextMenu;
}

int Control::GetStringWidth(const tstring& str)
{
#ifdef INSPECT_REFACTOR
    wxClientDC dc (m_Window);

    int x, y;
    wxString wxStr (str.c_str());
    dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

    return x;
#else
    return 128;
#endif
}

bool Control::EllipsizeString(tstring& str, int width)
{
#ifdef INSPECT_REFACTOR
    wxClientDC dc (m_Window);

    int x, y;
    wxString wxStr (str.c_str());
    dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

    if (x <= width)
    {
        return false;
    }

    size_t count = str.size();
    for ( size_t i = count; i>0; i-- )
    {
        wxStr = (str.substr(0, i-1) + TXT( "..." ) ).c_str();

        dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

        if (x < width)
        {
            str = wxStr.c_str();
            return true;
        }
    }

    str = TXT( "..." );
    return true;
#else
    return true;
#endif
}

bool Control::IsRealized()
{
    return m_IsRealized;
}

void Control::Realize(Container* parent)
{
    PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

    m_Parent = parent;
    m_IsRealized = true;

#pragma TODO("This will cause the canvas to allocate a widget specific to the type of canvas that is being used")
    m_Realized.Raise(this);
}

void Control::Unrealize()
{
    if ( m_Parent )
    {
        // If you hit this, you are trying to Unrealize a control that still belongs to
        // its parent.  Remove the control from its parent first, before calling this function.
        HELIUM_ASSERT( std::find( m_Parent->GetChildren().begin(), m_Parent->GetChildren().end(), ControlPtr(this) ) == m_Parent->GetChildren().end() );
    }

    m_IsRealized = false;
    m_Unrealized.Raise(this);
}

void Control::DataChanged(const DataChangedArgs& args)
{
    if (!m_IsWriting)
    {
        Read();
    }

    m_ControlChanged.Raise( this );
}

void Control::Read()
{
    SetDefaultAppearance(IsDefault());
}

bool Control::ReadData(tstring& str) const
{
    StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );
    if (data)
    {
        str.clear();
        data->Get( str );
        return true;
    }

    HELIUM_BREAK(); // you should not call this, your control is using custom data
    return false;
}

bool Control::ReadAll(std::vector< tstring >& strs) const
{
    StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );
    if ( data )
    {
        strs.clear();
        data->GetAll( strs );
        return true;
    }

    HELIUM_BREAK(); // you should not call this, your control is using custom data
    return false;
}

bool Control::PreWrite( Reflect::Serializer* newValue, bool preview )
{
    // check to see if a event handler bound to this control bypasses the write
    if ( !m_ControlChanging.RaiseWithReturn( ControlChangingArgs(this, newValue, preview) ) )
    {
        return false;
    }

    return true;
}

bool Control::Write()
{
    return true;
}

bool Control::WriteData(const tstring& str, bool preview)
{
    StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );

    return WriteTypedData(str, data, preview);
}

bool Control::WriteAll(const std::vector< tstring >& strs, bool preview)
{
    StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );
    if (data)
    {
        std::vector< tstring > currentValues;
        data->GetAll( currentValues );

        if ( strs == currentValues )
        {
            return true;
        }

        Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create< std::vector< tstring > >() );
        serializer->ConnectData( const_cast< std::vector< tstring >* >( &strs ) );
        if ( !PreWrite( serializer, preview ) )
        {
            Read();
            return false;
        }

        m_IsWriting = true;

        bool result = data->SetAll( strs );
        m_IsWriting = false;

        if (result)
        {
            PostWrite();
            return true;
        }
    }

    HELIUM_BREAK(); // you should not call this, your control is using custom data
    return false;
}

void Control::PostWrite()
{
    SetDefaultAppearance(IsDefault());

    // callback to our interpreter that we changed
    m_ControlChanged.Raise( this );

    // data validator could change our value, so re-read the value
    Read();
}
