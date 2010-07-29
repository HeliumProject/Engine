/* LiteSQL 
 * 
 * By Tero Laitinen 
 * 
 * See LICENSE for copyright information. */

#ifndef litesql_expr_hpp
#define litesql_expr_hpp
#include "litesql_char.hpp"
#include <string>
#include <utility>
#include "litesql/utils.hpp"
#include "litesql/field.hpp"
/** \file expr.hpp 
    Contains Expr-class hierarchy and operator overloadings for them.*/
namespace litesql {
/** A base class for expression in WHERE - clause. 
    See \ref usage_selecting_persistents */
class Expr {
protected:
    // extra tables to be joined
    Split extraTables;
public:
  /// constant for True expression
  static const LITESQL_Char* True; 
    // default expression is true
    virtual LITESQL_String asString() const { return True; }


    Split getExtraTables() const { 
        return extraTables;
    }
    virtual ~Expr() {}
};
/** used to inject custom expression into WHERE-clause */
class RawExpr : public Expr {
    LITESQL_String expr;
public:
    RawExpr(LITESQL_String e) : expr(e) {}
    virtual LITESQL_String asString() const { return expr; }
};
/** used to connect two expressions */
class Connective : public Expr {
private:
    LITESQL_String op;
protected:
    const Expr &e1, &e2;
    
    Connective(LITESQL_String o, const Expr & e1_, const Expr & e2_)
        : op(o), e1(e1_), e2(e2_) { }
    
public:        
    virtual ~Connective() {}
    
    virtual LITESQL_String asString() const {
        LITESQL_String res =  LITESQL_L("(") + e1.asString() +  LITESQL_L(") ") + op 
            +  LITESQL_L(" (") + e2.asString() +  LITESQL_L(")");
        return res;
    }
};  
/** connects two expressions with and-operator. */
class And : public Connective {
public:
    And(const Expr & e1_, const Expr & e2_) : Connective(LITESQL_L("and"), e1_, e2_) {}
    virtual LITESQL_String asString() const {     
        if (e1.asString() == True)
            return e2.asString();
        else if (e2.asString() == True)
            return e1.asString();
        else
            return Connective::asString();
    }
};
/** connects two expression with or-operator. */
class Or : public Connective {
public:
    Or(const Expr & e1_, const Expr & e2_) 
        : Connective(LITESQL_L("or"), e1_, e2_) {}
    virtual LITESQL_String asString() const {
        if ((e1.asString() == True)||(e2.asString() == True))
            return True;
        else
            return Connective::asString();
    }
};
/** negates expression */
class Not : public Expr {
private:
    const Expr & exp;
public:
    Not(const Expr & _exp) : exp(_exp) {}
    virtual LITESQL_String asString() const {    
        return  LITESQL_L("not (")+exp.asString()+ LITESQL_L(")");
    }
        
};
/** base class of operators in expressions */
class Oper : public Expr {
protected:
    const FieldType & field;
    LITESQL_String op;
    LITESQL_String data;
    bool escape;
    
