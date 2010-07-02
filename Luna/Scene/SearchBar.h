#pragma once

#include "SearchGenerated.h"

#include "Entity.h"
#include "Scene.h"
#include "SceneVisitor.h"
#include "Zone.h"
#include "Foundation/TUID.h"

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
    Component::M_Component m_IndexToComponent;

  public:
    SearchBar( SceneEditor* sceneEditor, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 362,34 ), long style = wxTAB_TRAVERSAL );
    virtual ~SearchBar();

  private:
    void OnSearch( wxCommandEvent& args );
    void OnSelect( wxCommandEvent& args );

    void OnSceneRemoving( const SceneChangeArgs& args );
    
    void SetupSearchCriteria( SearchBarTraverser& traverser );
    void SetupScenes( M_SceneToZone& sceneToZone );
    void SearchScenes( const M_SceneToZone& sceneToZone, SearchBarTraverser& traverser );
    void DisplayResults( const M_SceneToZone& sceneToZone, SearchBarTraverser& traverser );
    void RefreshResults( const M_SceneToZone& sceneToZone );
    
    static bool CompareComponents( const Component::ComponentPtr& rhs, const Component::ComponentPtr& lhs );
    static tstring WildcardToRegex( const tstring& str, bool partialMatch = true );
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
  
  class EntityNameCriteria : public ValueCriteria<tstring>
  {
  public:
    EntityNameCriteria( tstring entityName )
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

  class EntityAssetNameCriteria : public ValueCriteria<tstring>
  {
  public:
    EntityAssetNameCriteria( tstring entityClassName )
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
  
  class ComponentCriteria : public SearchBarCriteria
  {
  public:
    ComponentCriteria( const Component::V_Component& components )
    {
      m_Components = components;
    }
    
    virtual bool Validate( Luna::HierarchyNode* node );
  
  private:
    Component::V_Component m_Components;
  };
}
