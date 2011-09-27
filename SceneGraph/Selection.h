#pragma once

#include <vector>

#include "Foundation/Automation/Event.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Undo/PropertyCommand.h"

#include "SceneGraph/API.h"
#include "SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        struct SelectionChangeArgs
        {
            SelectionChangeArgs( const OS_SceneNodeDumbPtr& item )
                : m_Selection( item )
            {
            }

            const OS_SceneNodeDumbPtr& m_Selection;
        };

        struct SelectionChangingArgs : SelectionChangeArgs
        {
            SelectionChangingArgs( const OS_SceneNodeDumbPtr& item )
                : SelectionChangeArgs( item )
                , m_Veto( false )
            {
            }

            mutable bool m_Veto;
        };

        typedef Helium::Signature< const SelectionChangingArgs& > SelectionChangingSignature;
        typedef Helium::Signature< const SelectionChangeArgs& > SelectionChangedSignature;

        //
        // Selection
        //  This class makes no ties or associations to external UI
        //  If you need to extend the functionality of this class, patch in more events :)
        //

        class SCENE_GRAPH_API Selection
        {
        private:
            //
            // Command for item changes
            //

            class SelectionChangeCommand : public Undo::PropertyCommand< OS_SceneNodeDumbPtr >
            {
            public:
                SelectionChangeCommand( Selection* selection )
                    : Undo::PropertyCommand< OS_SceneNodeDumbPtr >( new Helium::MemberProperty< Selection, OS_SceneNodeDumbPtr > (selection, &Selection::GetUndo, &Selection::SetUndo) )
                {

                }

                virtual bool IsSignificant() const HELIUM_OVERRIDE
                {
                    return false;
                }
            };

            // The items of selected items
            OS_SceneNodeDumbPtr m_Items;

        public:
            // ctor doesn't do much
            Selection();

            // Emit changing events, but don't push undo items
            void Refresh();

            // Get item to a tuple of objects
            const OS_SceneNodeDumbPtr& GetItems() const;

            // Clear the item
            UndoCommandPtr Clear(const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            // Set item to a single object
            UndoCommandPtr SetItem(SceneNode* item, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            // Set item to a tuple of objects
            UndoCommandPtr SetItems(const OS_SceneNodeDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            //Add single object to item
            UndoCommandPtr AddItem(SceneNode* item, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            //Add tuple of objects to the current item
            UndoCommandPtr AddItems( const OS_SceneNodeDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate ());

            // Remove single object from item
            UndoCommandPtr RemoveItem(SceneNode* item, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate () );

            // Remove tuple of objects from item
            UndoCommandPtr RemoveItems(const OS_SceneNodeDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging = SelectionChangingSignature::Delegate (), const SelectionChangedSignature::Delegate& emitterChanged = SelectionChangedSignature::Delegate () );

            // Query containment of an individual object
            bool Contains(SceneNode* item) const;

        private:
            // Getter/Setter that matches the prototype required by the undo queue
            void GetUndo( OS_SceneNodeDumbPtr& outItems ) const;
            void SetUndo( const OS_SceneNodeDumbPtr& items );

        private:
            // fired before item changes, with the exiting set of selected objects
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
            // fired after item changes, with the entering set of selected objects
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