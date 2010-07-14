#pragma once

#include "Platform/Types.h"

#ifndef assetcachedb_hpp
#define assetcachedb_hpp
#include "litesql.hpp"
#include "AssetCacheDB.h"
namespace Luna {
class CacheEntry;
class IndexDatum;
class IndexData {
public:
    class Row {
    public:
        litesql::Field<tstring> mValue;
        litesql::Field<int> indexDatum;
        litesql::Field<int> cacheEntry;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const tstring table__;
    static const litesql::FieldType CacheEntry;
    static const litesql::FieldType IndexDatum;
    static const litesql::FieldType MValue;
    static void link(const litesql::Database& db, const Luna::CacheEntry& o0, const Luna::IndexDatum& o1, tstring mValue= TXT( "" ));
    static void unlink(const litesql::Database& db, const Luna::CacheEntry& o0, const Luna::IndexDatum& o1, tstring mValue);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<IndexData::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    template <class T> static litesql::DataSource<T> get(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
;
;
};
class Dependencies {
public:
    class Row {
    public:
        litesql::Field<int> cacheEntry2;
        litesql::Field<int> cacheEntry1;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const tstring table__;
    static const litesql::FieldType CacheEntry1;
    static const litesql::FieldType CacheEntry2;
    static void link(const litesql::Database& db, const Luna::CacheEntry& o0, const Luna::CacheEntry& o1);
    static void unlink(const litesql::Database& db, const Luna::CacheEntry& o0, const Luna::CacheEntry& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<Dependencies::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<Luna::CacheEntry> getCacheEntry1(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
    static litesql::DataSource<Luna::CacheEntry> getCacheEntry2(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
};
class CacheEntry : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class IndexdataHandle : public litesql::RelationHandle<CacheEntry> {
    public:
        IndexdataHandle(const CacheEntry& owner);
        void link(const IndexDatum& o0, tstring mValue= TXT( "" ));
        void unlink(const IndexDatum& o0, tstring mValue);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<IndexDatum> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<IndexData::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    class DependenciesHandle : public litesql::RelationHandle<CacheEntry> {
    public:
        DependenciesHandle(const CacheEntry& owner);
        void link(const CacheEntry& o0);
        void unlink(const CacheEntry& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<CacheEntry> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<Dependencies::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const tstring type__;
    static const tstring table__;
    static const tstring sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<tstring> type;
    static const litesql::FieldType MPath;
    litesql::Field<tstring> mPath;
protected:
    void defaults();
public:
    CacheEntry(const litesql::Database& db);
    CacheEntry(const litesql::Database& db, const litesql::Record& rec);
    CacheEntry(const CacheEntry& obj);
    const CacheEntry& operator=(const CacheEntry& obj);
    CacheEntry::IndexdataHandle indexdata();
    CacheEntry::DependenciesHandle dependencies();
protected:
    tstring insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
    void create();
    virtual void addUpdates(Updates& updates);
    virtual void addIDUpdates(Updates& updates);
public:
    static void getFieldTypes(std::vector<litesql::FieldType>& ftypes);
protected:
    virtual void delRecord();
    virtual void delRelations();
public:
    virtual void update();
    virtual void del();
    virtual bool typeIsCorrect();
    std::auto_ptr<CacheEntry> upcast();
    std::auto_ptr<CacheEntry> upcastCopy();
};
tostream & operator<<(tostream& os, CacheEntry o);
class IndexDatum : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    static const tstring type__;
    static const tstring table__;
    static const tstring sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<tstring> type;
    static const litesql::FieldType MKey;
    litesql::Field<tstring> mKey;
protected:
    void defaults();
public:
    IndexDatum(const litesql::Database& db);
    IndexDatum(const litesql::Database& db, const litesql::Record& rec);
    IndexDatum(const IndexDatum& obj);
    const IndexDatum& operator=(const IndexDatum& obj);
protected:
    tstring insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
    void create();
    virtual void addUpdates(Updates& updates);
    virtual void addIDUpdates(Updates& updates);
public:
    static void getFieldTypes(std::vector<litesql::FieldType>& ftypes);
protected:
    virtual void delRecord();
    virtual void delRelations();
public:
    virtual void update();
    virtual void del();
    virtual bool typeIsCorrect();
    std::auto_ptr<IndexDatum> upcast();
    std::auto_ptr<IndexDatum> upcastCopy();
};
tostream & operator<<(tostream& os, IndexDatum o);
class AssetCacheDB : public litesql::Database {
public:
    AssetCacheDB(tstring backendType, tstring connInfo);
protected:
    virtual std::vector<litesql::Database::SchemaItem> getSchema() const;
    static void initialize();
};
}
#endif