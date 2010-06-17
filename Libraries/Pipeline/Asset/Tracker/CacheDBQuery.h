#pragma once

#include "Pipeline/API.h"

#include "CacheDBColumn.h"

#include "Application/SQL/SQL.h"

namespace Asset
{
  /////////////////////////////////////////////////////////////////////////////
  // Word:
  //  - literal-word
  //
  // Phrase:
  //  - Word
  //  - "Double quoted group of Words"
  //  
  // ColumnAlias:
  //  - alias name of a searchable table column (must be 2 or more characters)
  //
  // ColumnQuery:
  //  - ColumnAlias : Phrase
  //  
  // Query:
  //  - Phrase
  //  - ColumnQuery
  
  /////////////////////////////////////////////////////////////////////////////
  class PIPELINE_API CacheDBExpr : public Nocturnal::RefCountBase< CacheDBExpr >
  {
  public:
    CacheDBExpr();
    virtual ~CacheDBExpr();

    bool NegateExpr() const { return m_NegateExpr; }
    void NegateExpr( const bool negateExpr ) { m_NegateExpr = negateExpr; }

    void SetBinaryOperator( const SQL::BinaryOperator binaryOp ) { m_BinaryOperator = binaryOp; }
    SQL::BinaryOperator GetBinaryOperator() const { return m_BinaryOperator; }

    virtual bool GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const = 0;

  protected:
    bool m_NegateExpr;
    SQL::BinaryOperator m_BinaryOperator;
  };
  PIPELINE_API typedef Nocturnal::SmartPtr< CacheDBExpr > CacheDBExprPtr;
  PIPELINE_API typedef std::vector< CacheDBExprPtr > V_CacheDBExpr;


  /////////////////////////////////////////////////////////////////////////////
  class PIPELINE_API CacheDBPhraseExpr : public CacheDBExpr
  {
  public:
    CacheDBPhraseExpr();
    virtual ~CacheDBPhraseExpr();

    void SetPhrase( const std::string& phrase );
    const std::string& GetPhrase() const { return m_Phrase; }

    virtual bool GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const NOC_OVERRIDE;

  protected:
    std::string m_Phrase;
  };
  PIPELINE_API typedef Nocturnal::SmartPtr< CacheDBPhraseExpr > CacheDBPhraseExprPtr;


  /////////////////////////////////////////////////////////////////////////////
  class PIPELINE_API CacheDBColumnExpr : public CacheDBPhraseExpr
  {
  public:
    CacheDBColumnExpr();
    virtual ~CacheDBColumnExpr();

    void SetColumnID( const CacheDBColumnID columnID );
    CacheDBColumnID GetColumnID() const { return m_ColumnID; }

    virtual bool GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const NOC_OVERRIDE;

  protected:
    CacheDBColumnID m_ColumnID;
  };
  PIPELINE_API typedef Nocturnal::SmartPtr< CacheDBColumnExpr > CacheDBColumnExprPtr;


  /////////////////////////////////////////////////////////////////////////////
  class PIPELINE_API CacheDBComponentExpr : public CacheDBPhraseExpr
  {
  public:
    CacheDBComponentExpr();
    virtual ~CacheDBComponentExpr();

    void SetComponentRowID( const u32 rowID ) { m_ComponentRowID = rowID; }
    u32 GetComponentRowID() const { return m_ComponentRowID; }

    virtual bool GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const NOC_OVERRIDE;

  protected:
    u32 m_ComponentRowID;
  };
  PIPELINE_API typedef Nocturnal::SmartPtr< CacheDBComponentExpr > CacheDBComponentExprPtr;


  /////////////////////////////////////////////////////////////////////////////
  class PIPELINE_API CacheDBQuery : public CacheDBExpr
  {
  public:
    CacheDBQuery();
    virtual ~CacheDBQuery();

    CacheDBExpr* AddExprs( CacheDBExpr* expr );
    const V_CacheDBExpr& GetExprs() const { return m_Exprs; }

    virtual bool GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const NOC_OVERRIDE;

  protected:
    V_CacheDBExpr m_Exprs;
  };
  PIPELINE_API typedef Nocturnal::SmartPtr< CacheDBQuery > CacheDBQueryPtr;

}