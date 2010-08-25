#pragma once

#include <string>
#include <map>

#include "Core/API.h"
#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/InspectControls.h"
#include "Application/Inspect/InspectInterpreter.h"
#include "Application/Inspect/Interpreters/File/FileDialogButton.h"
#include "Application/Inspect/Interpreters/File/FileBrowserButton.h"
#include "Application/Inspect/Interpreters/Content/ParametricKeyData.h"
#include "Application/Inspect/Interpreters/Content/ParametricKeyControl.h"

#include "Core/Scene/Selection.h"

namespace Helium
{
    namespace Core
    {
        struct CORE_API CreatePanelArgs
        {
            class PropertiesGenerator* m_Generator;
            const OS_SelectableDumbPtr& m_Selection;

            CreatePanelArgs(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection)
                : m_Generator (generator)
                , m_Selection (selection)
            {

            }
        };

        // callback for creating a named panel creator for the generator
        typedef Helium::Signature< void, CreatePanelArgs& > CreatePanelSignature;

        typedef std::map<tstring, CreatePanelSignature::Delegate> M_PanelCreators;

        extern M_PanelCreators s_PanelCreators;

        class CORE_API PropertiesGenerator : public Inspect::Interpreter
        {
        public:
            PropertiesGenerator (Inspect::Container* container);
            virtual ~PropertiesGenerator();

            // init and cleanup generator system
            static void Initialize();
            static void Cleanup();

            // attach client panel creator by name
            static bool InitializePanel(const tstring& name, const CreatePanelSignature::Delegate& creator);
            static void CleanupPanel(const tstring& name);

            // reset state
            void Reset();




            //
            //
            // These templated UI creators helps assist creating generic UI bound to getters and setters
            //
            //

            template <class T>
            Inspect::CheckBox* AddCheckBox( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                return Inspect::Interpreter::AddCheckBox( property );
            }

            template <class T>
            Inspect::Value* AddValue( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                return Inspect::Interpreter::AddValue( property );
            }

            template <class T>
            Inspect::Choice* AddChoice( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                return Inspect::Interpreter::AddChoice( property );
            }

            template <class T>
            Inspect::Choice* AddChoice( const Reflect::Enumeration* enumInfo, const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                return Inspect::Interpreter::AddChoice( enumInfo, property );
            }

            template <class T>
            Inspect::List* AddList( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                return Inspect::Interpreter::AddList( property );
            }

            template <class T>
            Inspect::Slider* AddSlider( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                return Inspect::Interpreter::AddSlider( property );
            }

            template <class T>
            Inspect::ColorPicker* AddColorPicker( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                return Inspect::Interpreter::AddColorPicker( property );
            }


            //
            // Behold how magical this function really is
            //

            // note the template paramters to types of member function pointers, which actually get deduced by the compiler (thank GOD)
            template<class T, class D, class G, class S>
            std::vector< Helium::SmartPtr< Helium::Property<D> > > BuildSelectionProperties(const OS_SelectableDumbPtr& selection, G getter, S setter)
            {
                std::vector< Helium::SmartPtr< Helium::Property<D> > > properties;

                OS_SelectableDumbPtr::Iterator itr = selection.Begin();
                OS_SelectableDumbPtr::Iterator end = selection.End();
                for ( ; itr != end; ++itr )
                {
                    // note the dynamic cast, and how AWESOME it is
                    properties.push_back( new Helium::MemberProperty<T, D> (dynamic_cast<T*>(*itr), getter, setter) );
                }

                return properties;
            }


            //
            // Button
            //

