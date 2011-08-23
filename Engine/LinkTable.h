//----------------------------------------------------------------------------------------------------------------------
// LinkTable.h
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_OBJECT_PATH_H
#define HELIUM_ENGINE_OBJECT_PATH_H

#include "Engine/Engine.h"

#include "Platform/LinkTable.h"

// Satisfy pending link
// - Finish linking waiting object
// Fail pending link
// - Fail waiting object
// 


namespace Helium
{
    /// Hashed object path name for fast lookups and comparisons.
    class HELIUM_ENGINE_API LinkTable : NonCopyable
    {
        LinkTable();
        virtual ~LinkTable();


        // Called when an object references another (the object may or may not exist yet, but we
        // require that it *will* exist (i.e. we issued a load request for it)
        void AddPendingLink(GameObjectPath &_path, Helium::StrongPtr<GameObject> &_ptr)
        {
            _path.GetObjectPtr(_ptr);
            if (!_ptr.ReferencesObject())
            {
                _path.AddPendingLink()
            }
        }

        // Called as soon as an object is far enough in the loading process to have a pointer
        void OnObjectPreloaded(GameObjectPath &_path)
        {
            Helium::DynArray<PendingLink> &pendingLinks = _path.GetPendingLinks();
            for (Helium::DynArray<PendingLink>::Iterator iter = pendingLinks.Begin();
                iter != pendingLinks.End(); ++iter)
            {
                _path.GetObjectPath(iter->spPtr);
                iter->objectPath.OnLinkSatisfied();
            }
        }

        // Called if an object we expected to load will never exist (i.e. it may be referenced by
        // a pending link that is now waiting on this failed object to exist)
        void OnObjectPreloadFailed(GameObjectPath &_path)
        {

        }

        // Called when the pending link count on an object goes to zero
        void OnObjectLinked(GameObjectPath &_path)
        {

        }

        struct PendingLink
        {
            Helium::StrongPtr<GameObject> &rPtrPendingLink;
            GameObject *spTargetGameObject;
        };
    };
}

#include "Engine/LinkTable.inl"

#endif  // HELIUM_ENGINE_OBJECT_PATH_H
