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
        class Control;
        class Container;
        class Canvas;

        const static tchar ATTR_VALUE_TRUE[]    = TXT( "true" );
        const static tchar ATTR_VALUE_FALSE[]   = TXT( "false" );
        const static tchar ATTR_TOOLTIP[]       = TXT( "tooltip" );

        //
        // Event Args and Signatures
        //

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
        // Widget, a base class for a GUI system implementation-specific Widget classes
        //

        class APPLICATION_API Widget : public Reflect::AbstractInheritor<Control, Reflect::Object>
        {
        public:
            Widget( Control* control )
                : m_Control( control )
            {

            }

            Inspect::Control* GetControl()
            {
                return m_Control;
            }

            virtual void Read() = 0;
            virtual bool Write() = 0;

        protected:
            Inspect::Control* m_Control;
        };
        typedef Helium::SmartPtr<Widget> WidgetPtr;

        //
        // Control, a class that is binadable to data and controls the state and appearance of a widget
        //  Controls own a Widget via a references counter smart pointer
        //  Controls can be created and modified without causing GUI widgets being created)
        //  Widgets are allocated to a Control when Realized, and deleted when Unrealized
        //

        class APPLICATION_API Control : public Reflect::AbstractInheritor<Control, Reflect::Object>
        {
        public:
            Control();
            virtual ~Control();

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
            // Client data
            // 

            Widget* GetWidget()
            {
                return m_Widget;
            }
            void SetWidget( Widget* widget )
            {
                m_Widget = widget;
            }

            // 
            // Client data
            // 

            ClientData* GetClientData()
            {
                return m_ClientData;
            }
            void SetClientData( ClientData* clientData )
            {
                m_ClientData = clientData;
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
            // Defaults
            //

            // queries if value is at default
            virtual bool IsDefault() const;

            // sets data back to default
            virtual bool SetDefault();

            // updates control appearance to appear to be at default value
            virtual void SetDefaultAppearance(bool def) {}

            // Context Menu
            virtual const ContextMenuPtr& GetContextMenu();
            virtual void SetContextMenu(const ContextMenuPtr& contextMenu);

            // process individual attribute key
            virtual bool Process(const tstring& key, const tstring& value);

            // Checks for initialization status
            virtual bool IsRealized();

            // Creates the canvas control, called during layout
            virtual void Realize(Container* parent);

            // Unrealizes the control (delete toolkit object)
            virtual void Unrealize();

            // populated cachend UI state (drop down lists, etc)
            virtual void Populate() {}

            //
            // Read
            //

            // refreshes the UI state from data
            virtual void Read();

            // helper read call for string based controls
            bool ReadStringData(tstring& str) const;

            // helper read call to get values of all bound data
            bool ReadAllStringData(std::vector< tstring >& strs) const;

            // helper write function for all other types of data
            template<class T>
            bool ReadTypedData(const typename DataTemplate<T>::Ptr& data, T& val);

            // callback when data changed, implements DataReference
            void DataChanged(const DataChangedArgs& args);

            //
            // Write
            //

            // fires callback
            bool PreWrite( Reflect::Serializer* newValue, bool preview );

            // updates the data based on the state of the UI
            virtual bool Write();

            // helper write call for string based controls
            bool WriteStringData(const tstring& str, bool preview = false);

            // helper to write values to each bound data member separately
            bool WriteAllStringData(const std::vector< tstring >& strs, bool preview = false);

            // helper write function for all other types of data
            template<class T>
            bool WriteTypedData(const T& val, const typename DataTemplate<T>::Ptr& data, bool preview = false);

            // fires callback
            void PostWrite();

        public:
            Attribute<bool>                         a_IsEnabled;              // are we enabled?
            Attribute<bool>                         a_IsReadOnly;             // are we writable?
            Attribute<bool>                         a_IsFrozen;               // is updating (polling, sorting, etc) disabled?
            Attribute<bool>                         a_IsHidden;               // is rendering disabled?
            Attribute<u32>                          a_ForegroundColor;        // our colors for appearange
            Attribute<u32>                          a_BackgroundColor;
            Attribute<bool>                         a_IsFixedWidth;           // are we fixed along an axis?
            Attribute<bool>                         a_IsFixedHeight;
            Attribute<f32>                          a_ProportionalWidth;      // are we proportional along an axis?
            Attribute<f32>                          a_ProportionalHeight;
            Attribute<tstring>                      a_Default;                // the default value
            Attribute<tstring>                      a_ToolTip;                // the tool tip for this control
            
            mutable ControlSignature::Event         e_Realized;               // upon realization of the control
            mutable ControlSignature::Event         e_Unrealized;

            mutable ControlChangingSignature::Event e_ControlChanging;        // these mean the *data state* of the control, not the appearance metrics
            mutable ControlChangedSignature::Event  e_ControlChanged;

        protected:
            // our context menu, if any
            ContextMenuPtr      m_ContextMenu;

            // the canvas that implements us
            Canvas*             m_Canvas;

            // the parent
            Container*          m_Parent;

            // writing flag (for re-entrancy checking)
            bool                m_IsWriting;

            // have we really fully realized?
            bool                m_IsRealized;

            // the data we manipulate
            DataPtr             m_BoundData;

            // GUI toolkit object
            WidgetPtr           m_Widget;

            // client-configurable data
            ClientDataPtr       m_ClientData;
        };

        typedef Helium::SmartPtr<Control> ControlPtr;
        typedef std::vector<ControlPtr> V_Control;

        template<class T>
        inline bool Control::ReadTypedData(const typename DataTemplate<T>::Ptr& data, T& val)
        {
            if (data)
            {
                T currentValue;
                data->Get( currentValue );
            }

            HELIUM_BREAK(); // you should not call this, your control is using custom data
            return false;
        }

        template<class T>
        inline bool Control::WriteTypedData(const T& val, const typename DataTemplate<T>::Ptr& data, bool preview)
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

#ifdef PROFILE_ACCUMULATION
        APPLICATION_API extern Profile::Accumulator g_RealizeAccumulator;
#endif
    }
}