#include "Precompile.h"
#include "LooseAttachCollisionDialog.h"

// Virtual event handlers, overide them in your derived class
void LooseAttachCollisionDialog::OnAddAll( wxCommandEvent& event )
{
  unsigned int numItems = m_listUnselectedCollisionVolumes->GetCount();

  for( unsigned int index = 0; index < numItems; index++ )
  {
    m_listSelectedCollisionVolumes->Append( m_listUnselectedCollisionVolumes->GetString( index ) );
  }

  m_listUnselectedCollisionVolumes->Clear();
}

void LooseAttachCollisionDialog::OnAddSelected( wxCommandEvent& event )
{
  wxArrayInt selectedIndicies;

  int numSelections = m_listUnselectedCollisionVolumes->GetSelections( selectedIndicies );

  for( int index = 0; index < numSelections; index++ )
  {
    m_listSelectedCollisionVolumes->Append( m_listUnselectedCollisionVolumes->GetString( selectedIndicies[index] ) );
  }

  // remove items that were moved
  for( int count = 0; count < numSelections; count++ )
  {
    m_listUnselectedCollisionVolumes->Delete( selectedIndicies[count] - count );
  }

}

void LooseAttachCollisionDialog::OnRemoveSelected( wxCommandEvent& event )
{
  wxArrayInt selectedIndicies;

  int numSelections = m_listSelectedCollisionVolumes->GetSelections( selectedIndicies );

  for( int index = 0; index < numSelections; index++ )
  {
    m_listUnselectedCollisionVolumes->Append( m_listSelectedCollisionVolumes->GetString( selectedIndicies[index] ) );
  }

  // remove items that were moved
  for( int count = 0; count < numSelections; count++ )
  {
    m_listSelectedCollisionVolumes->Delete( selectedIndicies[count] - count );
  }
}

void LooseAttachCollisionDialog::OnRemoveAll( wxCommandEvent& event )
{
  unsigned int numItems = m_listSelectedCollisionVolumes->GetCount();

  for( unsigned int index = 0; index < numItems; index++ )
  {
    m_listUnselectedCollisionVolumes->Append( m_listSelectedCollisionVolumes->GetString( index ) );
  }

  m_listSelectedCollisionVolumes->Clear();
}