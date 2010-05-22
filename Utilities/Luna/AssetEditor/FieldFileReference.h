#pragma once

#include "FieldNode.h"

#include "File/File.h"

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
        File::ReferencePtr m_FileReference;
        bool m_UseLabelPrefix;

    public:
        // Runtime Type Info
        LUNA_DECLARE_TYPE( Luna::FieldFileReference, Luna::FieldNode );
        static void InitializeType();
        static void CleanupType();

    public:
        FieldFileReference( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field, const File::Reference& fileRef );
        virtual ~FieldFileReference();

        void SetUseLabelPrefix( bool useLabelPrefix );
        virtual File::ReferencePtr GetFileReference() const
        {
            return m_FileReference;
        }
        virtual void ActivateItem() NOC_OVERRIDE;

    protected:
        std::string MakeLabel() const;
        void PreferenceChanged( const Reflect::ElementChangeArgs& args );
    };

    typedef Nocturnal::SmartPtr< Luna::FieldFileReference > FieldFileReferencePtr;
}
