#pragma once

#include "Pipeline/API.h"
#include "Foundation/Automation/Attribute.h"
#include "Foundation/Document/Document.h"
#include "Foundation/Reflect/Structure.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    class HELIUM_PIPELINE_API Project : public Reflect::Object
    {
    public:
        Project( const Path& path = TXT( "" ) );
        virtual ~Project();

        void ConnectDocument( Document* document );
        void DisconnectDocument( const Document* document );

        // Document and DocumentManager Events
        void OnDocumentOpened( const DocumentEventArgs& args );
        void OnDocumenClosed( const DocumentEventArgs& args );

        bool Serialize() const;

        const std::set< Path >& Paths()
        {
            return m_Paths;
        }

        Path GetTrackerDB() const;

        void AddPath( const Path& path );
        void RemovePath( const Path& path );

    public:
        Helium::Attribute< Path >    a_Path;
        Helium::Event< const Path& > e_PathAdded;
        Helium::Event< const Path& > e_PathRemoved;

        mutable DocumentObjectChangedSignature::Event e_HasChanged;

    protected:
        std::set< Path > m_Paths;

        void OnDocumentSave( const DocumentEventArgs& args );
        void OnDocumentPathChanged( const DocumentPathChangedArgs& args );
        void OnChildDocumentPathChanged( const DocumentPathChangedArgs& args );

    public:
        REFLECT_DECLARE_OBJECT( Project, Reflect::Object );
        static void PopulateComposite( Reflect::Composite& comp );
    };

    typedef Helium::StrongPtr<Project> ProjectPtr;
}