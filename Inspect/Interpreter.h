#pragma once

#include "Platform/Thread.h"
#include "Platform/Locks.h"

#include "Inspect/API.h"
#include "Inspect/DataBinding.h"
#include "Inspect/Controls.h"

namespace Helium
{
    namespace Inspect
    {
        struct PopulateItem
        {
            PopulateItem(const tstring& key, const tstring& data)
            {
                m_Key = key;
                m_Data = data;
            }

            tstring m_Key;
            tstring m_Data;
        };

        typedef std::vector<PopulateItem> V_PopulateItem;

        struct PopulateLinkArgs
        {
            PopulateLinkArgs(uint32_t type) : m_Type (type) {}

            uint32_t        m_Type;
            V_PopulateItem  m_Items;
        };
        typedef Helium::Signature< PopulateLinkArgs&> PopulateLinkSignature;

        struct SelectLinkArgs
        {
            SelectLinkArgs(const tstring& id) : m_ID (id) {}

            const tstring& m_ID;
        };
        typedef Helium::Signature< const SelectLinkArgs&> SelectLinkSignature;

        struct PickLinkArgs
        {
            PickLinkArgs(const DataBindingPtr& data) : m_DataBinding (data) {}

            const DataBindingPtr& m_DataBinding;
        };
        typedef Helium::Signature< const PickLinkArgs&> PickLinkSignature;

        class HELIUM_INSPECT_API ContainerStackPointer : public ThreadLocalPointer
        {
        public:
            ContainerStackPointer();
            ~ContainerStackPointer();

            std::stack< ContainerPtr >& Get();

        private:
            static std::multimap< uint32_t, std::stack< ContainerPtr >* > s_Stacks;
        };

        class HELIUM_INSPECT_API Interpreter HELIUM_ABSTRACT : public Reflect::Object
        {
        public:
            Interpreter (Container* container)
                : m_Container (container) 
            {
                HELIUM_ASSERT(container);
            }

            ~Interpreter()
            {

            }

            //
            // These helpers provide a pinch point for connecting nested interpreter events into this object's event emitters
            //  It essentially keeps all the events emitted in nested interpreters emitting events in the parent interpreter
            //

            template <class T>
            Helium::StrongPtr<T> CreateControl()
            {
                Helium::StrongPtr<T> control = new T ();
                ConnectControlEvents( this, control );
                return control;
            }

            static void ConnectControlEvents( Interpreter* interpreter, Control* control )
            {
                control->e_ControlChanging.AddMethod( &interpreter->PropertyChanging(), &ControlChangingSignature::Event::Raise );
                control->e_ControlChanged.AddMethod( &interpreter->PropertyChanged(), &ControlChangedSignature::Event::Raise );
            }

            template <class T>
            Helium::StrongPtr<T> CreateInterpreter(Container* container = NULL)
            {
                Helium::StrongPtr<T> interpreter = new T (container ? container : m_Container);
                ConnectInterpreterEvents( this, interpreter );
                return interpreter;
            }

            static void ConnectInterpreterEvents( Interpreter* parent, Interpreter* child )
            {
                child->PropertyChanging().AddMethod( &parent->PropertyChanging(), &ControlChangingSignature::Event::Raise );
                child->PropertyChanged().AddMethod( &parent->PropertyChanged(), &ControlChangedSignature::Event::Raise );
                child->PopulateLink().AddMethod( &parent->PopulateLink(), &PopulateLinkSignature::Event::Raise );
                child->SelectLink().AddMethod( &parent->SelectLink(), &SelectLinkSignature::Event::Raise );
                child->PickLink().AddMethod( &parent->PickLink(), &PickLinkSignature::Event::Raise );
            }

            //
            // Panel/container state management
            //

            std::stack< ContainerPtr >& GetContainerStack()
            {
                return m_ContainerStack.Get();
            }

            Container* GetContainer()
            {
                return m_Container;
            }

            void Add(Control* control);
            void Push(Container* container);

            Container* PushContainer( const tstring& name = TXT("") );
            Container* Pop( bool setParent = true );
            Container* Top();

            // Label (no data binding)
            Label* AddLabel(const tstring& name);

            // Button that notifies a listener when it is clicked
            Button* AddButton( const ButtonClickedSignature::Delegate& listener );

            template <class T>
            CheckBox* AddCheckBox( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                CheckBoxPtr control = CreateControl<CheckBox>();
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }

            template <class T>
            Value* AddValue( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                ValuePtr control = CreateControl<Value>();
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }

            template <class T>
            Choice* AddChoice( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                ChoicePtr control = CreateControl<Choice>();
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }

            template <class T>
            Choice* AddChoice( const Reflect::Enumeration* enumInfo, const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                Choice* control = AddChoice<T>( property );

                std::vector< ChoiceItem > items;
                Reflect::DynamicArray< EnumerationElement >::ConstIterator itr = enumInfo->m_Elements.begin();
                Reflect::DynamicArray< EnumerationElement >::ConstIterator end = enumInfo->m_Elements.end();
                for ( ; itr != end; ++itr )
                {
                    std::ostringstream str;
                    str << (*itr)->m_Value;
                    items.push_back( ChoiceItem ( (*itr)->m_Label, str.str() ) );
                }
                control->a_Items.Set(items);
                control->a_IsDropDown.Set(true);

                return control;        
            }

            template <class T>
            List* AddList( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                ListPtr control = CreateControl<List>();
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }

            template <class T>
            Slider* AddSlider( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                SliderPtr control = CreateControl<Slider>();
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }

            template <class T>
            ColorPicker* AddColorPicker( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                ColorPickerPtr control = CreateControl<ColorPicker>();
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }

            //
            // Events
            //

        public:
            ControlChangingSignature::Event& PropertyChanging() const
            {
                return m_PropertyChanging;
            }

            ControlChangedSignature::Event& PropertyChanged() const
            {
                return m_PropertyChanged;
            }

            PopulateLinkSignature::Event& PopulateLink() const
            {
                return m_PopulateLink;
            }

            SelectLinkSignature::Event& SelectLink() const
            {
                return m_SelectLink;
            }

            PickLinkSignature::Event& PickLink() const
            {
                return m_PickLink;
            }

        protected:
            // the container to inject into (these are not long lived hard references)
            //  this only stores pointers to GUI when generating UI (before they are added
            //  to the canvas, which is where the controls live permanently)
            Container* m_Container;

            // context for push/pop api
            ContainerStackPointer m_ContainerStack;

            // the changing event, emitted from Changing()
            mutable ControlChangingSignature::Event m_PropertyChanging;

            // the changed event, emitted from Changed()
            mutable ControlChangedSignature::Event m_PropertyChanged;

            // the find event, handlers should seek and select the contents
            mutable PopulateLinkSignature::Event m_PopulateLink;

            // the select event, handlers should seek and select the item linked by the data
            mutable SelectLinkSignature::Event m_SelectLink;

            // the pick event, handlers should stash data and write descriptor when selection occurs
            mutable PickLinkSignature::Event m_PickLink;
        };

        typedef Helium::StrongPtr<Interpreter> InterpreterPtr;
    }
}