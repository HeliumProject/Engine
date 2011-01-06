#pragma once

#include "Pipeline/API.h"
#include "Foundation/Automation/Attribute.h"
#include "Foundation/Document/Document.h"
#include "Foundation/Reflect/DOM.h"

namespace Helium
{
    class PIPELINE_API Project : public Reflect::Element
    {
    public:
        REFLECT_DECLARE_CLASS( Project, Reflect::Element );

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
        static void AcceptCompositeVisitor( Reflect::Composite& comp )
        {
            comp.AddField( &This::a_Path, "Path", Reflect::FieldFlags::Discard );
            comp.AddField( &This::m_Paths, "m_Paths" );
        }
    };

    typedef Helium::StrongPtr<Project> ProjectPtr;
}