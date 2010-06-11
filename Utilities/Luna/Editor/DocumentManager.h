#pragma once

#include "API.h"
#include "Document.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Container/OrderedSet.h"
#include "UID/TUID.h"

namespace Luna
{
  // Forwards
  class DocumentManager;

  struct DocumentManagerChangeArgs
  {
    DocumentManager*  m_Manager;
    Document*         m_Document;

    DocumentManagerChangeArgs (DocumentManager* manager, Document* document)
      : m_Manager( manager )
      , m_Document( document )
    {
    }
  };
  typedef Nocturnal::Signature< void, const DocumentManagerChangeArgs& > DocumentChangeSignature;

  // Enumeration of actions that can be requested during a save operation
  namespace SaveActions
  {
    enum SaveAction
    {
      Save,     // Carry out the save 
      SaveAll,  // Save this file and all others without prompting
      Skip,     // Don't save this file (it's already saved)
      SkipAll,  // Skip this file and all others without prompting
      Abort     // Don't save this file (and don't close it either)
    };
  }
  typedef SaveActions::SaveAction SaveAction;

  typedef Nocturnal::OrderedSet< DocumentPtr > OS_DocumentSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Manages a group of documents, providing convenience functions for saving,
  // closing, etc. all of them at once.
  // 
  class LUNA_EDITOR_API DocumentManager
  {
  private:
    OS_DocumentSmartPtr m_Documents;
    wxWindow*           m_ParentWindow;

  public:
    DocumentManager( wxWindow* parentWindow = NULL );
    virtual ~DocumentManager();

    const OS_DocumentSmartPtr& GetDocuments();

    bool Contains( const std::string& path ) const;

    virtual bool ValidateDocument( Document* document, std::string& error ) const;
    virtual bool ValidatePath( const std::string& path, std::string& error ) const;

    virtual DocumentPtr OpenPath( const std::string& path, std::string& error );

    virtual bool Save( DocumentPtr document, std::string& error );
    virtual bool SaveAll( std::string& error );

    virtual bool CloseAll();
    virtual bool CloseDocument( DocumentPtr document, bool prompt = true );
    bool CloseDocuments( OS_DocumentSmartPtr documents );

    bool QueryCheckOut( Document* document ) const;
    virtual bool IsCheckedOut( Document* document ) const;
    bool CheckOut( Document* document ) const;

    bool QueryAllowChanges( Document* document ) const;
    bool AttemptChanges( Document* document ) const;

    bool IsUpToDate( Document* document ) const;

    bool QueryAdd( Document* document ) const;
    bool QueryOpen( Document* document ) const;
    SaveAction QuerySave( Document* document ) const;
    SaveAction QueryClose( Document* document ) const;
    SaveAction QueryCloseAll( Document* document ) const;

  protected:
    bool AddDocument( const DocumentPtr& document );
    bool RemoveDocument( const DocumentPtr& document );
    Document* FindDocument( const std::string& path ) const;

  private:
    void OnDocumentClosed( const DocumentChangedArgs& args );

    //
    // Events
    //

  private:
    static DocumentChangeSignature::Event s_DocumentChange;
  public:
    static void AddDocumentChangeListener( DocumentChangeSignature::Delegate& delegate )
    {
      s_DocumentChange.Add( delegate );
    }
    static void RemoveDocumentChangeListener( DocumentChangeSignature::Delegate& delegate )
    {
      s_DocumentChange.Remove( delegate );
    }
  };
}
