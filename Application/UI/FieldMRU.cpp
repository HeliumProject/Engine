#include "FieldMRU.h"
#include "Foundation/Container/Insert.h" 

#include <wx/ctrlsub.h>

using Nocturnal::Insert; 

using namespace Nocturnal;


//////////////////////////////////////////////////////////////////////////////
// doesn't do a damned thing actually
//
void FieldMRU::Initialize()
{
}

/////////////////////////////////////////////////////////////////////////////
// Delete all the ManagedStringSetPtr stored in m_Fields
//
void FieldMRU::Cleanup()
{
  if ( m_Fields.empty() )
    return;

  M_ManagedStringSet::iterator itField    = m_Fields.begin();
  M_ManagedStringSet::iterator itEndField = m_Fields.end();
  for ( ; itField != itEndField ; ++itField )
  {
    ManagedStringSetPtr curSet = itField->second;
    curSet = NULL;     
  }
}


/////////////////////////////////////////////////////////////////////////////
// Gets the MRU items set for the given field
// FIXME: this should be read from a file or the registry
//
ManagedStringSetPtr FieldMRU::GetFieldItems( const tstring& fieldKey, const tstring& defaultValue, const bool autoInit )
{
  ManagedStringSetPtr result = NULL;

  // see if it's already there
  M_ManagedStringSet::iterator found = m_Fields.find( fieldKey );
  if ( found != m_Fields.end() )
  {
    result = found->second;
  }
  // Auto-Init the ManagedStringSet if it's not there
  else if ( autoInit )
  {
    Insert<M_ManagedStringSet>::Result inserted = m_Fields.insert( M_ManagedStringSet::value_type( fieldKey, new ManagedStringSet() ) );

    if ( inserted.second )
    {
      // inserted               = std::pair< std::map<>::iterator, bool >
      // inserted.first         = std::map< tstring, ManagedStringSet >::iterator
      // inserted.first->second = ManagedStringSet
      result = inserted.first->second;

      if ( !defaultValue.empty() )
      {
        result->Insert( defaultValue );
      }
    }
  }

  return result;
}


/////////////////////////////////////////////////////////////////////////////
// Adds a new MRU value to the given field
//
bool FieldMRU::AddItem( wxControlWithItems* control, const tstring& fieldKey, const tstring& value )
{
  if ( value.empty() )
    return false;

  ManagedStringSetPtr typeSet = GetFieldItems( fieldKey );

  if ( !typeSet )
    return false;

  bool result = typeSet->Insert( value );

  PopulateControl( control, fieldKey );

  return result;
}


/////////////////////////////////////////////////////////////////////////////
// Populates the given control with the currently stored MRU strings.
//
void FieldMRU::PopulateControl( wxControlWithItems* control, const tstring& fieldKey, const tstring& defaultValue, const bool autoInit )
{
  ManagedStringSetPtr mruSet = GetFieldItems( fieldKey, defaultValue, autoInit );

  if ( !mruSet || !control ) 
    return;

  control->Clear();

  OS_string::ReverseIterator it    = mruSet->GetItems().ReverseBegin();
  OS_string::ReverseIterator itEnd = mruSet->GetItems().ReverseEnd();
  for ( ; it != itEnd ; ++it )
  {
    const tstring& value = (*it);

    if ( value.empty() )
      continue;

    control->Append( value.c_str() );
  }

  control->SetSelection( 0 );
}
