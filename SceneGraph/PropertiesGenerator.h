#pragma once

#include <string>
#include <map>

#include "Inspect/DataBinding.h"
#include "Inspect/Controls.h"
#include "Inspect/ReflectInterpreter.h"

#include "SceneGraph/API.h"
#include "SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HELIUM_SCENE_GRAPH_API PropertiesGenerator : public Inspect::ReflectInterpreter
        {
        public:
            PropertiesGenerator(Inspect::Container* container);
            virtual ~PropertiesGenerator();

            // init and cleanup generator system
            static void Initialize();
            static void Cleanup();

            // reset state
            void Reset();

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
            Inspect::Choice* AddChoice( const Reflect::MetaEnum* enumInfo, const Helium::SmartPtr< Helium::Property<T> >& property )
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

            template <class T>
            Inspect::FileDialogButton* AddFileDialogButton( const Helium::SmartPtr< Helium::Property<T> >& property )
            {
                Inspect::FileDialogButtonPtr control = CreateControl<Inspect::FileDialogButton>();
                control->Bind( new Inspect::PropertyStringFormatter<T> ( property ) );
                m_ContainerStack.Get().top()->AddChild(control);
                return control;
            }
        };

        typedef Helium::StrongPtr<PropertiesGenerator> GeneratorPtr;
    }
}