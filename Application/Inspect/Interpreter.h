#pragma once

#include "Application/API.h"

#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Data/StringData.h"
#include "Application/Inspect/Data/SerializerData.h"
#include "Platform/Mutex.h"

namespace Helium
{
    namespace Inspect
    {
        //
        // Forwards
        //

        class Control;
        class Container;
        class Group;
        class Panel;

        class Button;
        class Action;

        class CheckBox;
        class CheckList;
        class ColorPicker;
        class Label;
        class List;
        class Slider;

        class Choice;
        class Value;


        //
        // Events
        //

        struct PopulateLinkArgs
        {
            u32 m_Type;
            V_Item m_Items;

            PopulateLinkArgs(u32 type)
                : m_Type (type)
            {

            }
        };
        typedef Helium::Signature<void, PopulateLinkArgs&> PopulateLinkSignature;

        struct SelectLinkArgs
        {
            const tstring& m_ID;

            SelectLinkArgs(const tstring& id)
                : m_ID (id)
            {

            }
        };
        typedef Helium::Signature<void, const SelectLinkArgs&> SelectLinkSignature;

        struct PickLinkArgs
        {
            const DataPtr& m_Data;

            PickLinkArgs(const DataPtr& data)
                : m_Data (data)
            {

            }
        };
        typedef Helium::Signature<void, const PickLinkArgs&> PickLinkSignature;


        //
        // Interpreters are owned by one or more controls.  They are the
        //  adapter base class upon which derived interpreters (or enumerators)
        //  can extend.  Those derived classes can be intimate with the format
        //  of the data that is being analyzed to generate the GUI widgets.
        //
        // This class is HELIUM_ABSTRACT and lightweight, and pretty much only here
        //  to provide very basic API at a later date.
        //

        typedef std::stack< ContainerPtr > ST_Container;
        typedef std::map< u32, ST_Container > M_U32ContainerStack;

        class APPLICATION_API Interpreter HELIUM_ABSTRACT : public Reflect::Object
        {
        protected:
            // the container to inject into (these are not long lived hard references)
            //  this only stores pointers to GUI when generating UI (before they are added
            //  to the canvas, which is where the controls live permanently)
            Container* m_Container;

            // context for push/pop api
            M_U32ContainerStack m_ContainerStack;

            // prevent access to creating new container stacks
            Platform::Mutex m_ContainerStackMutex;

        public:
            Interpreter (Container* container)
                : m_Container (container) 
            {
                HELIUM_ASSERT(container);
            }

            virtual ~Interpreter()
            {

            }


            //
            // These helpers provide a pinch point for connecting nested interpreter events into this object's event emitters
            //  It essentially keeps all the events emitted in nested interpreters emitting events in the parent interpreter
            //

            template <class T>
            Helium::SmartPtr<T> CreateInterpreter(Container* container = NULL)
            {
                Helium::SmartPtr<T> interpreter = new T (container ? container : m_Container);
                ConnectInterpreterEvents( this, interpreter );
                return interpreter;
            }

            static void ConnectInterpreterEvents( Interpreter* parent, Interpreter* child )
            {
                child->AddPropertyChangingListener( ChangingSignature::Delegate (parent, &Interpreter::RaisePropertyChanging) );
                child->AddPropertyChangedListener( ChangedSignature::Delegate (parent, &Interpreter::RaisePropertyChanged) );
                child->AddPopulateLinkListener( PopulateLinkSignature::Delegate (parent, &Interpreter::RaisePopulateLink) );
                child->AddSelectLinkListener( SelectLinkSignature::Delegate (parent, &Interpreter::RaiseSelectLink) );
                child->AddPickLinkListener( PickLinkSignature::Delegate (parent, &Interpreter::RaisePickLink) );
            }


            //
            // Panel/container state management
            //

            ST_Container& GetCurrentContainerStack();

            Container* GetContainer()
            {
                return m_Container;
            }

            void Add(Control* control);
            void Push(Container* container);
            Panel* PushPanel(const tstring& name, bool expanded = false);
            Container* PushContainer();
            Container* Pop( bool setParent = true );
            Container* Top();


            //
            // Label (no data binding)
            //

            Label* AddLabel(const tstring& name);


            //
            // Button
            //

            // Button that notifies a listener when it is clicked
            Action* AddAction( const ActionSignature::Delegate& listener );


            //
            // CheckBox
            //

            template <class T>
            CheckBox* AddCheckBox( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                CheckBoxPtr control = m_Container->GetCanvas()->Create<CheckBox>(this);
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddControl(control);
                return control;
            }


            //
            // Value
            //

            template <class T>
            Value* AddValue( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                ValuePtr control = m_Container->GetCanvas()->Create<Value>(this);
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddControl(control);
                return control;
            }


