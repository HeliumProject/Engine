#pragma once

#include "Math/Point.h"
#include "Reflect/Object.h"
#include "Foundation/Attribute.h"

#include "Inspect/API.h"
#include "Inspect/DataBinding.h"
#include "Inspect/ContextMenu.h"

namespace Helium
{
    namespace Inspect
    {
        class Control;
        class Container;
        class Canvas;

        const static tchar_t ATTR_VALUE_TRUE[]    = TXT( "true" );
        const static tchar_t ATTR_VALUE_FALSE[]   = TXT( "false" );
        const static tchar_t ATTR_HELPTEXT[]      = TXT( "helptext" );

        //
        // Event Args and Signatures
        //

        typedef Helium::Signature< Control* > ControlSignature;

        struct ControlChangingArgs
        {
            ControlChangingArgs( class Control* control, Reflect::Data* newValue, bool preview )
                : m_Control( control )
                , m_NewValue( newValue )
                , m_Preview( preview )
                , m_Veto( false )
            {

            }

            Control*                m_Control;
            Reflect::Data*    m_NewValue;
            bool                    m_Preview;
            mutable bool            m_Veto;
        };
        typedef Helium::Signature<const ControlChangingArgs&> ControlChangingSignature;

        struct ControlChangedArgs
        {
            ControlChangedArgs(class Control* control) : m_Control (control) {}

            Control* m_Control;
        };
        typedef Helium::Signature< const ControlChangedArgs&> ControlChangedSignature;

        //
        // ClientData, this could be toolkit OR interpreter client data, there are two pointer in Control
        //

        class HELIUM_INSPECT_API ClientData : public Reflect::Object
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ClientData, Reflect::Object );

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
        typedef Helium::StrongPtr<ClientData> ClientDataPtr;

        //
        // Widget, a base class for a GUI system implementation-specific Widget classes
        //

        class HELIUM_INSPECT_API Widget : public Reflect::Object
        {
        public:
            REFLECT_DECLARE_ABSTRACT( Widget, Reflect::Object );

            Widget()
                : m_Control( NULL )
            {

            }

            Inspect::Control* GetControl()
            {
                return m_Control;
            }

            void SetControl( Inspect::Control* control )
            {
                m_Control = control;
            }

            virtual void Read() = 0;
            virtual bool Write() = 0;

        protected:
            Inspect::Control* m_Control;
        };
        typedef Helium::StrongPtr<Widget> WidgetPtr;

        //
        // Control, a class that is binadable to data and controls the state and appearance of a widget
        //  Controls own a Widget via a references counter smart pointer
        //  Controls can be created and modified without causing GUI widgets being created)
        //  Widgets are allocated to a Control when Realized, and deleted when Unrealized
        //

        class HELIUM_INSPECT_API Control : public Reflect::Object
        {
        public:
            REFLECT_DECLARE_ABSTRACT( Control, Reflect::Object );
            Control();
            virtual ~Control();

            int GetDepth();

            Canvas* GetCanvas()
            {
                return m_Canvas;
            }
            void SetCanvas(Canvas* canvas);

            Container* GetParent()
            {
                return m_Parent;
            }
            void SetParent( Container* parent );

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
                return m_DataBinding.ReferencesObject();
            }

            const DataBinding* GetBinding() const
            {
                return m_DataBinding;
            }

            virtual void Bind(const DataBindingPtr& data);

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
            virtual void Realize(Canvas* canvas);

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
            bool ReadTypedData(const typename DataBindingTemplate<T>::Ptr& data, T& val);

            // callback when data changed, implements DataReference
            void DataChanged(const DataChangedArgs& args);

            //
            // Write
            //

            // fires callback
            bool PreWrite( Reflect::Data* newValue, bool preview );

            // updates the data based on the state of the UI
            virtual bool Write();

            // helper write call for string based controls
            bool WriteStringData(const tstring& str, bool preview = false);

            // helper to write values to each bound data member separately
            bool WriteAllStringData(const std::vector< tstring >& strs, bool preview = false);

            // helper write function for all other types of data
            template<class T>
            bool WriteTypedData(const T& val, const typename DataBindingTemplate<T>::Ptr& data, bool preview = false);

            // fires callback
            void PostWrite();

        public:
            Attribute< bool >                       a_IsEnabled;              // are we enabled?
            Attribute< bool >                       a_IsReadOnly;             // are we writable?
            Attribute< bool >                       a_IsFrozen;               // is updating (polling, sorting, etc) disabled?
            Attribute< bool >                       a_IsHidden;               // is rendering disabled?
            Attribute< uint32_t >                        a_ForegroundColor;        // our colors for appearange
            Attribute< uint32_t >                        a_BackgroundColor;
            Attribute< bool >                       a_IsFixedWidth;           // are we fixed along an axis?
            Attribute< bool >                       a_IsFixedHeight;
            Attribute< float32_t >                        a_ProportionalWidth;      // are we proportional along an axis?
            Attribute< float32_t >                        a_ProportionalHeight;
            Attribute< tstring >                    a_Default;                // the default value
            Attribute< tstring >                    a_HelpText;               // the help text for this control
            
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
            DataBindingPtr      m_DataBinding;

            // GUI toolkit object
            WidgetPtr           m_Widget;

            // client-configurable data
            ClientDataPtr       m_ClientData;

            //
            // Properties System
            //
        private:
            mutable std::map< tstring, tstring > m_Properties;

        public:

            template<class T>
            inline void SetProperty( const tstring& key, const T& value )
            {
                tostringstream str;
                str << value;

                if ( !str.fail() )
                {
                    SetProperty<tstring>( key, str.str() );
                }
            }

            template<>
            inline void SetProperty( const tstring& key, const tstring& value )
            {
                m_Properties[key] = value;
            }

            template<class T>
            inline bool GetProperty( const tstring& key, T& value ) const
            {
                tstring strValue;
                bool result = GetProperty<tstring>( key, strValue );

                if ( result )
                {
                    tistringstream str( strValue );
                    str >> value;
                    return !str.fail();
                }

                return false;
            }

            template<>
            inline bool GetProperty( const tstring& key, tstring& value ) const
            {
                std::map< tstring, tstring >::const_iterator found = m_Properties.find( key ); 
                if ( found != m_Properties.end() )
                {
                    value = found->second;
                    return true;
                }

                return false;
            }

            inline const tstring& GetProperty( const tstring& key ) const
            {
                std::map< tstring, tstring >::const_iterator found = m_Properties.find( key );
                if ( found != m_Properties.end() )
                {
                    return found->second;
                }

                static tstring empty;
                return empty;
            }
        };

        typedef Helium::StrongPtr<Control> ControlPtr;
        typedef std::vector<ControlPtr> V_Control;

        template<class T>
        inline bool Control::ReadTypedData(const typename DataBindingTemplate<T>::Ptr& data, T& val)
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
        inline bool Control::WriteTypedData(const T& val, const typename DataBindingTemplate<T>::Ptr& dataBinding, bool preview)
        {
            if (dataBinding)
            {
                T currentValue;
                dataBinding->Get( currentValue );
                if ( val == currentValue )
                {
                    return true;
                }

                Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create<T>() );
                data->ConnectData( const_cast< T* >( &val ) );
                if ( !PreWrite( data, preview ) )
                {
                    Read();
                    return false;
                }

                m_IsWriting = true;
                bool result = dataBinding->Set( val );
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
        HELIUM_INSPECT_API extern Profile::Accumulator g_RealizeAccumulator;
        HELIUM_INSPECT_API extern Profile::Accumulator g_UnrealizeAccumulator;
#endif
    }
}