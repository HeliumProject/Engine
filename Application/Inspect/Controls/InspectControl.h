#pragma once

#include "Foundation/Math/Point.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Automation/Attribute.h"

#include "Application/API.h"
#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/Controls/InspectContextMenu.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar ATTR_VALUE_TRUE[]    = TXT( "true" );
        const static tchar ATTR_VALUE_FALSE[]   = TXT( "false" );
        const static tchar ATTR_TOOLTIP[]       = TXT( "tooltip" );

        //
        // Event Args and Signatures
        //

        class Control;

        typedef Helium::Signature< void, Control* > ControlSignature;

        struct ControlChangingArgs
        {
            ControlChangingArgs( class Control* control, Reflect::Serializer* newValue, bool preview )
                : m_Control( control )
                , m_NewValue( newValue )
                , m_Preview( preview )
            {

            }

            Control*                m_Control;
            Reflect::Serializer*    m_NewValue;
            bool                    m_Preview;
        };
        typedef Helium::Signature<bool, const ControlChangingArgs&> ControlChangingSignature;

        struct ControlChangedArgs
        {
            ControlChangedArgs(class Control* control) : m_Control (control) {}

            Control* m_Control;
        };
        typedef Helium::Signature<void, const ControlChangedArgs&> ControlChangedSignature;

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

        class APPLICATION_API Control : public Reflect::AbstractInheritor<Control, Reflect::Object>
        {
        public:
            Control();
            virtual ~Control();
            virtual void Create(); // post-create initialize

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

            //
            // Data Binding
            //

            bool IsBound() const
            {
                return m_BoundData.ReferencesObject();
            }

            const Data* GetData() const
            {
              return m_BoundData;
            }

            virtual void Bind(const DataPtr& data);

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
                return m_IsFixedWidth;
            }

            bool IsFixedHeight() const
            {
                return m_IsFixedHeight;
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
            virtual bool EllipsizeString(tstring& str, int width);

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
            virtual bool PreWrite( Reflect::Serializer* newValue, bool preview );

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

                    m_IsWriting = true;
                    bool result = data->Set( val );
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

            ControlChangingSignature::Event& ControlChanging() const
            {
                return m_ControlChanging;
            }

            ControlChangedSignature::Event& ControlChanged() const
            {
                return m_ControlChanged;
            }

            ControlSignature::Event& Realized() const
            {
                return m_Realized;
            }

        protected:
            // are we enabled?
            bool m_IsEnabled;

            // are we writable?
            bool m_IsReadOnly;

            // our colors for appearange
            int m_ForeColor;
            int m_BackColor;

            // are we fixed along an axis?
            bool m_IsFixedWidth;
            bool m_IsFixedHeight;

            // are we proportional along an axis?
            f32 m_ProportionalWidth;
            f32 m_ProportionalHeight;

            // the default value
            tstring m_Default;

            // the tool tip for this control
            tstring m_ToolTip;

            // our context menu, if any
            ContextMenuPtr m_ContextMenu;

            // the canvas that implements us
            Canvas* m_Canvas;

            // the parent
            Container* m_Parent;

            // writing flag (for re-entrancy checking)
            bool m_IsWriting;

            // have we really fully realized?
            bool m_IsRealized;

            // the data we manipulate
            DataPtr m_BoundData;

            // client-configurable data
            ClientDataPtr m_ToolkitClientData;
            ClientDataPtr m_InterpreterClientData;

            // upon realization of the control
            ControlSignature::Event m_Realized;

            // these mean the *data state* of the control, not the appearance metrics
            ControlChangingSignature::Event m_ControlChanging;
            ControlChangedSignature::Event m_ControlChanged;
        };

        typedef Helium::SmartPtr<Control> ControlPtr;
        typedef std::vector<ControlPtr> V_Control;

#ifdef PROFILE_ACCUMULATION
        APPLICATION_API extern Profile::Accumulator g_RealizeAccumulator;
#endif
    }
}