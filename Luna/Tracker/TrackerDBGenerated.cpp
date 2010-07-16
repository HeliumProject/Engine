#include "Precompile.h"

#include "TrackerDBGenerated.h"
namespace Luna {
    using namespace litesql;
    IndexData::Row::Row(const litesql::Database& db, const litesql::Record& rec)
        : mValue(IndexData::MValue), indexDatum(IndexData::IndexDatum), cacheEntry(IndexData::CacheEntry) {
            switch(rec.size()) {
    case 3:
        mValue = rec[2];
    case 2:
        indexDatum = rec[1];
    case 1:
        cacheEntry = rec[0];
            }
    }
    const tstring IndexData::table__( TXT( "CacheEntry_IndexDatum_" ));
    const litesql::FieldType IndexData::CacheEntry( TXT( "CacheEntry1" ), TXT( "INTEGER" ),table__);
    const litesql::FieldType IndexData::IndexDatum( TXT( "IndexDatum2" ), TXT( "INTEGER" ),table__);
    const litesql::FieldType IndexData::MValue( TXT( "mValue_" ), TXT( "TEXT" ),table__);
    void IndexData::link(const litesql::Database& db, const Luna::CacheEntry& o0, const Luna::IndexDatum& o1, tstring mValue) {
        Record values;
        Split fields;
        fields.push_back(CacheEntry.name());
        values.push_back(o0.id);
        fields.push_back(IndexDatum.name());
        values.push_back(o1.id);
        fields.push_back(MValue.name());
        values.push_back(mValue);
        db.insert(table__, values, fields);
    }
    void IndexData::unlink(const litesql::Database& db, const Luna::CacheEntry& o0, const Luna::IndexDatum& o1, tstring mValue) {
        db.delete_(table__, (CacheEntry == o0.id && IndexDatum == o1.id && (MValue == mValue)));
    }
    void IndexData::del(const litesql::Database& db, const litesql::Expr& expr) {
        db.delete_(table__, expr);
    }
    litesql::DataSource<IndexData::Row> IndexData::getRows(const litesql::Database& db, const litesql::Expr& expr) {
        SelectQuery sel;
        sel.result(CacheEntry.fullName());
        sel.result(IndexDatum.fullName());
        sel.result(MValue.fullName());
        sel.source(table__);
        sel.where(expr);
        return DataSource<IndexData::Row>(db, sel);
    }
    template <> litesql::DataSource<Luna::CacheEntry> IndexData::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
        SelectQuery sel;
        sel.source(table__);
        sel.result(CacheEntry.fullName());
        sel.where(srcExpr);
        return DataSource<Luna::CacheEntry>(db, Luna::CacheEntry::Id.in(sel) && expr);
    }
    template <> litesql::DataSource<Luna::IndexDatum> IndexData::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
        SelectQuery sel;
        sel.source(table__);
        sel.result(IndexDatum.fullName());
        sel.where(srcExpr);
        return DataSource<Luna::IndexDatum>(db, Luna::IndexDatum::Id.in(sel) && expr);
    }
    FileReferences::Row::Row(const litesql::Database& db, const litesql::Record& rec)
        : cacheEntry2(FileReferences::CacheEntry2), cacheEntry1(FileReferences::CacheEntry1) {
            switch(rec.size()) {
    case 2:
        cacheEntry2 = rec[1];
    case 1:
        cacheEntry1 = rec[0];
            }
    }
    const tstring FileReferences::table__( TXT( "CacheEntry_CacheEntry_" ));
    const litesql::FieldType FileReferences::CacheEntry1( TXT( "CacheEntry1" ), TXT( "INTEGER" ),table__);
    const litesql::FieldType FileReferences::CacheEntry2( TXT( "CacheEntry2" ), TXT( "INTEGER" ),table__);
    void FileReferences::link(const litesql::Database& db, const Luna::CacheEntry& o0, const Luna::CacheEntry& o1) {
        Record values;
        Split fields;
        fields.push_back(CacheEntry1.name());
        values.push_back(o0.id);
        fields.push_back(CacheEntry2.name());
        values.push_back(o1.id);
        db.insert(table__, values, fields);
    }
    void FileReferences::unlink(const litesql::Database& db, const Luna::CacheEntry& o0, const Luna::CacheEntry& o1) {
        db.delete_(table__, (CacheEntry1 == o0.id && CacheEntry2 == o1.id));
    }
    void FileReferences::del(const litesql::Database& db, const litesql::Expr& expr) {
        db.delete_(table__, expr);
    }
    litesql::DataSource<FileReferences::Row> FileReferences::getRows(const litesql::Database& db, const litesql::Expr& expr) {
        SelectQuery sel;
        sel.result(CacheEntry1.fullName());
        sel.result(CacheEntry2.fullName());
        sel.source(table__);
        sel.where(expr);
        return DataSource<FileReferences::Row>(db, sel);
    }
    litesql::DataSource<Luna::CacheEntry> FileReferences::getCacheEntry1(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
        SelectQuery sel;
        sel.source(table__);
        sel.result(CacheEntry1.fullName());
        sel.where(srcExpr);
        return DataSource<Luna::CacheEntry>(db, Luna::CacheEntry::Id.in(sel) && expr);
    }
    litesql::DataSource<Luna::CacheEntry> FileReferences::getCacheEntry2(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
        SelectQuery sel;
        sel.source(table__);
        sel.result(CacheEntry2.fullName());
        sel.where(srcExpr);
        return DataSource<Luna::CacheEntry>(db, Luna::CacheEntry::Id.in(sel) && expr);
    }
    const litesql::FieldType CacheEntry::Own::Id( TXT( "id_" ), TXT( "INTEGER" ), TXT( "CacheEntry_" ));
    CacheEntry::IndexdataHandle::IndexdataHandle(const CacheEntry& owner)
        : litesql::RelationHandle<CacheEntry>(owner) {
    }
    void CacheEntry::IndexdataHandle::link(const IndexDatum& o0, tstring mValue) {
        IndexData::link(owner->getDatabase(), *owner, o0, mValue);
    }
    void CacheEntry::IndexdataHandle::unlink(const IndexDatum& o0, tstring mValue) {
        IndexData::unlink(owner->getDatabase(), *owner, o0, mValue);
    }
    void CacheEntry::IndexdataHandle::del(const litesql::Expr& expr) {
        IndexData::del(owner->getDatabase(), expr && IndexData::CacheEntry == owner->id);
    }
    litesql::DataSource<IndexDatum> CacheEntry::IndexdataHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
        return IndexData::get<IndexDatum>(owner->getDatabase(), expr, (IndexData::CacheEntry == owner->id) && srcExpr);
    }
    litesql::DataSource<IndexData::Row> CacheEntry::IndexdataHandle::getRows(const litesql::Expr& expr) {
        return IndexData::getRows(owner->getDatabase(), expr && (IndexData::CacheEntry == owner->id));
    }
    CacheEntry::FileReferencesHandle::FileReferencesHandle(const CacheEntry& owner)
        : litesql::RelationHandle<CacheEntry>(owner) {
    }
    void CacheEntry::FileReferencesHandle::link(const CacheEntry& o0) {
        FileReferences::link(owner->getDatabase(), o0, *owner);
    }
    void CacheEntry::FileReferencesHandle::unlink(const CacheEntry& o0) {
        FileReferences::unlink(owner->getDatabase(), o0, *owner);
    }
    void CacheEntry::FileReferencesHandle::del(const litesql::Expr& expr) {
        FileReferences::del(owner->getDatabase(), expr && FileReferences::CacheEntry2 == owner->id);
    }
    litesql::DataSource<CacheEntry> CacheEntry::FileReferencesHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
        return FileReferences::getCacheEntry1(owner->getDatabase(), expr, (FileReferences::CacheEntry2 == owner->id) && srcExpr);
    }
    litesql::DataSource<FileReferences::Row> CacheEntry::FileReferencesHandle::getRows(const litesql::Expr& expr) {
        return FileReferences::getRows(owner->getDatabase(), expr && (FileReferences::CacheEntry2 == owner->id));
    }
    const tstring CacheEntry::type__( TXT( "CacheEntry" ));
    const tstring CacheEntry::table__( TXT( "CacheEntry_" ));
    const tstring CacheEntry::sequence__( TXT( "CacheEntry_seq" ));
    const litesql::FieldType CacheEntry::Id( TXT( "id_" ), TXT( "INTEGER" ),table__);
    const litesql::FieldType CacheEntry::Type( TXT( "type_" ), TXT( "TEXT" ),table__);
    const litesql::FieldType CacheEntry::MPath( TXT( "mPath_" ), TXT( "TEXT" ),table__);
    void CacheEntry::defaults() {
        id = 0;
    }
    CacheEntry::CacheEntry(const litesql::Database& db)
        : litesql::Persistent(db), id(Id), type(Type), mPath(MPath) {
            defaults();
    }
    CacheEntry::CacheEntry(const litesql::Database& db, const litesql::Record& rec)
        : litesql::Persistent(db, rec), id(Id), type(Type), mPath(MPath) {
            defaults();
            size_t size = (rec.size() > 3) ? 3 : rec.size();
            switch(size) {
    case 3: mPath = convert<const tstring&, tstring>(rec[2]);
        mPath.setModified(false);
    case 2: type = convert<const tstring&, tstring>(rec[1]);
        type.setModified(false);
    case 1: id = convert<const tstring&, int>(rec[0]);
        id.setModified(false);
            }
    }
    CacheEntry::CacheEntry(const CacheEntry& obj)
        : litesql::Persistent(obj), id(obj.id), type(obj.type), mPath(obj.mPath) {
    }
    const CacheEntry& CacheEntry::operator=(const CacheEntry& obj) {
        if (this != &obj) {
            id = obj.id;
            type = obj.type;
            mPath = obj.mPath;
        }
        litesql::Persistent::operator=(obj);
        return *this;
    }
    CacheEntry::IndexdataHandle CacheEntry::indexdata() {
        return CacheEntry::IndexdataHandle(*this);
    }
    CacheEntry::FileReferencesHandle CacheEntry::fileReferences() {
        return CacheEntry::FileReferencesHandle(*this);
    }
    tstring CacheEntry::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
        tables.push_back(table__);
        litesql::Record fields;
        litesql::Record values;
        fields.push_back(id.name());
        values.push_back(id);
        id.setModified(false);
        fields.push_back(type.name());
        values.push_back(type);
        type.setModified(false);
        fields.push_back(mPath.name());
        values.push_back(mPath);
        mPath.setModified(false);
        fieldRecs.push_back(fields);
        valueRecs.push_back(values);
        return litesql::Persistent::insert(tables, fieldRecs, valueRecs, sequence__);
    }
    void CacheEntry::create() {
        litesql::Record tables;
        litesql::Records fieldRecs;
        litesql::Records valueRecs;
        type = type__;
        tstring newID = insert(tables, fieldRecs, valueRecs);
        if (id == 0)
            id = newID;
    }
    void CacheEntry::addUpdates(Updates& updates) {
        prepareUpdate(updates, table__);
        updateField(updates, table__, id);
        updateField(updates, table__, type);
        updateField(updates, table__, mPath);
    }
    void CacheEntry::addIDUpdates(Updates& updates) {
    }
    void CacheEntry::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
        ftypes.push_back(Id);
        ftypes.push_back(Type);
        ftypes.push_back(MPath);
    }
    void CacheEntry::delRecord() {
        deleteFromTable(table__, id);
    }
    void CacheEntry::delRelations() {
        IndexData::del(*db, (IndexData::CacheEntry == id));
        FileReferences::del(*db, (FileReferences::CacheEntry1 == id) || (FileReferences::CacheEntry2 == id));
    }
    void CacheEntry::update() {
        if (!inDatabase) {
            create();
            return;
        }
        Updates updates;
        addUpdates(updates);
        if (id != oldKey) {
            if (!typeIsCorrect()) 
                upcastCopy()->addIDUpdates(updates);
        }
        litesql::Persistent::update(updates);
        oldKey = id;
    }
    void CacheEntry::del() {
        if (typeIsCorrect() == false) {
            std::auto_ptr<CacheEntry> p(upcastCopy());
            p->delRelations();
            p->onDelete();
            p->delRecord();
        } else {
            onDelete();
            delRecord();
        }
        inDatabase = false;
    }
    bool CacheEntry::typeIsCorrect() {
        return type == type__;
    }
    std::auto_ptr<CacheEntry> CacheEntry::upcast() {
        return auto_ptr<CacheEntry>(new CacheEntry(*this));
    }
    std::auto_ptr<CacheEntry> CacheEntry::upcastCopy() {
        CacheEntry* np = new CacheEntry(*this);
        np->id = id;
        np->type = type;
        np->mPath = mPath;
        np->inDatabase = inDatabase;
        return auto_ptr<CacheEntry>(np);
    }
    tostream & operator<<(tostream& os, CacheEntry o) {
        os <<  TXT( "-------------------------------------" ) << std::endl;
        os << o.id.name() <<  TXT( " = " ) << o.id << std::endl;
        os << o.type.name() <<  TXT( " = " ) << o.type << std::endl;
        os << o.mPath.name() <<  TXT( " = " ) << o.mPath << std::endl;
        os <<  TXT( "-------------------------------------" ) << std::endl;
        return os;
    }
    const litesql::FieldType IndexDatum::Own::Id( TXT( "id_" ), TXT( "INTEGER" ), TXT( "IndexDatum_" ));
    const tstring IndexDatum::type__( TXT( "IndexDatum" ));
    const tstring IndexDatum::table__( TXT( "IndexDatum_" ));
    const tstring IndexDatum::sequence__( TXT( "IndexDatum_seq" ));
    const litesql::FieldType IndexDatum::Id( TXT( "id_" ), TXT( "INTEGER" ),table__);
    const litesql::FieldType IndexDatum::Type( TXT( "type_" ), TXT( "TEXT" ),table__);
    const litesql::FieldType IndexDatum::MKey( TXT( "mKey_" ), TXT( "TEXT" ),table__);
    void IndexDatum::defaults() {
        id = 0;
    }
    IndexDatum::IndexDatum(const litesql::Database& db)
        : litesql::Persistent(db), id(Id), type(Type), mKey(MKey) {
            defaults();
    }
    IndexDatum::IndexDatum(const litesql::Database& db, const litesql::Record& rec)
        : litesql::Persistent(db, rec), id(Id), type(Type), mKey(MKey) {
            defaults();
            size_t size = (rec.size() > 3) ? 3 : rec.size();
            switch(size) {
    case 3: mKey = convert<const tstring&, tstring>(rec[2]);
        mKey.setModified(false);
    case 2: type = convert<const tstring&, tstring>(rec[1]);
        type.setModified(false);
    case 1: id = convert<const tstring&, int>(rec[0]);
        id.setModified(false);
            }
    }
    IndexDatum::IndexDatum(const IndexDatum& obj)
        : litesql::Persistent(obj), id(obj.id), type(obj.type), mKey(obj.mKey) {
    }
    const IndexDatum& IndexDatum::operator=(const IndexDatum& obj) {
        if (this != &obj) {
            id = obj.id;
            type = obj.type;
            mKey = obj.mKey;
        }
        litesql::Persistent::operator=(obj);
        return *this;
    }
    tstring IndexDatum::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
        tables.push_back(table__);
        litesql::Record fields;
        litesql::Record values;
        fields.push_back(id.name());
        values.push_back(id);
        id.setModified(false);
        fields.push_back(type.name());
        values.push_back(type);
        type.setModified(false);
        fields.push_back(mKey.name());
        values.push_back(mKey);
        mKey.setModified(false);
        fieldRecs.push_back(fields);
        valueRecs.push_back(values);
        return litesql::Persistent::insert(tables, fieldRecs, valueRecs, sequence__);
    }
    void IndexDatum::create() {
        litesql::Record tables;
        litesql::Records fieldRecs;
        litesql::Records valueRecs;
        type = type__;
        tstring newID = insert(tables, fieldRecs, valueRecs);
        if (id == 0)
            id = newID;
    }
    void IndexDatum::addUpdates(Updates& updates) {
        prepareUpdate(updates, table__);
        updateField(updates, table__, id);
        updateField(updates, table__, type);
        updateField(updates, table__, mKey);
    }
    void IndexDatum::addIDUpdates(Updates& updates) {
    }
    void IndexDatum::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
        ftypes.push_back(Id);
        ftypes.push_back(Type);
        ftypes.push_back(MKey);
    }
    void IndexDatum::delRecord() {
        deleteFromTable(table__, id);
    }
    void IndexDatum::delRelations() {
        IndexData::del(*db, (IndexData::IndexDatum == id));
    }
    void IndexDatum::update() {
        if (!inDatabase) {
            create();
            return;
        }
        Updates updates;
        addUpdates(updates);
        if (id != oldKey) {
            if (!typeIsCorrect()) 
                upcastCopy()->addIDUpdates(updates);
        }
        litesql::Persistent::update(updates);
        oldKey = id;
    }
    void IndexDatum::del() {
        if (typeIsCorrect() == false) {
            std::auto_ptr<IndexDatum> p(upcastCopy());
            p->delRelations();
            p->onDelete();
            p->delRecord();
        } else {
            onDelete();
            delRecord();
        }
        inDatabase = false;
    }
    bool IndexDatum::typeIsCorrect() {
        return type == type__;
    }
    std::auto_ptr<IndexDatum> IndexDatum::upcast() {
        return auto_ptr<IndexDatum>(new IndexDatum(*this));
    }
    std::auto_ptr<IndexDatum> IndexDatum::upcastCopy() {
        IndexDatum* np = new IndexDatum(*this);
        np->id = id;
        np->type = type;
        np->mKey = mKey;
        np->inDatabase = inDatabase;
        return auto_ptr<IndexDatum>(np);
    }
    tostream & operator<<(tostream& os, IndexDatum o) {
        os <<  TXT( "-------------------------------------" ) << std::endl;
        os << o.id.name() <<  TXT( " = " ) << o.id << std::endl;
        os << o.type.name() <<  TXT( " = " ) << o.type << std::endl;
        os << o.mKey.name() <<  TXT( " = " ) << o.mKey << std::endl;
        os <<  TXT( "-------------------------------------" ) << std::endl;
        return os;
    }
    TrackerDBGenerated::TrackerDBGenerated(tstring backendType, tstring connInfo)
        : litesql::Database(backendType, connInfo) {
            initialize();
    }
    std::vector<litesql::Database::SchemaItem> TrackerDBGenerated::getSchema() const {
        vector<Database::SchemaItem> res;
        res.push_back(Database::SchemaItem( TXT( "schema_" ), TXT( "table" ), TXT( "CREATE TABLE schema_ (name_ TEXT, type_ TEXT, sql_ TEXT);" )));
        if (backend->supportsSequences()) {
            res.push_back(Database::SchemaItem( TXT( "CacheEntry_seq" ), TXT( "sequence" ), TXT( "CREATE SEQUENCE CacheEntry_seq START 1 INCREMENT 1" )));
            res.push_back(Database::SchemaItem( TXT( "IndexDatum_seq" ), TXT( "sequence" ), TXT( "CREATE SEQUENCE IndexDatum_seq START 1 INCREMENT 1" )));
        }
        res.push_back(Database::SchemaItem( TXT( "CacheEntry_" ), TXT( "table" ), TXT( "CREATE TABLE CacheEntry_ (id_ " ) + backend->getRowIDType() +  TXT( ",type_ TEXT,mPath_ TEXT)" )));
        res.push_back(Database::SchemaItem( TXT( "IndexDatum_" ), TXT( "table" ), TXT( "CREATE TABLE IndexDatum_ (id_ " ) + backend->getRowIDType() +  TXT( ",type_ TEXT,mKey_ TEXT)" )));
        res.push_back(Database::SchemaItem( TXT( "CacheEntry_IndexDatum_" ), TXT( "table" ), TXT( "CREATE TABLE CacheEntry_IndexDatum_ (CacheEntry1 INTEGER,IndexDatum2 INTEGER,mValue_ TEXT)" )));
        res.push_back(Database::SchemaItem( TXT( "CacheEntry_CacheEntry_" ), TXT( "table" ), TXT( "CREATE TABLE CacheEntry_CacheEntry_ (CacheEntry1 INTEGER,CacheEntry2 INTEGER)" )));
        res.push_back(Database::SchemaItem( TXT( "_ee16a8c8d1f53bf3a2093e55bea35396" ), TXT( "index" ), TXT( "CREATE INDEX _ee16a8c8d1f53bf3a2093e55bea35396 ON CacheEntry_IndexDatum_ (CacheEntry1)" )));
        res.push_back(Database::SchemaItem( TXT( "_fa66951291eb42734f3737dbed9527c0" ), TXT( "index" ), TXT( "CREATE INDEX _fa66951291eb42734f3737dbed9527c0 ON CacheEntry_IndexDatum_ (IndexDatum2)" )));
        res.push_back(Database::SchemaItem( TXT( "CacheEntry_IndexDatum__all_idx" ), TXT( "index" ), TXT( "CREATE INDEX CacheEntry_IndexDatum__all_idx ON CacheEntry_IndexDatum_ (CacheEntry1,IndexDatum2)" )));
        res.push_back(Database::SchemaItem( TXT( "_c4f738887926a7088dbc1fa810bf521f" ), TXT( "index" ), TXT( "CREATE INDEX _c4f738887926a7088dbc1fa810bf521f ON CacheEntry_CacheEntry_ (CacheEntry1)" )));
        res.push_back(Database::SchemaItem( TXT( "_5476b9756e094357209814051c03d075" ), TXT( "index" ), TXT( "CREATE INDEX _5476b9756e094357209814051c03d075 ON CacheEntry_CacheEntry_ (CacheEntry2)" )));
        res.push_back(Database::SchemaItem( TXT( "CacheEntry_CacheEntry__all_idx" ), TXT( "index" ), TXT( "CREATE INDEX CacheEntry_CacheEntry__all_idx ON CacheEntry_CacheEntry_ (CacheEntry1,CacheEntry2)" )));
        return res;
    }
    void TrackerDBGenerated::initialize() {
        static bool initialized = false;
        if (initialized)
            return;
        initialized = true;
    }
}