            //
            // Choice
            //

            template <class T>
            Choice* AddChoice( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                ChoicePtr control = m_Container->GetCanvas()->Create<Choice>(this);
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddControl(control);
                return control;
            }

            template <class T>
            Choice* AddChoice( const Reflect::Enumeration* enumInfo, const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                Choice* control = AddChoice<T>( property );

                V_Item items;
                Reflect::V_EnumerationElement::const_iterator itr = enumInfo->m_Elements.begin();
                Reflect::V_EnumerationElement::const_iterator end = enumInfo->m_Elements.end();
                for ( ; itr != end; ++itr )
                {
                    std::ostringstream str;
                    str << (*itr)->m_Value;
                    items.push_back( Item ( (*itr)->m_Label, str.str() ) );
                }
                control->SetItems(items);
                control->SetDropDown(true);

                return control;        
            }


            //
            // List
            //

            template <class T>
            List* AddList( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                ListPtr control = m_Container->GetCanvas()->Create<List>(this);
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddControl(control);
                return control;
            }


            //
            // CheckList
            //

            template <class T>
            CheckList* AddCheckList( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                CheckListPtr control = m_Container->GetCanvas()->Create<CheckList>(this);
                control->Bind( new SerializerPropertyFormatter< T >( property ) );
                ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddControl(control);
                return control;
            }


            //
            // Slider
            //

            template <class T>
            Slider* AddSlider( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                SliderPtr control = m_Container->GetCanvas()->Create<Slider>(this);
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddControl(control);
                return control;
            }


            //
            // ColorPicker
            //

            template <class T>
            ColorPicker* AddColorPicker( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                ColorPickerPtr control = m_Container->GetCanvas()->Create<ColorPicker>(this);
                control->Bind( new PropertyStringFormatter<T> ( property ) );
                ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddControl(control);
                return control;
            }


            //
            // Events
            //

        protected:
            // the changing event, emitted from Changing()
            mutable ChangingSignature::Event m_PropertyChanging;
        public:
            void AddPropertyChangingListener(const ChangingSignature::Delegate& listener)
            {
                m_PropertyChanging.Add(listener);
            }
            void RemovePropertyChangingListener(const ChangingSignature::Delegate& listener)
            {
                m_PropertyChanging.Remove(listener);
            }
            bool RaisePropertyChanging( const ChangingArgs& args )
            {
                return m_PropertyChanging.RaiseWithReturn( args );
            }

        protected:
            // the changed event, emitted from Changed()
            mutable ChangedSignature::Event m_PropertyChanged;
        public:
            void AddPropertyChangedListener(const ChangedSignature::Delegate& listener)
            {
                m_PropertyChanged.Add(listener);
            }
            void RemovePropertyChangedListener(const ChangedSignature::Delegate& listener)
            {
                m_PropertyChanged.Remove(listener);
            }
            void RaisePropertyChanged( const ChangeArgs& args )
            {
                m_PropertyChanged.Raise( args );
            }

        protected:
            // the find event, handlers should seek and select the contents
            mutable PopulateLinkSignature::Event m_PopulateLink;
        public:
            void AddPopulateLinkListener(const PopulateLinkSignature::Delegate& listener)
            {
                m_PopulateLink.Add(listener);
            }
            void RemovePopulateLinkListener(const PopulateLinkSignature::Delegate& listener)
            {
                m_PopulateLink.Remove(listener);
            }
            void RaisePopulateLink( PopulateLinkArgs& args )
            {
                m_PopulateLink.Raise( args );
            }

        protected:
            // the select event, handlers should seek and select the item linked by the data
            mutable SelectLinkSignature::Event m_SelectLink;
        public:
            void AddSelectLinkListener(const SelectLinkSignature::Delegate& listener)
            {
                m_SelectLink.Add(listener);
            }
            void RemoveSelectLinkListener(const SelectLinkSignature::Delegate& listener)
            {
                m_SelectLink.Remove(listener);
            }
            void RaiseSelectLink( const SelectLinkArgs& args )
            {
                m_SelectLink.Raise( args );
            }

        protected:
            // the pick event, handlers should stash data and write descriptor when selection occurs
            mutable PickLinkSignature::Event m_PickLink;
        public:
            void AddPickLinkListener(const PickLinkSignature::Delegate& listener)
            {
                m_PickLink.Add(listener);
            }
            void RemovePickLinkListener(const PickLinkSignature::Delegate& listener)
            {
                m_PickLink.Remove(listener);
            }
            void RaisePickLink( const PickLinkArgs& args )
            {
                m_PickLink.Raise( args );
            }
        };

        typedef Helium::SmartPtr<Interpreter> InterpreterPtr;
    }
}