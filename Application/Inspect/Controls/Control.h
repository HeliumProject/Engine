#pragma once

#include "Application/API.h"
#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Interpreter.h"
#include "Application/Inspect/Controls/ContextMenu.h"

#include "Foundation/Math/Point.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Object.h"

#include <wx/dnd.h>

namespace Inspect
{
  // the scroll increment in pixels
  const static int SCROLL_INCREMENT = 8;

  const static tchar ATTR_VALUE_TRUE[] = TXT( "true" );
  const static tchar ATTR_VALUE_FALSE[] = TXT( "false" );
  const static tchar ATTR_TOOLTIP[] = TXT( "tooltip" );

#ifdef PROFILE_ACCUMULATION
  APPLICATION_API extern Profile::Accumulator g_RealizeAccumulator;
#endif

  //
  // Attribute
  //

  class APPLICATION_API ControlAttribute
  {
  public:
    tstring m_Key;
    tstring m_Value;

    ControlAttribute ()
    {

    }

    ControlAttribute (const tstring& key, const tstring& value)
    {
      m_Key = key;
      m_Value = value;
    }
  };

  // standard container
  typedef std::vector<ControlAttribute> V_ControlAttribute;


  //
  // Control
  //

  class APPLICATION_API Control : public Reflect::AbstractInheritor<Control, Reflect::Element>
  {
  protected:
    // the canvas that implements us
    Canvas* m_Canvas;

    // the parent
    Container* m_Parent;

    // the interpreter that created us
    InterpreterPtr m_Interpreter;

    // the data we manipulate
    DataPtr m_BoundData;

    // writing flag
    bool m_Writing;

    // the default value
    tstring m_Default;

    // client-configurable data
    Reflect::ObjectPtr m_ClientData;

    // are we fixed along an axis?
    bool m_FixedWidth;
    bool m_FixedHeight;

    // are we proportional along an axis?
    f32 m_ProportionalWidth;
    f32 m_ProportionalHeight;

    // our colors for appearange
    int m_ForeColor;
    int m_BackColor;

    // are we enabled?
    bool m_Enabled;

    // are we writable?
    bool m_ReadOnly;

    // our context menu, if any
    ContextMenuPtr m_ContextMenu;

    // our drop target
    wxDropTarget* m_DropTarget;

    // a handle to the control
    wxWindow* m_Window;

    // have we really fully realized?
    bool m_Realized;

    // the tool tip for this control
    tstring m_ToolTip;


    //
    // Construction
    //

  public:
    Control();
    virtual ~Control();

    // created
    virtual void Create();


    //
    // Accessors
    //

    int GetDepth();

    Canvas* GetCanvas()
    {
      return m_Canvas;
    }

    void SetCanvas(Canvas* canvas)
    {
      m_Canvas = canvas;
    }

    Container* GetParent()
    {
      return m_Parent;
    }

    Interpreter* GetInterpreter()
    {
      return m_Interpreter;
    }

    void SetInterpreter(Interpreter* interpreter)
    {
      m_Interpreter = interpreter;
    }


    //
    // Data Binding
    //

    // query
    const DataPtr& GetData();

    // performs binding to specified data
    virtual void Bind(const DataPtr& data);
    virtual bool IsBound() const;


    // 
    // Client data
    // 

    Reflect::Object* GetClientData()
    {
      return m_ClientData;
    }
    void SetClientData( Reflect::Object* clientData )
    {
      m_ClientData = clientData;
    }


    //
    // Defaults
    //

    // sets default data
    virtual void SetDefault(const tstring& def);

    // sets data back to default
    virtual bool SetDefault();

    // queries if value is at default
    virtual bool IsDefault() const;

    // updates control appearance to appear to be at default value
    virtual void SetDefaultAppearance(bool def);


    //
    // Attributes
    //

    // process individual attribute key
    virtual bool Process(const tstring& key, const tstring& value);


    //
    // Arrangement
    //

    bool IsFixedWidth() const
    {
      return m_FixedWidth;
    }

    bool IsFixedHeight() const
    {
      return m_FixedHeight;
    }

    f32 GetProportionalWidth() const
    {
      return m_ProportionalWidth;
    }
    void SetProportionalWidth(f32 proportion)
    {
      m_ProportionalWidth = proportion;
    }

    f32 GetProportionalHeight() const
    {
      return m_ProportionalHeight;
    }
    void SetProportionalHeight(f32 proportion)
    {
      m_ProportionalHeight = proportion;
    }


    //
    // Color, ARGB
    //

    virtual int GetForeColor();
    virtual void SetForeColor(int color);

    virtual int GetBackColor();
    virtual void SetBackColor(int color);


    //
    // Enabled
    //

    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool enabled);


    //
    // ReadOnly
    //

    virtual bool IsReadOnly() const;
    virtual void SetReadOnly(bool readOnly);


    //
    // Context Menu
    //

    virtual const ContextMenuPtr& GetContextMenu();
    virtual void SetContextMenu(const ContextMenuPtr& contextMenu);


    //
    // Drop Target
    //

