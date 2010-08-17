#pragma once

#include "Application/API.h"
#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/InspectInterpreter.h"
#include "Application/Inspect/Controls/InspectContextMenu.h"

#include "Foundation/Math/Point.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Object.h"

namespace Helium
{
    namespace Inspect
    {
        // the scroll increment in pixels
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
        typedef std::vector<ControlAttribute> V_ControlAttribute;

        //
        // ClientData, this could be toolkit OR interpreter client data, there are two pointer in Control
        //
        
        class Control;

        class APPLICATION_API ClientData : public Reflect::AbstractInheritor< ClientData, Reflect::Object >
        {
        public:
            ClientData( Control* control = NULL )
                : m_Control ( control )
            {

            }

            virtual ~ClientData()
            {

            }
            
            Control* GetControl()
            {
                return m_Control;
            }

            const Control* GetControl() const
            {
                return m_Control;
            }

            void SetControl( Control* control )
            {
                m_Control = control;
            }

        protected:
            Control* m_Control;
        };
        typedef Helium::SmartPtr<ClientData> ClientDataPtr;

        //
        // Control
        //

        class APPLICATION_API Control : public Reflect::AbstractInheritor<Control, Reflect::Element>
        {
        public:
            Control();
            virtual ~Control();

            // post-create initialize
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

            ClientData* GetToolkitClientData()
            {
                return m_ToolkitClientData;
            }
            void SetToolkitClientData( ClientData* clientData )
            {
                m_ToolkitClientData = clientData;
            }
            template< class T > Helium::SmartPtr< T > CreateToolkitClientData()
            {
                return new T ( this );
            }

            ClientData* GetInterpreterClientData()
            {
                return m_InterpreterClientData;
            }
            void SetInterpreterClientData( ClientData* clientData )
            {
                m_InterpreterClientData = clientData;
            }
            template< class T > Helium::SmartPtr< T > CreateInterpreterClientData()
            {
                return new T ( this );
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

            // Color, ARGB
            virtual int GetForeColor();
            virtual void SetForeColor(int color);

            virtual int GetBackColor();
            virtual void SetBackColor(int color);

            // Enabled
            virtual bool IsEnabled() const;
            virtual void SetEnabled(bool enabled);

            // ReadOnly
            virtual bool IsReadOnly() const;
            virtual void SetReadOnly(bool readOnly);

            // Context Menu
            virtual const ContextMenuPtr& GetContextMenu();
            virtual void SetContextMenu(const ContextMenuPtr& contextMenu);

            // string elipsization
            int GetStringWidth(const tstring& str);
            virtual bool TrimString(tstring& str, int width);

            // ToolTip
            const tstring& GetToolTip();
            virtual void SetToolTip( const tstring& toolTip );

            // Checks for initialization status
            virtual bool IsRealized();

            // Creates the canvas control, called during layout
            virtual void Realize(Container* parent);

            // Unrealizes the control (delete toolkit object)
            virtual void UnRealize();

            //
            // Layout
            //

        public:
            // populated cachend UI state (drop down lists, etc)
            virtual void Populate() {}

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
            ClientDataPtr m_ToolkitClientData;
            ClientDataPtr m_InterpreterClientData;

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

            // have we really fully realized?
            bool m_Realized;

            // the tool tip for this control
            tstring m_ToolTip;
        };

        typedef Helium::SmartPtr<Control> ControlPtr;
        typedef std::vector<ControlPtr> V_Control;
    }
}