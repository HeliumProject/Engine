#include "Precompiled.h"
#include "DocumentPanel.h"

struct OutlineItemData : public wxTreeItemData
{
  Reflect::Element* m_Element;

  OutlineItemData(Reflect::Element* element)
    : m_Element( element )
  {

  }
};

BEGIN_EVENT_TABLE(DocumentPanel, wxPanel)
EVT_TREE_SEL_CHANGED(wxID_ANY, DocumentPanel::OnOutlineSelectionChanged)
END_EVENT_TABLE()

DocumentPanel::DocumentPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxPanel(parent, id, pos, size, style, name)
, m_Document (NULL)
, m_Outline (NULL)
{
  // outline
  m_Outline = new wxTreeCtrl ( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT );
  m_Root = m_Outline->AddRoot("INVISIBLE SANDWICH");

  // properties
  m_Canvas.SetControl( new Inspect::CanvasWindow( this, wxID_ANY, wxDefaultPosition, wxSize( 350, 350 ), wxALWAYS_SHOW_SB | wxCLIP_CHILDREN ) );
  m_Interpreter = new Inspect::ReflectInterpreter( &m_Canvas );

  m_PanelManager.SetManagedWindow( this ); 
  m_PanelManager.AddPane( m_Outline, wxAuiPaneInfo().Name( wxT( "outline" ) ).CloseButton( false ).Caption( wxT( "Outline" ) ).CenterPane() );
  m_PanelManager.AddPane( m_Canvas.GetControl(), wxAuiPaneInfo().Name( wxT( "properties" ) ).CloseButton( false ).Caption( wxT( "Properties" ) ).Right().Layer( 2 ).Position( 1 ).MinSize( wxSize (80, 80) ) );
  m_PanelManager.Update();
}

DocumentPanel::~DocumentPanel()
{
  m_PanelManager.UnInit();

  SetDocument(NULL);
}

Document* DocumentPanel::GetDocument()
{
  return m_Document;
}

void DocumentPanel::SetDocument(Document* document)
{
  if (m_Document == document)
  {
    return;
  }

  if (m_Document)
  {
    m_Document->GetTitleChangedEvent().Remove( TitleSignature::Delegate ( this, &DocumentPanel::OnTitleChanged ) );
    m_Document->GetElementAddedEvent().Remove( ElementSignature::Delegate ( this, &DocumentPanel::OnElementAdded ) );
    m_Document->GetElementRemovedEvent().Remove( ElementSignature::Delegate ( this, &DocumentPanel::OnElementRemoved ) );
    m_Document->GetSelectionChangedEvent().Remove( SelectionSignature::Delegate ( this, &DocumentPanel::OnSelectionChanged ) );
  }

  m_Document = document;

  if (m_Document)
  {
    m_Outline->Freeze();

    Reflect::V_Element::const_iterator itr = m_Document->GetElements().begin();
    Reflect::V_Element::const_iterator end = m_Document->GetElements().end();
    for ( ; itr != end; ++itr )
    {
      OnElementAdded( ElementArgs( *itr ) );
    }

    m_Outline->Thaw();

    m_Document->GetTitleChangedEvent().Add( TitleSignature::Delegate ( this, &DocumentPanel::OnTitleChanged ) );
    m_Document->GetElementAddedEvent().Add( ElementSignature::Delegate ( this, &DocumentPanel::OnElementAdded ) );
    m_Document->GetElementRemovedEvent().Add( ElementSignature::Delegate ( this, &DocumentPanel::OnElementRemoved ) );
    m_Document->GetSelectionChangedEvent().Add( SelectionSignature::Delegate ( this, &DocumentPanel::OnSelectionChanged ) );
  }
}

void DocumentPanel::OnOutlineSelectionChanged( wxTreeEvent& event )
{
  OutlineItemData* data = static_cast<OutlineItemData*>( m_Outline->GetItemData( event.GetItem() ) );

  if ( data )
  {
    Reflect::V_Element selection;
    selection.push_back( data->m_Element );
    m_Document->SetSelection( selection );
  }
}

void DocumentPanel::OnTitleChanged( const TitleArgs& args )
{
  m_TitleChanged.Raise( args );
}

void DocumentPanel::OnElementAdded( const ElementArgs& args )
{
  class DocumentVisitor : public Reflect::Visitor
  {
  public:
    DocumentVisitor( DocumentPanel* panel )
      : m_Panel ( panel )
    {
      m_Root.push( m_Panel->m_Root );
    }

    bool VisitElement(Reflect::Element* element)
    {
      if ( m_Visited.insert( element ).second )
      {
        // name
        const std::string& name = element->GetClass()->m_ShortName;

        // push this item into the outline
        m_Root.push( m_Panel->m_IDs[ element ] = m_Panel->m_Outline->AppendItem( m_Root.top(), name, -1, -1, new OutlineItemData ( element ) ) );

        // manually control the visitor traversal (return false below)
        element->Host( *this );

        // pop our node off the stack
        m_Root.pop();

        // don't recurse (we handled the recursion ourself)
        return false;
      }

      // this handles the redundant call to Host above (skip the first object)
      return true;
    }

    bool VisitField(Reflect::Element* element, const Reflect::Field* field)
    {
      // for fields the element is a serializer to the field
      Reflect::SerializerPtr serializer = field->CreateSerializer( element );

      bool elementPointer = serializer->HasType( Reflect::GetType< Reflect::PointerSerializer >() );
      bool elementArray = serializer->HasType( Reflect::GetType< Reflect::ElementArraySerializer >() );
      bool elementSet = serializer->HasType( Reflect::GetType< Reflect::ElementSetSerializer >() );
      bool elementMap = serializer->HasType( Reflect::GetType< Reflect::ElementMapSerializer >() );
      if ( elementPointer || elementArray || elementSet || elementMap )
      {
        // push this item into the outline
        m_Root.push( m_Panel->m_IDs[ serializer ] = m_Panel->m_Outline->AppendItem( m_Root.top(), field->m_Name, -1, -1, new OutlineItemData ( serializer ) ) );

        // manually control the visitor traversal (return false below)
        serializer->Host( *this );

        // pop our node off the stack
        m_Root.pop();

        // don't recurse (we handled the recursion ourself)
        return false;
      }

      // its a pointer or some other field that doesn't need a tree node,
      //  just let the visitor handle it (it will call another visit function)
      return true;
    }

  private:
    DocumentPanel*              m_Panel;
    std::stack<wxTreeItemId>    m_Root;
    std::set<Reflect::Element*> m_Visited;
  };

  args.m_Element->Host( DocumentVisitor (this) );
}

void DocumentPanel::OnElementRemoved( const ElementArgs& args )
{
  m_Outline->Delete( m_IDs[ args.m_Element ] );
}

void DocumentPanel::OnSelectionChanged( const SelectionArgs& args )
{
  m_Canvas.Clear();

  m_Interpreter->Interpret( (const std::vector<Reflect::Element*>&) args.m_Selection );

  m_Canvas.Layout();

  m_Canvas.Read();
}
