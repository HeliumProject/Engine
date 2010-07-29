/* LiteSQL - Database implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
#include "litesql_char.hpp"
#include "compatibility.hpp"
#include "litesql/database.hpp"
#include "litesql/cursor.hpp"
#include "litesql/except.hpp"
#include "litesql/selectquery.hpp"
#include <map>
#include <algorithm>

namespace litesql {

void Database::openDatabase() {
   backend = Backend::getBackend(backendType,connInfo);
   if (backend==NULL)
      throw DatabaseError(LITESQL_L("Unknown backend: ") + backendType);
}

void Database::storeSchemaItem(const SchemaItem& s) const {
    delete_(LITESQL_L("schema_"), 
            RawExpr(LITESQL_L("name_='") + s.name 
                +  LITESQL_L("' and type_='") + s.type +  LITESQL_L("'")));
    Record values(3);

    values.push_back(s.name);
    values.push_back(s.type);
    values.push_back(s.sql);
    insert(LITESQL_L("schema_"), values);
}

std::vector<Database::SchemaItem> Database::getCurrentSchema() const {
    SelectQuery sel;
    sel.result(LITESQL_L("name_")).result(LITESQL_L("type_")).result(LITESQL_L("sql_")).source(LITESQL_L("schema_"));
    std::vector<SchemaItem> s;
    Records recs;
    try {
        recs = query(sel);
    } catch (Except e) {
        return std::vector<Database::SchemaItem>();
    }
    for (size_t i = 0; i < recs.size(); i++) {
        s.push_back(SchemaItem(recs[i][0], recs[i][1], recs[i][2]));
    }
    return s;
}

bool operator ==(const ColumnDefinition& c1,const ColumnDefinition& c2)
{
  return (c1.name == c2.name) && (c1.type==c2.type);
}

typedef std::vector<ColumnDefinition> ColumnDefinitions;

static ColumnDefinitions getFields(LITESQL_String schema) {
    ColumnDefinitions fields;
    int start = schema.find(LITESQL_L("("));
    int end = schema.find(LITESQL_L(")"));
    if (start == -1 || end == -1)
        return fields;
    Split tmp(replace(schema.substr(start+1, end-start-1),  LITESQL_L(", "),  LITESQL_L(",")),  LITESQL_L(","));
    
    ColumnDefinition field_def;
    for (size_t i = 0; i < tmp.size(); i++) 
    {
      Split s(tmp[i]);
      field_def.name = s[0];
      field_def.type = s[1];
      fields.push_back(field_def);
    }
    return fields;
}


bool Database::addColumn(const LITESQL_String & name,const ColumnDefinition & column_def) const 
{
  query(LITESQL_L("ALTER TABLE ") + name +  LITESQL_L(" ADD COLUMN ") + column_def.name + LITESQL_L(" ")+ column_def.type);
  return true;
}

void Database::upgradeTable(LITESQL_String name, 
                            LITESQL_String oldSchema, LITESQL_String newSchema) const {
    ColumnDefinitions oldFields = getFields(oldSchema);
    ColumnDefinitions newFields = getFields(newSchema);

    ColumnDefinitions toAdd(newFields);
    ColumnDefinitions::iterator found;

    for (ColumnDefinitions::iterator it = oldFields.begin();it!=oldFields.end();it++)
    {
      found = find_if(toAdd.begin(),toAdd.end(),ColumnDefinition::EqualName(it->name));
      if (found!=toAdd.end())
      {
        toAdd.erase(found);
      }
    }

    ColumnDefinitions commonFields;
    for (ColumnDefinitions::iterator it = oldFields.begin();it!=oldFields.end();it++)
    {
      found = find_if(newFields.begin(),newFields.end(),ColumnDefinition::EqualName(it->name));
      if (found!=newFields.end())
      {
        commonFields.push_back(*found);
      }
    }

    begin();
    LITESQL_String bkp_name(name+ LITESQL_L("backup"));
    query(LITESQL_L(" ALTER TABLE ") + name +  LITESQL_L(" RENAME TO ") + bkp_name); 
    for (ColumnDefinitions::iterator it = toAdd.begin();it!= toAdd.end();it++)
    {
      addColumn(bkp_name,*it);
    }

    query(newSchema);
    // oldfields as ...
    Split cols;
    LITESQL_String s;

    for (ColumnDefinitions::iterator it = commonFields.begin();it!= commonFields.end();it++)
    {
        s = it->name;
        s.append(LITESQL_L(" AS "));
        s.append(it->name);
        cols.push_back(s);
    }
    
    for (ColumnDefinitions::iterator it = toAdd.begin();it!= toAdd.end();it++)
    {
        s = it->name;
        s.append(LITESQL_L(" AS "));
        s.append(it->name);
        cols.push_back(s);
    }

    query(LITESQL_L(" INSERT INTO ") + name +  LITESQL_L(" SELECT ")+ cols.join(LITESQL_L(","))+ LITESQL_L(" FROM ") + bkp_name); 
    query(LITESQL_L(" DROP TABLE ") + bkp_name); 
    commit();
}

Database::Database(const LITESQL_String& backend, const LITESQL_String& conn) 
     : backendType(backend), connInfo(conn), backend(NULL), verbose(false) {
        openDatabase();
    }
    Database::Database(const Database &op) 
      : backendType(op.backendType), connInfo(op.connInfo), 
        verbose(op.verbose) {
        openDatabase();
    }
    Database::~Database() {
        delete backend;
    }
    void Database::create() const { 
        std::vector<SchemaItem> s = getSchema(); 
        begin();
        for (size_t i = 0; i < s.size(); i++) {
            query(s[i].sql);
            storeSchemaItem(s[i]);
        }
        commit();
    } 
    void Database::drop() const { 
        std::vector<SchemaItem> s = getSchema(); 

        for (size_t i = 0; i < s.size(); i++) {
            try {
                begin();
                if (s[i].type ==  LITESQL_L("table"))
                    query(LITESQL_L("DROP TABLE ") + s[i].name);
                else if (s[i].type ==  LITESQL_L("sequence"))
                    query(LITESQL_L("DROP SEQUENCE ") + s[i].name);
                commit();
            } catch (Except e) {
                rollback();
            }
        }
    }  
   
    bool Database::needsUpgrade() const {
        std::vector<SchemaItem> cs = getCurrentSchema();
        std::vector<SchemaItem> s = getSchema();
        std::map<LITESQL_String, int> items;
        for (size_t i = 0; i < cs.size(); i++) 
            items[cs[i].name] = i;
    
        for (size_t i = 0; i < s.size(); i++) {
            if (items.find(s[i].name) == items.end() 
                    || cs[items[s[i].name]].sql != s[i].sql) 
                return true;
        }
        return false;
    }
    void Database::upgrade() const {
        std::vector<SchemaItem> cs = getCurrentSchema();
        std::vector<SchemaItem> s = getSchema();
        std::map<LITESQL_String, int> items;
        for (size_t i = 0; i < cs.size(); i++) {
            items[cs[i].name] = i;
        }
        begin();
        for (size_t i = 0; i < s.size(); i++) {
            if (items.find(s[i].name) == items.end()) {
                query(s[i].sql);
                storeSchemaItem(s[i]);
                continue;
            }
            if (s[i].type ==  LITESQL_L("table") && cs[items[s[i].name]].sql != s[i].sql) {
                upgradeTable(s[i].name, cs[items[s[i].name]].sql, s[i].sql);
                storeSchemaItem(s[i]);
            }
        }
        commit();
    }
    Records Database::query(const LITESQL_String &q) const {
        if (verbose)
            LITESQL_cerr << q << std::endl;
        std::auto_ptr<Backend::Result> r(backend->execute(q));
        return r->records();
    }

    void Database::insert(const LITESQL_String &table, const Record &r,
                          const Split& fields) const {
        LITESQL_String command =  LITESQL_L("INSERT INTO ") + table;
        if (fields.size())
            command +=  LITESQL_L(" (") + fields.join(LITESQL_L(",")) +  LITESQL_L(")");
        command +=  LITESQL_L(" VALUES (");
        unsigned int i;
        for (i=0; i < r.size() -1; i++) {
            command += escapeSQL(r[i]) +  LITESQL_L(",");
        }
        command += escapeSQL(r[i]) +  LITESQL_L(")");
        query(command);
    }
    LITESQL_String Database::groupInsert(Record tables, Records fields, Records values, 
                     LITESQL_String sequence) const {
        if (verbose) {
            LITESQL_cerr <<  LITESQL_L("groupInsert") << std::endl;
            for (size_t i = 0; i < tables.size(); i++) {
                LITESQL_cerr <<  LITESQL_L("\t") << tables[i] << std::endl;
                LITESQL_cerr <<  LITESQL_L("\t\tfields : ") << Split::join(fields[i], LITESQL_L(",")) << std::endl;
                LITESQL_cerr <<  LITESQL_L("\t\tvalues : ") << Split::join(values[i], LITESQL_L("|")) << std::endl;
            }
        }
        return backend->groupInsert(tables, fields, values, sequence);
    }

    void Database::delete_(const LITESQL_String& table, const Expr& e) const {
        LITESQL_String where =  LITESQL_L("");
        if (e.asString() !=  LITESQL_L("True"))
            where =  LITESQL_L(" WHERE ") + e.asString();
        query(LITESQL_L("DELETE FROM ") + table + where);
    }

}
    