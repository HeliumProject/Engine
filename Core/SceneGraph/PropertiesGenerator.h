#pragma once

#include <string>
#include <map>

#include "Foundation/Inspect/Data.h"
#include "Foundation/Inspect/Controls.h"
#include "Foundation/Inspect/Interpreter.h"

#include "Core/API.h"
#include "Core/SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PropertiesGenerator;

        struct CORE_API CreatePanelArgs
        {
            PropertiesGenerator*        m_Generator;
            const OS_SceneNodeDumbPtr&  m_Selection;

            CreatePanelArgs(PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection)
                : m_Generator (generator)
                , m_Selection (selection)
            {

            }
        };

        // callback for creating a named panel creator for the generator
        typedef Helium::Signature< CreatePanelArgs& > CreatePanelSignature;

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
            std::vector< Helium::SmartPtr< Helium::Property<D> > > BuildSelectionProperties(const OS_SceneNodeDumbPtr& selection, G getter, S setter)
            {
                std::vector< Helium::SmartPtr< Helium::Property<D> > > properties;

                OS_SceneNodeDumbPtr::Iterator itr = selection.Begin();
                OS_SceneNodeDumbPtr::Iterator end = selection.End();
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
                Inspect::FileDialogButtonPtr control = CreateControl<Inspect::FileDialogButton>();
                control->Bind( new Inspect::PropertyStringFormatter<T> ( property ) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }

            // File dialog button with a selection list
            template <class T, class D, class G, class S>
            Inspect::FileDialogButton* AddFileDialogButton( const OS_SceneNodeDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::FileDialogButtonPtr control = CreateControl<Inspect::FileDialogButton>();
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }


            //
            // CheckBox
            //

            template <class T, class D, class G, class S>
            Inspect::CheckBox* AddCheckBox( const OS_SceneNodeDumbPtr& selection, G getter = NULL, S setter = NULL, bool significant = true)
            {
                Inspect::CheckBoxPtr control = CreateControl<Inspect::CheckBox>();

                Inspect::DataPtr data = new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )); 
                data->SetSignificant( significant ); 
                control->Bind( data );

                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }


            //
            // TextBox
            //

            template <class T, class D, class G, class S>
            Inspect::Value* AddValue( const OS_SceneNodeDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::ValuePtr control = CreateControl<Inspect::Value>();
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }


            //
            // Choice
            //

            template <class T, class D, class G, class S>
            Inspect::Choice* AddChoice( const OS_SceneNodeDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::ChoicePtr control = CreateControl<Inspect::Choice>();
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }

            template <class T, class D, class G, class S>
            Inspect::Choice* AddChoice( const OS_SceneNodeDumbPtr& selection, const Reflect::Enumeration* enumInfo, G getter = NULL, S setter = NULL )
            {
                Inspect::ChoicePtr control = AddChoice<T, D, G, S>( selection, getter, setter );

                std::vector< Inspect::ChoiceItem > items;
                Reflect::V_EnumerationElement::const_iterator itr = enumInfo->m_Elements.begin();
                Reflect::V_EnumerationElement::const_iterator end = enumInfo->m_Elements.end();
                for ( ; itr != end; ++itr )
                {
                    tostringstream str;
                    str << (*itr)->m_Value;
                    items.push_back( Inspect::ChoiceItem ( (*itr)->m_Label, str.str() ) );
                }
                control->a_Items.Set(items);
                control->a_IsDropDown.Set(true);

                return control;
            }


            //
            // List
            //

            template <class T, class D, class G, class S>
            Inspect::List* AddList( const OS_SceneNodeDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::ListPtr control = CreateControl<Inspect::List>();
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }


            //
            // Slider
            //

            template <class T, class D, class G, class S>
            Inspect::Slider* AddSlider( const OS_SceneNodeDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::SliderPtr control = CreateControl<Inspect::Slider>();
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }


            //
            // ColorPicker
            //

            template <class T, class D, class G, class S>
            Inspect::ColorPicker* AddColorPicker( const OS_SceneNodeDumbPtr& selection, G getter = NULL, S setter = NULL )
            {
                Inspect::ColorPickerPtr control = CreateControl<Inspect::ColorPicker>();
                control->Bind( new Inspect::MultiPropertyStringFormatter<D> (BuildSelectionProperties<T, D, G, S> ( selection, getter, setter )) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }
        };

        typedef Helium::SmartPtr<PropertiesGenerator> GeneratorPtr;
    }
}