    virtual wxDropTarget* GetDropTarget() const;
    virtual void SetDropTarget( wxDropTarget* dropTarget );



    //
    // String
    //

    int GetStringWidth(const tstring& str);
    virtual bool TrimString(tstring& str, int width);

    //
    // Tool Tip
    //
    const tstring& GetToolTip();
    virtual void SetToolTip( const tstring& toolTip );


    //
    // Init and Layout
    //

    // get actual control window
    wxWindow* GetWindow()
    {
      return m_Window;
    }
    const wxWindow* GetWindow() const
    {
      return m_Window;
    }

    virtual wxWindow* GetContextWindow()
    {
      wxWindow* window = GetWindow();
      return window;
    }
    virtual const wxWindow* GetContextWindow() const
    {
      const wxWindow* window = GetWindow();
      return window;
    }
    
    // checks for initialization status
    virtual bool IsRealized();

    // creates the canvas control, called during layout
    virtual void Realize(Container* parent);

    // unrealizes the control
    virtual void UnRealize();


    //
    // Layout
    //

  public:
    // populated cachend UI state (drop down lists, etc)
    virtual void Populate() { }

    // invalidate (force repaint)
    virtual void Invalidate();

    // Freeze/Thaw
    virtual void Freeze();
    virtual void Thaw();

    // Show/Hide
    virtual void Show();
    virtual void Hide();


    //
    // Read
    //

    // refreshes the UI state from data
    virtual void Read();

    // helper read call for string based controls
    virtual bool ReadData(tstring& str) const;

    // helper read call to get values of all bound data
    virtual bool ReadAll(std::vector< tstring >& strs) const;

    // callback when data changed, implements DataReference
    virtual void DataChanged(const DataChangedArgs& args);


    //
    // Write
    //

    // fires callback
    virtual bool PreWrite( const Reflect::SerializerPtr& newValue, bool preview );

    // updates the data based on the state of the UI
    virtual bool Write();

    // helper write call for string based controls
    bool WriteData(const tstring& str, bool preview = false);

    template<class T>
    bool WriteTypedData(const T& val, const typename DataTemplate<T>::Ptr& data, bool preview = false)
    {
      if (data)
      {
        T currentValue;
        data->Get( currentValue );
        if ( val == currentValue )
        {
          return true;
        }

        Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create<T>() );
        serializer->ConnectData( const_cast< T* >( &val ) );
        if ( !PreWrite( serializer, preview ) )
        {
          Read();
          return false;
        }

        m_Writing = true;
        bool result = data->Set( val );
        m_Writing = false;

        if (result)
        {
          PostWrite();
          return true;
        }
      }

      HELIUM_BREAK(); // you should not call this, your control is using custom data
      return false;
    }

    // helper to write values to each bound data member separately
    virtual bool WriteAll(const std::vector< tstring >& strs, bool preview = false);

    // fires callback
    virtual void PostWrite();


    //
    // Casting
    //

    template<class T>
    static T* Cast(Control* control)
    {
      HELIUM_ASSERT( control->GetWindow() );
      return static_cast<T*>(control->GetWindow());
    }

    template<class T>
    static const T* Cast( const Control* control )
    {
      HELIUM_ASSERT( control->GetWindow() );
      return static_cast< const T* >( control->GetWindow() );
    }


    //
    // Events
    //

  protected:
    ControlSignature::Event m_RealizeEvent;
  public:
    void AddRealizedListener( const ControlSignature::Delegate& listener )
    {
      m_RealizeEvent.Add( listener );
    }
    void RemoveRealizedListener( const ControlSignature::Delegate& listener )
    {
      m_RealizeEvent.Remove( listener );
    }

  protected:
    ChangingSignature::Event m_BoundDataChanging;
  public:
    void AddBoundDataChangingListener( const ChangingSignature::Delegate& listener )
    {
      m_BoundDataChanging.Add( listener );
    }
    void RemoveBoundDataChangingListener( const ChangingSignature::Delegate& listener )
    {
      m_BoundDataChanging.Remove( listener );
    }
    bool RaiseBoundDataChanging( const Reflect::SerializerPtr& newValue, bool preview )
    {
      return m_BoundDataChanging.RaiseWithReturn( ChangingArgs(this, newValue, preview) );
    }

  protected:
    ChangedSignature::Event m_BoundDataChanged;
  public:
    void AddBoundDataChangedListener( const ChangedSignature::Delegate& listener )
    {
      m_BoundDataChanged.Add( listener );
    }
    void RemoveBoundDataChangedListener( const ChangedSignature::Delegate& listener )
    {
      m_BoundDataChanged.Remove( listener );
    }
    void RaiseBoundDataChanged()
    {
      m_BoundDataChanged.Raise( ChangeArgs(this) );
    }

    //
    // Debug
    //

#ifdef INSPECT_DEBUG_LAYOUT_LOGIC
    virtual void PrintLayout();
#endif
  };

  typedef Helium::SmartPtr<Control> ControlPtr;
  typedef std::vector<ControlPtr> V_Control;
}