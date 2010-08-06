#pragma once

#include <vector>

#include "Editor/API.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Reflect/Element.h"

#include "Editor/Selectable.h"
#include "Application/Undo/PropertyCommand.h"

namespace Helium
{
    namespace Editor
    {
        // callback for when (before and after) a selection changes
        typedef Helium::Signature< bool, const OS_SelectableDumbPtr& > SelectionChangingSignature;
        typedef Helium::Signature< void, const OS_SelectableDumbPtr& > SelectionChangedSignature;

        //
        // Selection
        //  This class makes no ties or associations to external UI
        //  If you need to extend the functionality of this class, patch in more events :)
        //

        class EDITOR_CORE_API Selection
        {
        private:
            //
            // Command for selection changes
            //

            class SelectionChangeCommand : public Undo::PropertyCommand< OS_SelectableDumbPtr >
            {
            public:
                SelectionChangeCommand( Selection* selection )
                    : Undo::PropertyCommand< OS_SelectableDumbPtr >( new Helium::MemberProperty< Selection, OS_SelectableDumbPtr > (selection, &Selection::GetUndo, &Selection::SetUndo) )
                {

                }

                virtual bool IsSignificant() const HELIUM_OVERRIDE
                {
                    return false;
                }
            };

            // The items of selected items
            OS_SelectableDumbPtr m_Items;

        public:
            // ctor doesn't do much
            Selection();

            // Emit changing events, but don't push undo items
            void Refresh();

            // Get selection to a tuple of objects
            const OS_SelectableDumbPtr& GetItems() const;

            // Clear the selection
            Undo::CommandPtr Clear(const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            // Set selection to a single object
            Undo::CommandPtr SetItem(Selectable* selection, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            // Set selection to a tuple of objects
            Undo::CommandPtr SetItems(const OS_SelectableDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            //Add single object to selection
            Undo::CommandPtr AddItem(Selectable* selection, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            //Add tuple of objects to the current selection
            Undo::CommandPtr AddItems( const OS_SelectableDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            // Remove single object from selection
            Undo::CommandPtr RemoveItem(Selectable* selection, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate () );

            // Remove tuple of objects from selection
            Undo::CommandPtr RemoveItems(const OS_SelectableDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate () );

            // Query containment of an individual object
            bool Contains(Selectable* selection) const;

        private:
            // Getter/Setter that matches the prototype required by the undo queue
            void GetUndo( OS_SelectableDumbPtr& outItems ) const;
            void SetUndo( const OS_SelectableDumbPtr& items );

        private:
            // fired before selection changes, with the exiting set of selected objects
            mutable SelectionChangingSignature::Event m_SelectionChanging;
        public:
            void AddChangingListener( const SelectionChangingSignature::Delegate& listener ) const
            {
                m_SelectionChanging.Add( listener );
            }
            void RemoveChangingListener( const SelectionChangingSignature::Delegate& listener ) const
            {
                m_SelectionChanging.Remove( listener );
            }

        private:
            // fired after selection changes, with the entering set of selected objects
            mutable SelectionChangedSignature::Event m_SelectionChanged;
        public:
            void AddChangedListener( const SelectionChangedSignature::Delegate& listener ) const
            {
                m_SelectionChanged.Add( listener );
            }
            void RemoveChangedListener( const SelectionChangedSignature::Delegate& listener ) const
            {
                m_SelectionChanged.Remove( listener );
            }
        };
    }
}