#include "exampledatabase.hpp"
namespace example {
using namespace litesql;
PersonPersonRelationMother::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : person2(PersonPersonRelationMother::Person2), person1(PersonPersonRelationMother::Person1) {
    switch(rec.size()) {
    case 2:
        person2 = rec[1];
    case 1:
        person1 = rec[0];
    }
}
const LITESQL_String PersonPersonRelationMother::table__( LITESQL_L( "Person_Person_Mother" ));
const litesql::FieldType PersonPersonRelationMother::Person1( LITESQL_L( "Person1" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType PersonPersonRelationMother::Person2( LITESQL_L( "Person2" ), LITESQL_L( "INTEGER" ),table__);
void PersonPersonRelationMother::link(const litesql::Database& db, const example::Person& o0, const example::Person& o1) {
    Record values;
    Split fields;
    fields.push_back(Person1.name());
    values.push_back(o0.id);
    fields.push_back(Person2.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void PersonPersonRelationMother::unlink(const litesql::Database& db, const example::Person& o0, const example::Person& o1) {
    db.delete_(table__, (Person1 == o0.id && Person2 == o1.id));
}
void PersonPersonRelationMother::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<PersonPersonRelationMother::Row> PersonPersonRelationMother::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Person1.fullName());
    sel.result(Person2.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<PersonPersonRelationMother::Row>(db, sel);
}
litesql::DataSource<example::Person> PersonPersonRelationMother::getPerson1(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person1.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
litesql::DataSource<example::Person> PersonPersonRelationMother::getPerson2(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person2.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
PersonPersonRelationFather::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : person2(PersonPersonRelationFather::Person2), person1(PersonPersonRelationFather::Person1) {
    switch(rec.size()) {
    case 2:
        person2 = rec[1];
    case 1:
        person1 = rec[0];
    }
}
const LITESQL_String PersonPersonRelationFather::table__( LITESQL_L( "Person_Person_Father" ));
const litesql::FieldType PersonPersonRelationFather::Person1( LITESQL_L( "Person1" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType PersonPersonRelationFather::Person2( LITESQL_L( "Person2" ), LITESQL_L( "INTEGER" ),table__);
void PersonPersonRelationFather::link(const litesql::Database& db, const example::Person& o0, const example::Person& o1) {
    Record values;
    Split fields;
    fields.push_back(Person1.name());
    values.push_back(o0.id);
    fields.push_back(Person2.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void PersonPersonRelationFather::unlink(const litesql::Database& db, const example::Person& o0, const example::Person& o1) {
    db.delete_(table__, (Person1 == o0.id && Person2 == o1.id));
}
void PersonPersonRelationFather::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<PersonPersonRelationFather::Row> PersonPersonRelationFather::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Person1.fullName());
    sel.result(Person2.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<PersonPersonRelationFather::Row>(db, sel);
}
litesql::DataSource<example::Person> PersonPersonRelationFather::getPerson1(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person1.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
litesql::DataSource<example::Person> PersonPersonRelationFather::getPerson2(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person2.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
PersonPersonRelationSiblings::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : person2(PersonPersonRelationSiblings::Person2), person1(PersonPersonRelationSiblings::Person1) {
    switch(rec.size()) {
    case 2:
        person2 = rec[1];
    case 1:
        person1 = rec[0];
    }
}
const LITESQL_String PersonPersonRelationSiblings::table__( LITESQL_L( "Person_Person_Siblings" ));
const litesql::FieldType PersonPersonRelationSiblings::Person1( LITESQL_L( "Person1" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType PersonPersonRelationSiblings::Person2( LITESQL_L( "Person2" ), LITESQL_L( "INTEGER" ),table__);
void PersonPersonRelationSiblings::link(const litesql::Database& db, const example::Person& o0, const example::Person& o1) {
    Record values;
    Split fields;
    fields.push_back(Person1.name());
    values.push_back(o0.id);
    fields.push_back(Person2.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
    fields.clear();
    values.clear();
    fields.push_back(Person1.name());
    values.push_back(o1.id);
    fields.push_back(Person2.name());
    values.push_back(o0.id);
    db.insert(table__, values, fields);
}
void PersonPersonRelationSiblings::unlink(const litesql::Database& db, const example::Person& o0, const example::Person& o1) {
    db.delete_(table__, (Person1 == o0.id && Person2 == o1.id));
    db.delete_(table__, (Person1 == o1.id && Person2 == o0.id));
}
void PersonPersonRelationSiblings::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<PersonPersonRelationSiblings::Row> PersonPersonRelationSiblings::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Person1.fullName());
    sel.result(Person2.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<PersonPersonRelationSiblings::Row>(db, sel);
}
litesql::DataSource<example::Person> PersonPersonRelationSiblings::getPerson1(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person1.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
litesql::DataSource<example::Person> PersonPersonRelationSiblings::getPerson2(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person2.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
PersonPersonRelationChildren::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : person2(PersonPersonRelationChildren::Person2), person1(PersonPersonRelationChildren::Person1) {
    switch(rec.size()) {
    case 2:
        person2 = rec[1];
    case 1:
        person1 = rec[0];
    }
}
const LITESQL_String PersonPersonRelationChildren::table__( LITESQL_L( "Person_Person_Children" ));
const litesql::FieldType PersonPersonRelationChildren::Person1( LITESQL_L( "Person1" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType PersonPersonRelationChildren::Person2( LITESQL_L( "Person2" ), LITESQL_L( "INTEGER" ),table__);
void PersonPersonRelationChildren::link(const litesql::Database& db, const example::Person& o0, const example::Person& o1) {
    Record values;
    Split fields;
    fields.push_back(Person1.name());
    values.push_back(o0.id);
    fields.push_back(Person2.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void PersonPersonRelationChildren::unlink(const litesql::Database& db, const example::Person& o0, const example::Person& o1) {
    db.delete_(table__, (Person1 == o0.id && Person2 == o1.id));
}
void PersonPersonRelationChildren::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<PersonPersonRelationChildren::Row> PersonPersonRelationChildren::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Person1.fullName());
    sel.result(Person2.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<PersonPersonRelationChildren::Row>(db, sel);
}
litesql::DataSource<example::Person> PersonPersonRelationChildren::getPerson1(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person1.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
litesql::DataSource<example::Person> PersonPersonRelationChildren::getPerson2(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person2.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
RoleRelation::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : role(RoleRelation::Role), person(RoleRelation::Person) {
    switch(rec.size()) {
    case 2:
        role = rec[1];
    case 1:
        person = rec[0];
    }
}
const LITESQL_String RoleRelation::table__( LITESQL_L( "Person_Role_Roles" ));
const litesql::FieldType RoleRelation::Person( LITESQL_L( "Person1" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType RoleRelation::Role( LITESQL_L( "Role2" ), LITESQL_L( "INTEGER" ),table__);
void RoleRelation::link(const litesql::Database& db, const example::Person& o0, const example::Role& o1) {
    Record values;
    Split fields;
    fields.push_back(Person.name());
    values.push_back(o0.id);
    fields.push_back(Role.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void RoleRelation::unlink(const litesql::Database& db, const example::Person& o0, const example::Role& o1) {
    db.delete_(table__, (Person == o0.id && Role == o1.id));
}
void RoleRelation::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<RoleRelation::Row> RoleRelation::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Person.fullName());
    sel.result(Role.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<RoleRelation::Row>(db, sel);
}
template <> litesql::DataSource<example::Person> RoleRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Person.fullName());
    sel.where(srcExpr);
    return DataSource<example::Person>(db, example::Person::Id.in(sel) && expr);
}
template <> litesql::DataSource<example::Role> RoleRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Role.fullName());
    sel.where(srcExpr);
    return DataSource<example::Role>(db, example::Role::Id.in(sel) && expr);
}
SchoolStudentRelation::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : student(SchoolStudentRelation::Student), school(SchoolStudentRelation::School) {
    switch(rec.size()) {
    case 2:
        student = rec[1];
    case 1:
        school = rec[0];
    }
}
const LITESQL_String SchoolStudentRelation::table__( LITESQL_L( "School_Student_" ));
const litesql::FieldType SchoolStudentRelation::School( LITESQL_L( "School1" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType SchoolStudentRelation::Student( LITESQL_L( "Student2" ), LITESQL_L( "INTEGER" ),table__);
void SchoolStudentRelation::link(const litesql::Database& db, const example::School& o0, const example::Student& o1) {
    Record values;
    Split fields;
    fields.push_back(School.name());
    values.push_back(o0.id);
    fields.push_back(Student.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void SchoolStudentRelation::unlink(const litesql::Database& db, const example::School& o0, const example::Student& o1) {
    db.delete_(table__, (School == o0.id && Student == o1.id));
}
void SchoolStudentRelation::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<SchoolStudentRelation::Row> SchoolStudentRelation::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(School.fullName());
    sel.result(Student.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<SchoolStudentRelation::Row>(db, sel);
}
template <> litesql::DataSource<example::School> SchoolStudentRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(School.fullName());
    sel.where(srcExpr);
    return DataSource<example::School>(db, example::School::Id.in(sel) && expr);
}
template <> litesql::DataSource<example::Student> SchoolStudentRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Student.fullName());
    sel.where(srcExpr);
    return DataSource<example::Student>(db, example::Student::Id.in(sel) && expr);
}
EmployeeOfficeRelation::Row::Row(const litesql::Database& db, const litesql::Record& rec)
         : office(EmployeeOfficeRelation::Office), employee(EmployeeOfficeRelation::Employee) {
    switch(rec.size()) {
    case 2:
        office = rec[1];
    case 1:
        employee = rec[0];
    }
}
const LITESQL_String EmployeeOfficeRelation::table__( LITESQL_L( "Employee_Office_" ));
const litesql::FieldType EmployeeOfficeRelation::Employee( LITESQL_L( "Employee1" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType EmployeeOfficeRelation::Office( LITESQL_L( "Office2" ), LITESQL_L( "INTEGER" ),table__);
void EmployeeOfficeRelation::link(const litesql::Database& db, const example::Employee& o0, const example::Office& o1) {
    Record values;
    Split fields;
    fields.push_back(Employee.name());
    values.push_back(o0.id);
    fields.push_back(Office.name());
    values.push_back(o1.id);
    db.insert(table__, values, fields);
}
void EmployeeOfficeRelation::unlink(const litesql::Database& db, const example::Employee& o0, const example::Office& o1) {
    db.delete_(table__, (Employee == o0.id && Office == o1.id));
}
void EmployeeOfficeRelation::del(const litesql::Database& db, const litesql::Expr& expr) {
    db.delete_(table__, expr);
}
litesql::DataSource<EmployeeOfficeRelation::Row> EmployeeOfficeRelation::getRows(const litesql::Database& db, const litesql::Expr& expr) {
    SelectQuery sel;
    sel.result(Employee.fullName());
    sel.result(Office.fullName());
    sel.source(table__);
    sel.where(expr);
    return DataSource<EmployeeOfficeRelation::Row>(db, sel);
}
template <> litesql::DataSource<example::Employee> EmployeeOfficeRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Employee.fullName());
    sel.where(srcExpr);
    return DataSource<example::Employee>(db, example::Employee::Id.in(sel) && expr);
}
template <> litesql::DataSource<example::Office> EmployeeOfficeRelation::get(const litesql::Database& db, const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    SelectQuery sel;
    sel.source(table__);
    sel.result(Office.fullName());
    sel.where(srcExpr);
    return DataSource<example::Office>(db, example::Office::Id.in(sel) && expr);
}
const litesql::FieldType user::Own::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ), LITESQL_L( "user_" ));
const LITESQL_String user::type__( LITESQL_L( "user" ));
const LITESQL_String user::table__( LITESQL_L( "user_" ));
const LITESQL_String user::sequence__( LITESQL_L( "user_seq" ));
const litesql::FieldType user::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType user::Type( LITESQL_L( "type_" ), LITESQL_L( "TEXT" ),table__);
const litesql::FieldType user::Name( LITESQL_L( "name_" ), LITESQL_L( "TEXT" ),table__);
const litesql::FieldType user::Passwd( LITESQL_L( "passwd_" ), LITESQL_L( "TEXT" ),table__);
void user::defaults() {
    id = 0;
}
user::user(const litesql::Database& db)
     : litesql::Persistent(db), id(Id), type(Type), name(Name), passwd(Passwd) {
    defaults();
}
user::user(const litesql::Database& db, const litesql::Record& rec)
     : litesql::Persistent(db, rec), id(Id), type(Type), name(Name), passwd(Passwd) {
    defaults();
    size_t size = (rec.size() > 4) ? 4 : rec.size();
    switch(size) {
    case 4: passwd = convert<const LITESQL_String&, LITESQL_String>(rec[3]);
        passwd.setModified(false);
    case 3: name = convert<const LITESQL_String&, LITESQL_String>(rec[2]);
        name.setModified(false);
    case 2: type = convert<const LITESQL_String&, LITESQL_String>(rec[1]);
        type.setModified(false);
    case 1: id = convert<const LITESQL_String&, int>(rec[0]);
        id.setModified(false);
    }
}
user::user(const user& obj)
     : litesql::Persistent(obj), id(obj.id), type(obj.type), name(obj.name), passwd(obj.passwd) {
}
const user& user::operator=(const user& obj) {
    if (this != &obj) {
        id = obj.id;
        type = obj.type;
        name = obj.name;
        passwd = obj.passwd;
    }
    litesql::Persistent::operator=(obj);
    return *this;
}
LITESQL_String user::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back(id.name());
    values.push_back(id);
    id.setModified(false);
    fields.push_back(type.name());
    values.push_back(type);
    type.setModified(false);
    fields.push_back(name.name());
    values.push_back(name);
    name.setModified(false);
    fields.push_back(passwd.name());
    values.push_back(passwd);
    passwd.setModified(false);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return litesql::Persistent::insert(tables, fieldRecs, valueRecs, sequence__);
}
void user::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    LITESQL_String newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void user::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
    updateField(updates, table__, type);
    updateField(updates, table__, name);
    updateField(updates, table__, passwd);
}
void user::addIDUpdates(Updates& updates) {
}
void user::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ftypes.push_back(Id);
    ftypes.push_back(Type);
    ftypes.push_back(Name);
    ftypes.push_back(Passwd);
}
void user::delRecord() {
    deleteFromTable(table__, id);
}
void user::delRelations() {
}
void user::update() {
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
void user::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<user> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool user::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<user> user::upcast() {
    return std::auto_ptr<user>(new user(*this));
}
std::auto_ptr<user> user::upcastCopy() {
    user* np = new user(*this);
    np->id = id;
    np->type = type;
    np->name = name;
    np->passwd = passwd;
    np->inDatabase = inDatabase;
    return std::auto_ptr<user>(np);
}
LITESQL_oStream & operator<<(LITESQL_oStream& os, user o) {
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    os << o.id.name() <<  LITESQL_L( " = " ) << o.id << std::endl;
    os << o.type.name() <<  LITESQL_L( " = " ) << o.type << std::endl;
    os << o.name.name() <<  LITESQL_L( " = " ) << o.name << std::endl;
    os << o.passwd.name() <<  LITESQL_L( " = " ) << o.passwd << std::endl;
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    return os;
}
const litesql::FieldType Person::Own::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ), LITESQL_L( "Person_" ));
const int Person::SexType::Male(0);
const int Person::SexType::Female(1);
Person::SexType::SexType(const LITESQL_String& n, const LITESQL_String& t, const LITESQL_String& tbl, const litesql::FieldType::Values& vals)
         : litesql::FieldType(n,t,tbl,vals) {
}
const int Person::Sex::Male(0);
const int Person::Sex::Female(1);
Person::MotherHandle::MotherHandle(const Person& owner)
         : litesql::RelationHandle<Person>(owner) {
}
void Person::MotherHandle::link(const Person& o0) {
    PersonPersonRelationMother::link(owner->getDatabase(), *owner, o0);
}
void Person::MotherHandle::unlink(const Person& o0) {
    PersonPersonRelationMother::unlink(owner->getDatabase(), *owner, o0);
}
void Person::MotherHandle::del(const litesql::Expr& expr) {
    PersonPersonRelationMother::del(owner->getDatabase(), expr && PersonPersonRelationMother::Person1 == owner->id);
}
litesql::DataSource<Person> Person::MotherHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return PersonPersonRelationMother::getPerson2(owner->getDatabase(), expr, (PersonPersonRelationMother::Person1 == owner->id) && srcExpr);
}
litesql::DataSource<PersonPersonRelationMother::Row> Person::MotherHandle::getRows(const litesql::Expr& expr) {
    return PersonPersonRelationMother::getRows(owner->getDatabase(), expr && (PersonPersonRelationMother::Person1 == owner->id));
}
Person::FatherHandle::FatherHandle(const Person& owner)
         : litesql::RelationHandle<Person>(owner) {
}
void Person::FatherHandle::link(const Person& o0) {
    PersonPersonRelationFather::link(owner->getDatabase(), *owner, o0);
}
void Person::FatherHandle::unlink(const Person& o0) {
    PersonPersonRelationFather::unlink(owner->getDatabase(), *owner, o0);
}
void Person::FatherHandle::del(const litesql::Expr& expr) {
    PersonPersonRelationFather::del(owner->getDatabase(), expr && PersonPersonRelationFather::Person1 == owner->id);
}
litesql::DataSource<Person> Person::FatherHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return PersonPersonRelationFather::getPerson2(owner->getDatabase(), expr, (PersonPersonRelationFather::Person1 == owner->id) && srcExpr);
}
litesql::DataSource<PersonPersonRelationFather::Row> Person::FatherHandle::getRows(const litesql::Expr& expr) {
    return PersonPersonRelationFather::getRows(owner->getDatabase(), expr && (PersonPersonRelationFather::Person1 == owner->id));
}
Person::SiblingsHandle::SiblingsHandle(const Person& owner)
         : litesql::RelationHandle<Person>(owner) {
}
void Person::SiblingsHandle::link(const Person& o0) {
    PersonPersonRelationSiblings::link(owner->getDatabase(), *owner, o0);
}
void Person::SiblingsHandle::unlink(const Person& o0) {
    PersonPersonRelationSiblings::unlink(owner->getDatabase(), *owner, o0);
}
void Person::SiblingsHandle::del(const litesql::Expr& expr) {
    PersonPersonRelationSiblings::del(owner->getDatabase(), expr && PersonPersonRelationSiblings::Person1 == owner->id);
}
litesql::DataSource<Person> Person::SiblingsHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return PersonPersonRelationSiblings::getPerson2(owner->getDatabase(), expr, (PersonPersonRelationSiblings::Person1 == owner->id) && srcExpr);
}
litesql::DataSource<PersonPersonRelationSiblings::Row> Person::SiblingsHandle::getRows(const litesql::Expr& expr) {
    return PersonPersonRelationSiblings::getRows(owner->getDatabase(), expr && (PersonPersonRelationSiblings::Person1 == owner->id));
}
Person::ChildrenHandle::ChildrenHandle(const Person& owner)
         : litesql::RelationHandle<Person>(owner) {
}
void Person::ChildrenHandle::link(const Person& o0) {
    PersonPersonRelationChildren::link(owner->getDatabase(), *owner, o0);
}
void Person::ChildrenHandle::unlink(const Person& o0) {
    PersonPersonRelationChildren::unlink(owner->getDatabase(), *owner, o0);
}
void Person::ChildrenHandle::del(const litesql::Expr& expr) {
    PersonPersonRelationChildren::del(owner->getDatabase(), expr && PersonPersonRelationChildren::Person1 == owner->id);
}
litesql::DataSource<Person> Person::ChildrenHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return PersonPersonRelationChildren::getPerson2(owner->getDatabase(), expr, (PersonPersonRelationChildren::Person1 == owner->id) && srcExpr);
}
litesql::DataSource<PersonPersonRelationChildren::Row> Person::ChildrenHandle::getRows(const litesql::Expr& expr) {
    return PersonPersonRelationChildren::getRows(owner->getDatabase(), expr && (PersonPersonRelationChildren::Person1 == owner->id));
}
Person::RolesHandle::RolesHandle(const Person& owner)
         : litesql::RelationHandle<Person>(owner) {
}
void Person::RolesHandle::link(const Role& o0) {
    RoleRelation::link(owner->getDatabase(), *owner, o0);
}
void Person::RolesHandle::unlink(const Role& o0) {
    RoleRelation::unlink(owner->getDatabase(), *owner, o0);
}
void Person::RolesHandle::del(const litesql::Expr& expr) {
    RoleRelation::del(owner->getDatabase(), expr && RoleRelation::Person == owner->id);
}
litesql::DataSource<Role> Person::RolesHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return RoleRelation::get<Role>(owner->getDatabase(), expr, (RoleRelation::Person == owner->id) && srcExpr);
}
litesql::DataSource<RoleRelation::Row> Person::RolesHandle::getRows(const litesql::Expr& expr) {
    return RoleRelation::getRows(owner->getDatabase(), expr && (RoleRelation::Person == owner->id));
}
const LITESQL_String Person::type__( LITESQL_L( "Person" ));
const LITESQL_String Person::table__( LITESQL_L( "Person_" ));
const LITESQL_String Person::sequence__( LITESQL_L( "Person_seq" ));
const litesql::FieldType Person::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType Person::Type( LITESQL_L( "type_" ), LITESQL_L( "TEXT" ),table__);
const litesql::FieldType Person::Name( LITESQL_L( "name_" ), LITESQL_L( "TEXT" ),table__);
const litesql::FieldType Person::Age( LITESQL_L( "age_" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType Person::Image( LITESQL_L( "image_" ), LITESQL_L( "BLOB" ),table__);
const litesql::FieldType Person::ADoubleValue( LITESQL_L( "aDoubleValue_" ), LITESQL_L( "DOUBLE" ),table__);
std::vector < std::pair< LITESQL_String, LITESQL_String > > Person::sex_values;
const Person::SexType Person::Sex( LITESQL_L( "sex_" ), LITESQL_L( "INTEGER" ),table__,sex_values);
void Person::initValues() {
    sex_values.clear();
    sex_values.push_back(make_pair<LITESQL_String, LITESQL_String>( LITESQL_L( "Male" ), LITESQL_L( "0" )));
    sex_values.push_back(make_pair<LITESQL_String, LITESQL_String>( LITESQL_L( "Female" ), LITESQL_L( "1" )));
}
void Person::defaults() {
    id = 0;
    age = 15;
    image = Blob::nil;
    aDoubleValue = 0.0;
    sex = 0;
}
Person::Person(const litesql::Database& db)
     : litesql::Persistent(db), id(Id), type(Type), name(Name), age(Age), image(Image), aDoubleValue(ADoubleValue), sex(Sex) {
    defaults();
}
Person::Person(const litesql::Database& db, const litesql::Record& rec)
     : litesql::Persistent(db, rec), id(Id), type(Type), name(Name), age(Age), image(Image), aDoubleValue(ADoubleValue), sex(Sex) {
    defaults();
    size_t size = (rec.size() > 7) ? 7 : rec.size();
    switch(size) {
    case 7: sex = convert<const LITESQL_String&, int>(rec[6]);
        sex.setModified(false);
    case 6: aDoubleValue = convert<const LITESQL_String&, double>(rec[5]);
        aDoubleValue.setModified(false);
    case 5: image = convert<const LITESQL_String&, litesql::Blob>(rec[4]);
        image.setModified(false);
    case 4: age = convert<const LITESQL_String&, int>(rec[3]);
        age.setModified(false);
    case 3: name = convert<const LITESQL_String&, LITESQL_String>(rec[2]);
        name.setModified(false);
    case 2: type = convert<const LITESQL_String&, LITESQL_String>(rec[1]);
        type.setModified(false);
    case 1: id = convert<const LITESQL_String&, int>(rec[0]);
        id.setModified(false);
    }
}
Person::Person(const Person& obj)
     : litesql::Persistent(obj), id(obj.id), type(obj.type), name(obj.name), age(obj.age), image(obj.image), aDoubleValue(obj.aDoubleValue), sex(obj.sex) {
}
const Person& Person::operator=(const Person& obj) {
    if (this != &obj) {
        id = obj.id;
        type = obj.type;
        name = obj.name;
        age = obj.age;
        image = obj.image;
        aDoubleValue = obj.aDoubleValue;
        sex = obj.sex;
    }
    litesql::Persistent::operator=(obj);
    return *this;
}
Person::MotherHandle Person::mother() {
    return Person::MotherHandle(*this);
}
Person::FatherHandle Person::father() {
    return Person::FatherHandle(*this);
}
Person::SiblingsHandle Person::siblings() {
    return Person::SiblingsHandle(*this);
}
Person::ChildrenHandle Person::children() {
    return Person::ChildrenHandle(*this);
}
Person::RolesHandle Person::roles() {
    return Person::RolesHandle(*this);
}
LITESQL_String Person::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back(id.name());
    values.push_back(id);
    id.setModified(false);
    fields.push_back(type.name());
    values.push_back(type);
    type.setModified(false);
    fields.push_back(name.name());
    values.push_back(name);
    name.setModified(false);
    fields.push_back(age.name());
    values.push_back(age);
    age.setModified(false);
    fields.push_back(image.name());
    values.push_back(image);
    image.setModified(false);
    fields.push_back(aDoubleValue.name());
    values.push_back(aDoubleValue);
    aDoubleValue.setModified(false);
    fields.push_back(sex.name());
    values.push_back(sex);
    sex.setModified(false);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return litesql::Persistent::insert(tables, fieldRecs, valueRecs, sequence__);
}
void Person::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    LITESQL_String newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void Person::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
    updateField(updates, table__, type);
    updateField(updates, table__, name);
    updateField(updates, table__, age);
    updateField(updates, table__, image);
    updateField(updates, table__, aDoubleValue);
    updateField(updates, table__, sex);
}
void Person::addIDUpdates(Updates& updates) {
}
void Person::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ftypes.push_back(Id);
    ftypes.push_back(Type);
    ftypes.push_back(Name);
    ftypes.push_back(Age);
    ftypes.push_back(Image);
    ftypes.push_back(ADoubleValue);
    ftypes.push_back(Sex);
}
void Person::delRecord() {
    deleteFromTable(table__, id);
}
void Person::delRelations() {
    PersonPersonRelationMother::del(*db, (PersonPersonRelationMother::Person1 == id) || (PersonPersonRelationMother::Person2 == id));
    PersonPersonRelationFather::del(*db, (PersonPersonRelationFather::Person1 == id) || (PersonPersonRelationFather::Person2 == id));
    PersonPersonRelationSiblings::del(*db, (PersonPersonRelationSiblings::Person1 == id) || (PersonPersonRelationSiblings::Person2 == id));
    RoleRelation::del(*db, (RoleRelation::Person == id));
    PersonPersonRelationChildren::del(*db, (PersonPersonRelationChildren::Person1 == id) || (PersonPersonRelationChildren::Person2 == id));
}
void Person::update() {
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
void Person::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<Person> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool Person::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<Person> Person::upcast() {
    return std::auto_ptr<Person>(new Person(*this));
}
std::auto_ptr<Person> Person::upcastCopy() {
    Person* np = new Person(*this);
    np->id = id;
    np->type = type;
    np->name = name;
    np->age = age;
    np->image = image;
    np->aDoubleValue = aDoubleValue;
    np->sex = sex;
    np->inDatabase = inDatabase;
    return std::auto_ptr<Person>(np);
}
LITESQL_oStream & operator<<(LITESQL_oStream& os, Person o) {
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    os << o.id.name() <<  LITESQL_L( " = " ) << o.id << std::endl;
    os << o.type.name() <<  LITESQL_L( " = " ) << o.type << std::endl;
    os << o.name.name() <<  LITESQL_L( " = " ) << o.name << std::endl;
    os << o.age.name() <<  LITESQL_L( " = " ) << o.age << std::endl;
    os << o.image.name() <<  LITESQL_L( " = " ) << o.image << std::endl;
    os << o.aDoubleValue.name() <<  LITESQL_L( " = " ) << o.aDoubleValue << std::endl;
    os << o.sex.name() <<  LITESQL_L( " = " ) << o.sex << std::endl;
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    return os;
}
const litesql::FieldType Role::Own::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ), LITESQL_L( "Role_" ));
Role::PersonHandle::PersonHandle(const Role& owner)
         : litesql::RelationHandle<Role>(owner) {
}
void Role::PersonHandle::link(const Person& o0) {
    RoleRelation::link(owner->getDatabase(), o0, *owner);
}
void Role::PersonHandle::unlink(const Person& o0) {
    RoleRelation::unlink(owner->getDatabase(), o0, *owner);
}
void Role::PersonHandle::del(const litesql::Expr& expr) {
    RoleRelation::del(owner->getDatabase(), expr && RoleRelation::Role == owner->id);
}
litesql::DataSource<Person> Role::PersonHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return RoleRelation::get<Person>(owner->getDatabase(), expr, (RoleRelation::Role == owner->id) && srcExpr);
}
litesql::DataSource<RoleRelation::Row> Role::PersonHandle::getRows(const litesql::Expr& expr) {
    return RoleRelation::getRows(owner->getDatabase(), expr && (RoleRelation::Role == owner->id));
}
const LITESQL_String Role::type__( LITESQL_L( "Role" ));
const LITESQL_String Role::table__( LITESQL_L( "Role_" ));
const LITESQL_String Role::sequence__( LITESQL_L( "Role_seq" ));
const litesql::FieldType Role::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType Role::Type( LITESQL_L( "type_" ), LITESQL_L( "TEXT" ),table__);
void Role::defaults() {
    id = 0;
}
Role::Role(const litesql::Database& db)
     : litesql::Persistent(db), id(Id), type(Type) {
    defaults();
}
Role::Role(const litesql::Database& db, const litesql::Record& rec)
     : litesql::Persistent(db, rec), id(Id), type(Type) {
    defaults();
    size_t size = (rec.size() > 2) ? 2 : rec.size();
    switch(size) {
    case 2: type = convert<const LITESQL_String&, LITESQL_String>(rec[1]);
        type.setModified(false);
    case 1: id = convert<const LITESQL_String&, int>(rec[0]);
        id.setModified(false);
    }
}
Role::Role(const Role& obj)
     : litesql::Persistent(obj), id(obj.id), type(obj.type) {
}
const Role& Role::operator=(const Role& obj) {
    if (this != &obj) {
        id = obj.id;
        type = obj.type;
    }
    litesql::Persistent::operator=(obj);
    return *this;
}
Role::PersonHandle Role::person() {
    return Role::PersonHandle(*this);
}
LITESQL_String Role::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back(id.name());
    values.push_back(id);
    id.setModified(false);
    fields.push_back(type.name());
    values.push_back(type);
    type.setModified(false);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return litesql::Persistent::insert(tables, fieldRecs, valueRecs, sequence__);
}
void Role::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    LITESQL_String newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void Role::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
    updateField(updates, table__, type);
}
void Role::addIDUpdates(Updates& updates) {
}
void Role::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ftypes.push_back(Id);
    ftypes.push_back(Type);
}
void Role::delRecord() {
    deleteFromTable(table__, id);
}
void Role::delRelations() {
    RoleRelation::del(*db, (RoleRelation::Role == id));
}
void Role::update() {
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
void Role::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<Role> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool Role::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<Role> Role::upcast() {
    if (type == Student::type__)
        return std::auto_ptr<Role>(new Student(select<Student>(*db, Id == id).one()));
    if (type == Employee::type__)
        return std::auto_ptr<Role>(new Employee(select<Employee>(*db, Id == id).one()));
    return std::auto_ptr<Role>(new Role(*this));
}
std::auto_ptr<Role> Role::upcastCopy() {
    Role* np = NULL;
    if (type ==  LITESQL_L( "Student" ))
        np = new Student(*db);
    if (type ==  LITESQL_L( "Employee" ))
        np = new Employee(*db);
    np->id = id;
    np->type = type;
    np->inDatabase = inDatabase;
    return std::auto_ptr<Role>(np);
}
LITESQL_oStream & operator<<(LITESQL_oStream& os, Role o) {
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    os << o.id.name() <<  LITESQL_L( " = " ) << o.id << std::endl;
    os << o.type.name() <<  LITESQL_L( " = " ) << o.type << std::endl;
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    return os;
}
const litesql::FieldType Student::Own::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ), LITESQL_L( "Student_" ));
Student::SchoolHandle::SchoolHandle(const Student& owner)
         : litesql::RelationHandle<Student>(owner) {
}
void Student::SchoolHandle::link(const School& o0) {
    SchoolStudentRelation::link(owner->getDatabase(), o0, *owner);
}
void Student::SchoolHandle::unlink(const School& o0) {
    SchoolStudentRelation::unlink(owner->getDatabase(), o0, *owner);
}
void Student::SchoolHandle::del(const litesql::Expr& expr) {
    SchoolStudentRelation::del(owner->getDatabase(), expr && SchoolStudentRelation::Student == owner->id);
}
litesql::DataSource<School> Student::SchoolHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return SchoolStudentRelation::get<School>(owner->getDatabase(), expr, (SchoolStudentRelation::Student == owner->id) && srcExpr);
}
litesql::DataSource<SchoolStudentRelation::Row> Student::SchoolHandle::getRows(const litesql::Expr& expr) {
    return SchoolStudentRelation::getRows(owner->getDatabase(), expr && (SchoolStudentRelation::Student == owner->id));
}
const LITESQL_String Student::type__( LITESQL_L( "Student" ));
const LITESQL_String Student::table__( LITESQL_L( "Student_" ));
Student::Student(const litesql::Database& db)
     : Role(db) {
}
Student::Student(const litesql::Database& db, const litesql::Record& rec)
     : Role(db, rec) {
}
Student::Student(const Student& obj)
     : Role(obj) {
}
const Student& Student::operator=(const Student& obj) {
    Role::operator=(obj);
    return *this;
}
Student::SchoolHandle Student::school() {
    return Student::SchoolHandle(*this);
}
LITESQL_String Student::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back( LITESQL_L( "id_" ));
    values.push_back(id);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return Role::insert(tables, fieldRecs, valueRecs);
}
void Student::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    LITESQL_String newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void Student::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    Role::addUpdates(updates);
}
void Student::addIDUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
}
void Student::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    Role::getFieldTypes(ftypes);
}
void Student::delRecord() {
    deleteFromTable(table__, id);
    Role::delRecord();
}
void Student::delRelations() {
    SchoolStudentRelation::del(*db, (SchoolStudentRelation::Student == id));
}
void Student::update() {
    if (!inDatabase) {
        create();
        return;
    }
    Updates updates;
    addUpdates(updates);
    if (id != oldKey) {
        if (!typeIsCorrect()) 
            upcastCopy()->addIDUpdates(updates);
        else
            addIDUpdates(updates);
    }
    litesql::Persistent::update(updates);
    oldKey = id;
}
void Student::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<Student> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool Student::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<Student> Student::upcast() {
    return std::auto_ptr<Student>(new Student(*this));
}
std::auto_ptr<Student> Student::upcastCopy() {
    Student* np = new Student(*this);
    np->inDatabase = inDatabase;
    return std::auto_ptr<Student>(np);
}
LITESQL_oStream & operator<<(LITESQL_oStream& os, Student o) {
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    os << o.id.name() <<  LITESQL_L( " = " ) << o.id << std::endl;
    os << o.type.name() <<  LITESQL_L( " = " ) << o.type << std::endl;
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    return os;
}
const litesql::FieldType Employee::Own::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ), LITESQL_L( "Employee_" ));
Employee::OfficeHandle::OfficeHandle(const Employee& owner)
         : litesql::RelationHandle<Employee>(owner) {
}
void Employee::OfficeHandle::link(const Office& o0) {
    EmployeeOfficeRelation::link(owner->getDatabase(), *owner, o0);
}
void Employee::OfficeHandle::unlink(const Office& o0) {
    EmployeeOfficeRelation::unlink(owner->getDatabase(), *owner, o0);
}
void Employee::OfficeHandle::del(const litesql::Expr& expr) {
    EmployeeOfficeRelation::del(owner->getDatabase(), expr && EmployeeOfficeRelation::Employee == owner->id);
}
litesql::DataSource<Office> Employee::OfficeHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return EmployeeOfficeRelation::get<Office>(owner->getDatabase(), expr, (EmployeeOfficeRelation::Employee == owner->id) && srcExpr);
}
litesql::DataSource<EmployeeOfficeRelation::Row> Employee::OfficeHandle::getRows(const litesql::Expr& expr) {
    return EmployeeOfficeRelation::getRows(owner->getDatabase(), expr && (EmployeeOfficeRelation::Employee == owner->id));
}
const LITESQL_String Employee::type__( LITESQL_L( "Employee" ));
const LITESQL_String Employee::table__( LITESQL_L( "Employee_" ));
Employee::Employee(const litesql::Database& db)
     : Role(db) {
}
Employee::Employee(const litesql::Database& db, const litesql::Record& rec)
     : Role(db, rec) {
}
Employee::Employee(const Employee& obj)
     : Role(obj) {
}
const Employee& Employee::operator=(const Employee& obj) {
    Role::operator=(obj);
    return *this;
}
Employee::OfficeHandle Employee::office() {
    return Employee::OfficeHandle(*this);
}
LITESQL_String Employee::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back( LITESQL_L( "id_" ));
    values.push_back(id);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return Role::insert(tables, fieldRecs, valueRecs);
}
void Employee::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    LITESQL_String newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void Employee::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    Role::addUpdates(updates);
}
void Employee::addIDUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
}
void Employee::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    Role::getFieldTypes(ftypes);
}
void Employee::delRecord() {
    deleteFromTable(table__, id);
    Role::delRecord();
}
void Employee::delRelations() {
    EmployeeOfficeRelation::del(*db, (EmployeeOfficeRelation::Employee == id));
}
void Employee::update() {
    if (!inDatabase) {
        create();
        return;
    }
    Updates updates;
    addUpdates(updates);
    if (id != oldKey) {
        if (!typeIsCorrect()) 
            upcastCopy()->addIDUpdates(updates);
        else
            addIDUpdates(updates);
    }
    litesql::Persistent::update(updates);
    oldKey = id;
}
void Employee::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<Employee> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool Employee::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<Employee> Employee::upcast() {
    return std::auto_ptr<Employee>(new Employee(*this));
}
std::auto_ptr<Employee> Employee::upcastCopy() {
    Employee* np = new Employee(*this);
    np->inDatabase = inDatabase;
    return std::auto_ptr<Employee>(np);
}
LITESQL_oStream & operator<<(LITESQL_oStream& os, Employee o) {
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    os << o.id.name() <<  LITESQL_L( " = " ) << o.id << std::endl;
    os << o.type.name() <<  LITESQL_L( " = " ) << o.type << std::endl;
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    return os;
}
const litesql::FieldType School::Own::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ), LITESQL_L( "School_" ));
School::StudentsHandle::StudentsHandle(const School& owner)
         : litesql::RelationHandle<School>(owner) {
}
void School::StudentsHandle::link(const Student& o0) {
    SchoolStudentRelation::link(owner->getDatabase(), *owner, o0);
}
void School::StudentsHandle::unlink(const Student& o0) {
    SchoolStudentRelation::unlink(owner->getDatabase(), *owner, o0);
}
void School::StudentsHandle::del(const litesql::Expr& expr) {
    SchoolStudentRelation::del(owner->getDatabase(), expr && SchoolStudentRelation::School == owner->id);
}
litesql::DataSource<Student> School::StudentsHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return SchoolStudentRelation::get<Student>(owner->getDatabase(), expr, (SchoolStudentRelation::School == owner->id) && srcExpr);
}
litesql::DataSource<SchoolStudentRelation::Row> School::StudentsHandle::getRows(const litesql::Expr& expr) {
    return SchoolStudentRelation::getRows(owner->getDatabase(), expr && (SchoolStudentRelation::School == owner->id));
}
const LITESQL_String School::type__( LITESQL_L( "School" ));
const LITESQL_String School::table__( LITESQL_L( "School_" ));
const LITESQL_String School::sequence__( LITESQL_L( "School_seq" ));
const litesql::FieldType School::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType School::Type( LITESQL_L( "type_" ), LITESQL_L( "TEXT" ),table__);
const litesql::FieldType School::Name( LITESQL_L( "name_" ), LITESQL_L( "TEXT" ),table__);
void School::defaults() {
    id = 0;
}
School::School(const litesql::Database& db)
     : litesql::Persistent(db), id(Id), type(Type), name(Name) {
    defaults();
}
School::School(const litesql::Database& db, const litesql::Record& rec)
     : litesql::Persistent(db, rec), id(Id), type(Type), name(Name) {
    defaults();
    size_t size = (rec.size() > 3) ? 3 : rec.size();
    switch(size) {
    case 3: name = convert<const LITESQL_String&, LITESQL_String>(rec[2]);
        name.setModified(false);
    case 2: type = convert<const LITESQL_String&, LITESQL_String>(rec[1]);
        type.setModified(false);
    case 1: id = convert<const LITESQL_String&, int>(rec[0]);
        id.setModified(false);
    }
}
School::School(const School& obj)
     : litesql::Persistent(obj), id(obj.id), type(obj.type), name(obj.name) {
}
const School& School::operator=(const School& obj) {
    if (this != &obj) {
        id = obj.id;
        type = obj.type;
        name = obj.name;
    }
    litesql::Persistent::operator=(obj);
    return *this;
}
School::StudentsHandle School::students() {
    return School::StudentsHandle(*this);
}
LITESQL_String School::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back(id.name());
    values.push_back(id);
    id.setModified(false);
    fields.push_back(type.name());
    values.push_back(type);
    type.setModified(false);
    fields.push_back(name.name());
    values.push_back(name);
    name.setModified(false);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return litesql::Persistent::insert(tables, fieldRecs, valueRecs, sequence__);
}
void School::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    LITESQL_String newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void School::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
    updateField(updates, table__, type);
    updateField(updates, table__, name);
}
void School::addIDUpdates(Updates& updates) {
}
void School::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ftypes.push_back(Id);
    ftypes.push_back(Type);
    ftypes.push_back(Name);
}
void School::delRecord() {
    deleteFromTable(table__, id);
}
void School::delRelations() {
    SchoolStudentRelation::del(*db, (SchoolStudentRelation::School == id));
}
void School::update() {
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
void School::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<School> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool School::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<School> School::upcast() {
    return std::auto_ptr<School>(new School(*this));
}
std::auto_ptr<School> School::upcastCopy() {
    School* np = new School(*this);
    np->id = id;
    np->type = type;
    np->name = name;
    np->inDatabase = inDatabase;
    return std::auto_ptr<School>(np);
}
LITESQL_oStream & operator<<(LITESQL_oStream& os, School o) {
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    os << o.id.name() <<  LITESQL_L( " = " ) << o.id << std::endl;
    os << o.type.name() <<  LITESQL_L( " = " ) << o.type << std::endl;
    os << o.name.name() <<  LITESQL_L( " = " ) << o.name << std::endl;
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    return os;
}
const litesql::FieldType Office::Own::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ), LITESQL_L( "Office_" ));
Office::EmployeesHandle::EmployeesHandle(const Office& owner)
         : litesql::RelationHandle<Office>(owner) {
}
void Office::EmployeesHandle::link(const Employee& o0) {
    EmployeeOfficeRelation::link(owner->getDatabase(), o0, *owner);
}
void Office::EmployeesHandle::unlink(const Employee& o0) {
    EmployeeOfficeRelation::unlink(owner->getDatabase(), o0, *owner);
}
void Office::EmployeesHandle::del(const litesql::Expr& expr) {
    EmployeeOfficeRelation::del(owner->getDatabase(), expr && EmployeeOfficeRelation::Office == owner->id);
}
litesql::DataSource<Employee> Office::EmployeesHandle::get(const litesql::Expr& expr, const litesql::Expr& srcExpr) {
    return EmployeeOfficeRelation::get<Employee>(owner->getDatabase(), expr, (EmployeeOfficeRelation::Office == owner->id) && srcExpr);
}
litesql::DataSource<EmployeeOfficeRelation::Row> Office::EmployeesHandle::getRows(const litesql::Expr& expr) {
    return EmployeeOfficeRelation::getRows(owner->getDatabase(), expr && (EmployeeOfficeRelation::Office == owner->id));
}
const LITESQL_String Office::type__( LITESQL_L( "Office" ));
const LITESQL_String Office::table__( LITESQL_L( "Office_" ));
const LITESQL_String Office::sequence__( LITESQL_L( "Office_seq" ));
const litesql::FieldType Office::Id( LITESQL_L( "id_" ), LITESQL_L( "INTEGER" ),table__);
const litesql::FieldType Office::Type( LITESQL_L( "type_" ), LITESQL_L( "TEXT" ),table__);
void Office::defaults() {
    id = 0;
}
Office::Office(const litesql::Database& db)
     : litesql::Persistent(db), id(Id), type(Type) {
    defaults();
}
Office::Office(const litesql::Database& db, const litesql::Record& rec)
     : litesql::Persistent(db, rec), id(Id), type(Type) {
    defaults();
    size_t size = (rec.size() > 2) ? 2 : rec.size();
    switch(size) {
    case 2: type = convert<const LITESQL_String&, LITESQL_String>(rec[1]);
        type.setModified(false);
    case 1: id = convert<const LITESQL_String&, int>(rec[0]);
        id.setModified(false);
    }
}
Office::Office(const Office& obj)
     : litesql::Persistent(obj), id(obj.id), type(obj.type) {
}
const Office& Office::operator=(const Office& obj) {
    if (this != &obj) {
        id = obj.id;
        type = obj.type;
    }
    litesql::Persistent::operator=(obj);
    return *this;
}
Office::EmployeesHandle Office::employees() {
    return Office::EmployeesHandle(*this);
}
LITESQL_String Office::insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs) {
    tables.push_back(table__);
    litesql::Record fields;
    litesql::Record values;
    fields.push_back(id.name());
    values.push_back(id);
    id.setModified(false);
    fields.push_back(type.name());
    values.push_back(type);
    type.setModified(false);
    fieldRecs.push_back(fields);
    valueRecs.push_back(values);
    return litesql::Persistent::insert(tables, fieldRecs, valueRecs, sequence__);
}
void Office::create() {
    litesql::Record tables;
    litesql::Records fieldRecs;
    litesql::Records valueRecs;
    type = type__;
    LITESQL_String newID = insert(tables, fieldRecs, valueRecs);
    if (id == 0)
        id = newID;
}
void Office::addUpdates(Updates& updates) {
    prepareUpdate(updates, table__);
    updateField(updates, table__, id);
    updateField(updates, table__, type);
}
void Office::addIDUpdates(Updates& updates) {
}
void Office::getFieldTypes(std::vector<litesql::FieldType>& ftypes) {
    ftypes.push_back(Id);
    ftypes.push_back(Type);
}
void Office::delRecord() {
    deleteFromTable(table__, id);
}
void Office::delRelations() {
    EmployeeOfficeRelation::del(*db, (EmployeeOfficeRelation::Office == id));
}
void Office::update() {
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
void Office::del() {
    if (typeIsCorrect() == false) {
        std::auto_ptr<Office> p(upcastCopy());
        p->delRelations();
        p->onDelete();
        p->delRecord();
    } else {
        onDelete();
        delRecord();
    }
    inDatabase = false;
}
bool Office::typeIsCorrect() {
    return type == type__;
}
std::auto_ptr<Office> Office::upcast() {
    return std::auto_ptr<Office>(new Office(*this));
}
std::auto_ptr<Office> Office::upcastCopy() {
    Office* np = new Office(*this);
    np->id = id;
    np->type = type;
    np->inDatabase = inDatabase;
    return std::auto_ptr<Office>(np);
}
LITESQL_oStream & operator<<(LITESQL_oStream& os, Office o) {
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    os << o.id.name() <<  LITESQL_L( " = " ) << o.id << std::endl;
    os << o.type.name() <<  LITESQL_L( " = " ) << o.type << std::endl;
    os <<  LITESQL_L( "-------------------------------------" ) << std::endl;
    return os;
}
ExampleDatabase::ExampleDatabase(LITESQL_String backendType, LITESQL_String connInfo)
     : litesql::Database(backendType, connInfo) {
    initialize();
}
std::vector<litesql::Database::SchemaItem> ExampleDatabase::getSchema() const {
    std::vector<Database::SchemaItem> res;
    res.push_back(Database::SchemaItem( LITESQL_L( "schema_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE schema_ (name_ TEXT, type_ TEXT, sql_ TEXT);" )));
    if (backend->supportsSequences()) {
        res.push_back(Database::SchemaItem( LITESQL_L( "user_seq" ), LITESQL_L( "sequence" ), LITESQL_L( "CREATE SEQUENCE user_seq START 1 INCREMENT 1" )));
        res.push_back(Database::SchemaItem( LITESQL_L( "Person_seq" ), LITESQL_L( "sequence" ), LITESQL_L( "CREATE SEQUENCE Person_seq START 1 INCREMENT 1" )));
        res.push_back(Database::SchemaItem( LITESQL_L( "Role_seq" ), LITESQL_L( "sequence" ), LITESQL_L( "CREATE SEQUENCE Role_seq START 1 INCREMENT 1" )));
        res.push_back(Database::SchemaItem( LITESQL_L( "School_seq" ), LITESQL_L( "sequence" ), LITESQL_L( "CREATE SEQUENCE School_seq START 1 INCREMENT 1" )));
        res.push_back(Database::SchemaItem( LITESQL_L( "Office_seq" ), LITESQL_L( "sequence" ), LITESQL_L( "CREATE SEQUENCE Office_seq START 1 INCREMENT 1" )));
    }
    res.push_back(Database::SchemaItem( LITESQL_L( "user_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE user_ (id_ " ) + backend->getRowIDType() +  LITESQL_L( ",type_ TEXT,name_ TEXT,passwd_ TEXT)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Person_ (id_ " ) + backend->getRowIDType() +  LITESQL_L( ",type_ TEXT,name_ TEXT,age_ INTEGER,image_ BLOB,aDoubleValue_ DOUBLE,sex_ INTEGER)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Role_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Role_ (id_ " ) + backend->getRowIDType() +  LITESQL_L( ",type_ TEXT)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Student_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Student_ (id_ " ) + backend->getRowIDType() +  LITESQL_L( ")" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Employee_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Employee_ (id_ " ) + backend->getRowIDType() +  LITESQL_L( ")" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "School_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE School_ (id_ " ) + backend->getRowIDType() +  LITESQL_L( ",type_ TEXT,name_ TEXT)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Office_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Office_ (id_ " ) + backend->getRowIDType() +  LITESQL_L( ",type_ TEXT)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_Mother" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Person_Person_Mother (Person1 INTEGER UNIQUE,Person2 INTEGER)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_Father" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Person_Person_Father (Person1 INTEGER UNIQUE,Person2 INTEGER)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_Siblings" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Person_Person_Siblings (Person1 INTEGER,Person2 INTEGER)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_Children" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Person_Person_Children (Person1 INTEGER,Person2 INTEGER)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Role_Roles" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Person_Role_Roles (Person1 INTEGER,Role2 INTEGER UNIQUE)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "School_Student_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE School_Student_ (School1 INTEGER,Student2 INTEGER UNIQUE)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Employee_Office_" ), LITESQL_L( "table" ), LITESQL_L( "CREATE TABLE Employee_Office_ (Employee1 INTEGER,Office2 INTEGER)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person__name_age_idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE UNIQUE INDEX Person__name_age_idx ON Person_ (name_,age_)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_MotherPerson1idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Person_MotherPerson1idx ON Person_Person_Mother (Person1)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_MotherPerson2idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Person_MotherPerson2idx ON Person_Person_Mother (Person2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_Mother_all_idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Person_Mother_all_idx ON Person_Person_Mother (Person1,Person2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_FatherPerson1idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Person_FatherPerson1idx ON Person_Person_Father (Person1)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_FatherPerson2idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Person_FatherPerson2idx ON Person_Person_Father (Person2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_Father_all_idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Person_Father_all_idx ON Person_Person_Father (Person1,Person2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "_fc4501d1c1e9cc173fbe356a08a9d12f" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX _fc4501d1c1e9cc173fbe356a08a9d12f ON Person_Person_Siblings (Person1)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "_29908e51ecc673e39c38238d4abe5b3b" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX _29908e51ecc673e39c38238d4abe5b3b ON Person_Person_Siblings (Person2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_Siblings_all_idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Person_Siblings_all_idx ON Person_Person_Siblings (Person1,Person2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "_c77a0c252bbee950ec06bda52dd09648" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX _c77a0c252bbee950ec06bda52dd09648 ON Person_Person_Children (Person1)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "_64f9014350ce47b5d0f7606b127df7c3" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX _64f9014350ce47b5d0f7606b127df7c3 ON Person_Person_Children (Person2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Person_Children_all_idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Person_Children_all_idx ON Person_Person_Children (Person1,Person2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Role_RolesPerson1idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Role_RolesPerson1idx ON Person_Role_Roles (Person1)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Role_RolesRole2idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Role_RolesRole2idx ON Person_Role_Roles (Role2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Person_Role_Roles_all_idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Person_Role_Roles_all_idx ON Person_Role_Roles (Person1,Role2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "School_Student_School1idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX School_Student_School1idx ON School_Student_ (School1)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "School_Student_Student2idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX School_Student_Student2idx ON School_Student_ (Student2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "School_Student__all_idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX School_Student__all_idx ON School_Student_ (School1,Student2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Employee_Office_Employee1idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Employee_Office_Employee1idx ON Employee_Office_ (Employee1)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Employee_Office_Office2idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Employee_Office_Office2idx ON Employee_Office_ (Office2)" )));
    res.push_back(Database::SchemaItem( LITESQL_L( "Employee_Office__all_idx" ), LITESQL_L( "index" ), LITESQL_L( "CREATE INDEX Employee_Office__all_idx ON Employee_Office_ (Employee1,Office2)" )));
    return res;
}
void ExampleDatabase::initialize() {
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;
    Person::initValues();
}
}