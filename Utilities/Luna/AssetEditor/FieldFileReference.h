#pragma once

#include "FieldNode.h"

#include "Foundation/File/Path.h"

// Forwards
namespace UIToolKit
{
    struct PreferenceChangedArgs;
}

namespace Luna
{
    /////////////////////////////////////////////////////////////////////////////
    // Node that represents a file reference, connected to a member variable on
    // an element.
    // 
    class FieldFileReference : public Luna::FieldNode
    {
    protected:
        Nocturnal::Path m_Path;
        bool m_UseLabelPrefix;

    public:
        // Runtime Type Info
        LUNA_DECLARE_TYPE( Luna::FieldFileReference, Luna::FieldNode );
        static void InitializeType();
        static void CleanupType();

    public:
        FieldFileReference( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field, const Nocturnal::Path& path );
        virtual ~FieldFileReference();

        void SetUseLabelPrefix( bool useLabelPrefix );
        virtual const Nocturnal::Path& GetPath() const
        {
            return m_Path;
        }
        virtual void ActivateItem() NOC_OVERRIDE;

    protected:
        std::string MakeLabel() const;
        void PreferenceChanged( const Reflect::ElementChangeArgs& args );
    };

    typedef Nocturnal::SmartPtr< Luna::FieldFileReference > FieldFileReferencePtr;
}