    Oper(const FieldType & fld, const LITESQL_String& o, const LITESQL_String& d) 
        : field(fld), op(o), data(d), escape(true) {
        extraTables.push_back(fld.table());
    }
    Oper(const FieldType & fld, const LITESQL_String& o, const FieldType &f2) 
        : field(fld), op(o), data(f2.fullName()), escape(false) {
        extraTables.push_back(fld.table());
    }

public:
    virtual LITESQL_String asString() const {
        LITESQL_String res;
        res += field.fullName() +  LITESQL_L(" ") + op +  LITESQL_L(" ") + (escape ? escapeSQL(data) : data);
        return res;
    }
};
/** equality operator */
class Eq : public Oper {
public:
    Eq(const FieldType & fld, const LITESQL_String& d)
        : Oper(fld,  LITESQL_L("="), d) {}
    Eq(const FieldType & fld, const FieldType & f2)
        : Oper(fld,  LITESQL_L("="), f2) {}

};
/** inequality operator */
class NotEq : public Oper {
public:
    NotEq(const FieldType & fld, const LITESQL_String& d)
        : Oper(fld,  LITESQL_L("<>"), d) {}
    NotEq(const FieldType & fld, const FieldType & f2)
        : Oper(fld,  LITESQL_L("<>"), f2) {
    }
   
};
/** greater than operator */
class Gt : public Oper {
public:
    Gt(const FieldType & fld, const LITESQL_String& d)
        : Oper(fld,  LITESQL_L(">"), d) {}
    Gt(const FieldType & fld, const FieldType& d)
        : Oper(fld,  LITESQL_L(">"), d) {}

};
/** greater or equal operator */
class GtEq : public Oper {
public:
    GtEq(const FieldType & fld, const LITESQL_String& d)
        : Oper(fld,  LITESQL_L(">="), d) {}
    GtEq(const FieldType & fld, const FieldType& d)
        : Oper(fld,  LITESQL_L(">="), d) {}

};
/** less than operator */
class Lt : public Oper {
public:
    Lt(const FieldType & fld, const LITESQL_String& d)
        : Oper(fld,  LITESQL_L("<"), d) {}
    Lt(const FieldType & fld, const FieldType& d)
        : Oper(fld,  LITESQL_L("<"), d) {}

};
/** less than or equal operator */
class LtEq : public Oper {
public:
    LtEq(const FieldType & fld, const LITESQL_String& d)
        : Oper(fld,  LITESQL_L("<="), d) {}
    LtEq(const FieldType & fld, const FieldType& d)
        : Oper(fld,  LITESQL_L("<="), d) {}

};
/** like operator */
class Like : public Oper {
public:
    Like(const FieldType & fld, const LITESQL_String& d)
        : Oper(fld,  LITESQL_L("like"), d) {}
};
class SelectQuery;
/** in operator */
class In : public Oper {
public:
    In(const FieldType & fld, const LITESQL_String& set)
        : Oper(fld,  LITESQL_L("in"),  LITESQL_L("(")+set+ LITESQL_L(")")) {};
    In(const FieldType & fld, const SelectQuery& s);
    virtual LITESQL_String asString() const {
        return field.fullName() +  LITESQL_L(" ") + op +  LITESQL_L(" ") + data;
    }
    
};
And operator&&(const Expr& o1, const Expr& o2);
Or operator||(const Expr& o1, const Expr& o2);
template <class T>
litesql::Eq operator==(const litesql::FieldType& fld, const T& o2) {
    return litesql::Eq(fld, litesql::toString(o2));
}
Eq operator==(const FieldType& fld, const FieldType& f2);
Gt operator>(const FieldType& fld, const FieldType& o2);
GtEq operator>=(const FieldType& fld, const FieldType& o2);
Lt operator<(const FieldType& fld, const FieldType& o2);
LtEq operator<=(const FieldType& fld, const FieldType& o2);
NotEq operator!=(const FieldType& fld, const FieldType& f2);

template <class T>
litesql::Gt operator>(const litesql::FieldType& fld, const T& o2) {
    return litesql::Gt(fld, litesql::toString(o2));
}

template <class T>
litesql::GtEq operator>=(const litesql::FieldType& fld, const T& o2) {
    return litesql::GtEq(fld, litesql::toString(o2));
}

template <class T>
litesql::Lt operator<(const litesql::FieldType& fld, const T& o2) {
    return litesql::Lt(fld, litesql::toString(o2));
}


template <class T>
litesql::LtEq operator<=(const litesql::FieldType& fld, const T& o2) {
    return litesql::LtEq(fld, litesql::toString(o2));
}
template <class T>
litesql::NotEq operator!=(const litesql::FieldType& fld, const T& o2) {
    return litesql::NotEq(fld, litesql::toString(o2));
}

Not operator!(const Expr &exp);
}


#endif