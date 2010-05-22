#pragma once

#include "SearchGenerated.h"

#include "Attribute/AttributeCategory.h"
#include "Entity.h"
#include "Scene.h"
#include "SceneVisitor.h"
#include "Zone.h"
#include "TUID/TUID.h"

namespace Luna
{
  class SceneEditor;
  class SearchBarTraverser;
  class Zone;
  struct SceneChangeArgs;
  
  typedef std::map< tuid, ZonePtr > M_TuidToZone;
  typedef std::map< ScenePtr, ZonePtr > M_SceneToZone;

  namespace SearchOptions
  {
    enum SearchOption
    {
      Name,
      ID,
      EntityAsset,
      Count
    };
  }
  
  namespace BoundOptions
  {
    enum BoundOption
    {
      AABB,
      OBB,
      Count
    };
  }
  
  namespace ResultColumns
  {
    enum ResultColumn
    {
      Name,
      EntityAsset,
      Zone,
      Region,
      AssetType,
      Count
    };
  }

  class SearchBar : public SearchPanel
  {
  private:
    SceneEditor* m_SceneEditor;
    V_HierarchyNodeSmartPtr m_ResultNodes;
    Attribute::M_Attribute m_IndexToAttribute;

  public:
    SearchBar( SceneEditor* sceneEditor, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 362,34 ), long style = wxTAB_TRAVERSAL );
    virtual ~SearchBar();

  private:
    void OnSearch( wxCommandEvent& args );
    void OnSelect( wxCommandEvent& args );

    void OnSceneRemoving( const SceneChangeArgs& args );
    
    void SetupSearchCriteria( SearchBarTraverser& traverser );
    void SetupScenes( M_SceneToZone& sceneToZone, S_RegionDumbPtr& regionSet );
    void SearchScenes( const M_SceneToZone& sceneToZone, SearchBarTraverser& traverser );
    void DisplayResults( const M_SceneToZone& sceneToZone, const S_RegionDumbPtr& regionSet, SearchBarTraverser& traverser );
    void RefreshResults( const M_SceneToZone& sceneToZone, const S_RegionDumbPtr& regionSet );
    
    static bool CompareAttributes( const Attribute::AttributePtr& rhs, const Attribute::AttributePtr& lhs );
    static std::string WildcardToRegex( const std::string& str, bool partialMatch = true );
  };

  class SearchBarCriteria : public Nocturnal::RefCountBase<SearchBarCriteria>
  {
  public:
    virtual bool Validate( Luna::HierarchyNode* node ) = 0;
  };
  
  typedef Nocturnal::SmartPtr<SearchBarCriteria> SearchBarCriteriaPtr;
  typedef std::vector<SearchBarCriteriaPtr> V_SearchBarCriteria;

  class SearchBarTraverser : public HierarchyTraverser
  {
  public:
    SearchBarTraverser( SearchBar* searchBar )
    : m_SearchBar( searchBar )
    {
    }

    virtual TraversalAction VisitHierarchyNode( Luna::HierarchyNode* node ) NOC_OVERRIDE
    {
      if ( node->IsTransient() )
      {
        return TraversalActions::Continue;
      }
      
      V_SearchBarCriteria::iterator itr = m_SearchCriteria.begin();
      V_SearchBarCriteria::iterator end = m_SearchCriteria.end();
      for ( ; itr != end; ++itr )
      {
        if ( !(*itr)->Validate( node ) )
        {
          return TraversalActions::Continue;
        }
      }

      m_SearchResults.push_back( node );
      return TraversalActions::Continue;
    }
    
    void AddSearchCriteria( SearchBarCriteria* criteria )
    {
      m_SearchCriteria.push_back( criteria );
    }

    const V_HierarchyNodeSmartPtr& GetSearchResults()
    {
      return m_SearchResults;
    }
    
  private:
    SearchBar* m_SearchBar;
    V_SearchBarCriteria m_SearchCriteria;
    V_HierarchyNodeSmartPtr m_SearchResults;
  };
  
  template<class T>
  class ValueCriteria : public SearchBarCriteria
  {
  public:
    ValueCriteria( const T& value )
    : m_Value( value )
    {
    }
  
  protected:
    T m_Value;
  };
  
  class EntityNameCriteria : public ValueCriteria<std::string>
  {
  public:
    EntityNameCriteria( std::string entityName )
    : ValueCriteria( entityName )
    {
    }
    
    virtual bool Validate( Luna::HierarchyNode* node );
  };

  class EntityIDCriteria : public ValueCriteria<tuid>
  {
  public:
    EntityIDCriteria( tuid entityId )
    : ValueCriteria( entityId )
    {
    }
    
    virtual bool Validate( Luna::HierarchyNode* node );
  };

  class EntityAssetNameCriteria : public ValueCriteria<std::string>
  {
  public:
    EntityAssetNameCriteria( std::string entityClassName )
    : ValueCriteria( entityClassName )
    {
    }

    virtual bool Validate( Luna::HierarchyNode* node );
  };

  class AssetTypeCriteria : public ValueCriteria<int>
  {
  public:
    AssetTypeCriteria( int assetType )
    : ValueCriteria( assetType )
    {
    }
    
    virtual bool Validate( Luna::HierarchyNode* node );
  };
  
  class AABBCriteria : public SearchBarCriteria
  {
  public:
    AABBCriteria( float lowerBound, float upperBound )
    : m_LowerBound( lowerBound )
    , m_UpperBound( upperBound )
    {
    }
    
    virtual bool Validate( Luna::HierarchyNode* node );

  protected:
    float m_LowerBound;
    float m_UpperBound;
  };

  class OBBCriteria : public AABBCriteria
  {
  public:
    OBBCriteria( float lowerBound, float upperBound )
    : AABBCriteria( lowerBound, upperBound )
    {
    }

    virtual bool Validate( Luna::HierarchyNode* node );
  };
  
  class AttributeCriteria : public SearchBarCriteria
  {
  public:
    AttributeCriteria( const Attribute::V_Attribute& attributes )
    {
      m_Attributes = attributes;
    }
    
    virtual bool Validate( Luna::HierarchyNode* node );
  
  private:
    Attribute::V_Attribute m_Attributes;
  };
}