            // File dialog button with a single target
            template <class T>
            Inspect::FileDialogButton* AddFileDialogButton( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                Inspect::FileDialogButtonPtr control = m_Container->GetCanvas()->Create<Inspect::FileDialogButton>( this );
                control->Bind( new Inspect::PropertyStringFormatter<T> ( property ) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }

            // File dialog button with a selection list
            template <class T, class D, class G, class S>
            Inspect::FileDialogButton* AddFileDialogButton( const OS_SelectableDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::FileDialogButtonPtr control = m_Container->GetCanvas()->Create<Inspect::FileDialogButton>( this );
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }

            // File browse button with single target (for wildcard text searches through the asset files)
            template <class T>
            Inspect::FileBrowserButton* AddFileBrowserButton( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                Inspect::FileBrowserButtonPtr control = m_Container->GetCanvas()->Create<Inspect::FileBrowserButton>( this );
                control->Bind( new Inspect::PropertyStringFormatter<T> ( property ) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }

            // File browse button with a selection list
            template <class T, class D, class G, class S>
            Inspect::FileBrowserButton* AddFileBrowserButton( const OS_SelectableDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::FileBrowserButtonPtr control = m_Container->GetCanvas()->Create<Inspect::FileBrowserButton>( this );
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }


            //
            // CheckBox
            //

            template <class T, class D, class G, class S>
            Inspect::CheckBox* AddCheckBox( const OS_SelectableDumbPtr& selection, G getter = NULL, S setter = NULL, bool significant = true)
            {
                Inspect::CheckBoxPtr control = m_Container->GetCanvas()->Create<Inspect::CheckBox>( this );

                Inspect::DataPtr data = new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )); 
                data->SetSignificant( significant ); 
                control->Bind( data );

                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }


            //
            // TextBox
            //

            template <class T, class D, class G, class S>
            Inspect::Value* AddValue( const OS_SelectableDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::ValuePtr control = m_Container->GetCanvas()->Create<Inspect::Value>( this );
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }


            //
            // Choice
            //

            template <class T, class D, class G, class S>
            Inspect::Choice* AddChoice( const OS_SelectableDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::ChoicePtr control = m_Container->GetCanvas()->Create<Inspect::Choice>( this );
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }

            template <class T, class D, class G, class S>
            Inspect::Choice* AddChoice( const OS_SelectableDumbPtr& selection, const Reflect::Enumeration* enumInfo, G getter = NULL, S setter = NULL )
            {
                Inspect::ChoicePtr control = AddChoice<T, D, G, S>( selection, getter, setter );

                Inspect::V_Item items;
                Reflect::V_EnumerationElement::const_iterator itr = enumInfo->m_Elements.begin();
                Reflect::V_EnumerationElement::const_iterator end = enumInfo->m_Elements.end();
                for ( ; itr != end; ++itr )
                {
                    tostringstream str;
                    str << (*itr)->m_Value;
                    items.push_back( Inspect::Item ( (*itr)->m_Label, str.str() ) );
                }
                control->SetItems(items);
                control->SetDropDown(true);

                return control;
            }


            //
            // List
            //

            template <class T, class D, class G, class S>
            Inspect::List* AddList( const OS_SelectableDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::ListPtr control = m_Container->GetCanvas()->Create<Inspect::List>( this );
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }


            //
            // Slider
            //

            template <class T, class D, class G, class S>
            Inspect::Slider* AddSlider( const OS_SelectableDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::SliderPtr control = m_Container->GetCanvas()->Create<Inspect::Slider>( this );
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }


            //
            // ColorPicker
            //

            template <class T, class D, class G, class S>
            Inspect::ColorPicker* AddColorPicker( const OS_SelectableDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::ColorPickerPtr control = m_Container->GetCanvas()->Create<Inspect::ColorPicker>( this );
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild(control);
                return control;
            }


            // 
            // KeyControl
            // 

            template <class T, class G, class S>
            Inspect::ParametricKeyControl* AddKeyControl( const OS_SelectableDumbPtr& selection, Reflect::CreateObjectFunc creator, G getter = NULL, S setter = NULL )
            {
                Inspect::ParametricKeyControlPtr control = m_Container->GetCanvas()->Create<Inspect::ParametricKeyControl>( this );
                control->Bind( new Inspect::MultiParametricKeyPropertyFormatter( creator, BuildSelectionProperties<T, Content::V_ParametricKeyPtr, G, S>( selection, getter, setter ) ) );
                Inspect::ST_Container& containerStack = GetCurrentContainerStack();
                containerStack.top()->AddChild( control );
                return control;
            }
        };

        typedef Helium::SmartPtr<PropertiesGenerator> EnumeratorPtr;
    }
}