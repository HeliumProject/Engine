#ifndef exampledatabase_hpp
#define exampledatabase_hpp
#include "litesql.hpp"
namespace example {
class user;
class Person;
class Role;
class Student;
class Employee;
class School;
class Office;
class ThingWithMethods;
class PersonPersonRelationMother {
public:
    class Row {
    public:
        litesql::Field<int> person2;
        litesql::Field<int> person1;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const LITESQL_String table__;
    static const litesql::FieldType Person1;
    static const litesql::FieldType Person2;
    static void link(const litesql::Database& db, const example::Person& o0, const example::Person& o1);
    static void unlink(const litesql::Database& db, const example::Person& o0, const example::Person& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<PersonPersonRelationMother::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<example::Person> getPerson1(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
    static litesql::DataSource<example::Person> getPerson2(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
};
class PersonPersonRelationFather {
public:
    class Row {
    public:
        litesql::Field<int> person2;
        litesql::Field<int> person1;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const LITESQL_String table__;
    static const litesql::FieldType Person1;
    static const litesql::FieldType Person2;
    static void link(const litesql::Database& db, const example::Person& o0, const example::Person& o1);
    static void unlink(const litesql::Database& db, const example::Person& o0, const example::Person& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<PersonPersonRelationFather::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<example::Person> getPerson1(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
    static litesql::DataSource<example::Person> getPerson2(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
};
class PersonPersonRelationSiblings {
public:
    class Row {
    public:
        litesql::Field<int> person2;
        litesql::Field<int> person1;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const LITESQL_String table__;
    static const litesql::FieldType Person1;
    static const litesql::FieldType Person2;
    static void link(const litesql::Database& db, const example::Person& o0, const example::Person& o1);
    static void unlink(const litesql::Database& db, const example::Person& o0, const example::Person& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<PersonPersonRelationSiblings::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<example::Person> getPerson1(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
    static litesql::DataSource<example::Person> getPerson2(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
};
class PersonPersonRelationChildren {
public:
    class Row {
    public:
        litesql::Field<int> person2;
        litesql::Field<int> person1;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const LITESQL_String table__;
    static const litesql::FieldType Person1;
    static const litesql::FieldType Person2;
    static void link(const litesql::Database& db, const example::Person& o0, const example::Person& o1);
    static void unlink(const litesql::Database& db, const example::Person& o0, const example::Person& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<PersonPersonRelationChildren::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<example::Person> getPerson1(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
    static litesql::DataSource<example::Person> getPerson2(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
};
class RoleRelation {
public:
    class Row {
    public:
        litesql::Field<int> role;
        litesql::Field<int> person;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const LITESQL_String table__;
    static const litesql::FieldType Person;
    static const litesql::FieldType Role;
    static void link(const litesql::Database& db, const example::Person& o0, const example::Role& o1);
    static void unlink(const litesql::Database& db, const example::Person& o0, const example::Role& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<RoleRelation::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    template <class T> static litesql::DataSource<T> get(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
;
;
};
class SchoolStudentRelation {
public:
    class Row {
    public:
        litesql::Field<int> student;
        litesql::Field<int> school;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const LITESQL_String table__;
    static const litesql::FieldType School;
    static const litesql::FieldType Student;
    static void link(const litesql::Database& db, const example::School& o0, const example::Student& o1);
    static void unlink(const litesql::Database& db, const example::School& o0, const example::Student& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<SchoolStudentRelation::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    template <class T> static litesql::DataSource<T> get(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
;
;
};
class EmployeeOfficeRelation {
public:
    class Row {
    public:
        litesql::Field<int> office;
        litesql::Field<int> employee;
        Row(const litesql::Database& db, const litesql::Record& rec=litesql::Record());
    };
    static const LITESQL_String table__;
    static const litesql::FieldType Employee;
    static const litesql::FieldType Office;
    static void link(const litesql::Database& db, const example::Employee& o0, const example::Office& o1);
    static void unlink(const litesql::Database& db, const example::Employee& o0, const example::Office& o1);
    static void del(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    static litesql::DataSource<EmployeeOfficeRelation::Row> getRows(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr());
    template <class T> static litesql::DataSource<T> get(const litesql::Database& db, const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
;
;
};
class user : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    static const LITESQL_String type__;
    static const LITESQL_String table__;
    static const LITESQL_String sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<LITESQL_String> type;
    static const litesql::FieldType Name;
    litesql::Field<LITESQL_String> name;
    static const litesql::FieldType Passwd;
    litesql::Field<LITESQL_String> passwd;
protected:
    void defaults();
public:
    user(const litesql::Database& db);
    user(const litesql::Database& db, const litesql::Record& rec);
    user(const user& obj);
    const user& operator=(const user& obj);
    virtual void sayHello();
protected:
    LITESQL_String insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
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
    std::auto_ptr<user> upcast();
    std::auto_ptr<user> upcastCopy();
};
LITESQL_oStream & operator<<(LITESQL_oStream& os, user o);
class Person : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class SexType : public litesql::FieldType {
    public:
        static const int Male;
        static const int Female;
        SexType(const LITESQL_String& n, const LITESQL_String& t, const LITESQL_String& tbl, const litesql::FieldType::Values& vals=Values());
    };
    class Sex {
    public:
        static const int Male;
        static const int Female;
    };
    class MotherHandle : public litesql::RelationHandle<Person> {
    public:
        MotherHandle(const Person& owner);
        void link(const Person& o0);
        void unlink(const Person& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Person> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<PersonPersonRelationMother::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    class FatherHandle : public litesql::RelationHandle<Person> {
    public:
        FatherHandle(const Person& owner);
        void link(const Person& o0);
        void unlink(const Person& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Person> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<PersonPersonRelationFather::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    class SiblingsHandle : public litesql::RelationHandle<Person> {
    public:
        SiblingsHandle(const Person& owner);
        void link(const Person& o0);
        void unlink(const Person& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Person> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<PersonPersonRelationSiblings::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    class ChildrenHandle : public litesql::RelationHandle<Person> {
    public:
        ChildrenHandle(const Person& owner);
        void link(const Person& o0);
        void unlink(const Person& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Person> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<PersonPersonRelationChildren::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    class RolesHandle : public litesql::RelationHandle<Person> {
    public:
        RolesHandle(const Person& owner);
        void link(const Role& o0);
        void unlink(const Role& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Role> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<RoleRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const LITESQL_String type__;
    static const LITESQL_String table__;
    static const LITESQL_String sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<LITESQL_String> type;
    static const litesql::FieldType Name;
    litesql::Field<LITESQL_String> name;
    static const litesql::FieldType Age;
    litesql::Field<int> age;
    static const litesql::FieldType Image;
    litesql::Field<litesql::Blob> image;
    static const litesql::FieldType ADoubleValue;
    litesql::Field<double> aDoubleValue;
protected:
    static std::vector < std::pair< LITESQL_String, LITESQL_String > > sex_values;
public:
    static const Person::SexType Sex;
    litesql::Field<int> sex;
    static void initValues();
protected:
    void defaults();
public:
    Person(const litesql::Database& db);
    Person(const litesql::Database& db, const litesql::Record& rec);
    Person(const Person& obj);
    const Person& operator=(const Person& obj);
    Person::MotherHandle mother();
    Person::FatherHandle father();
    Person::SiblingsHandle siblings();
    Person::ChildrenHandle children();
    Person::RolesHandle roles();
    virtual void sayHello();
protected:
    LITESQL_String insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
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
    std::auto_ptr<Person> upcast();
    std::auto_ptr<Person> upcastCopy();
};
LITESQL_oStream & operator<<(LITESQL_oStream& os, Person o);
class Role : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class PersonHandle : public litesql::RelationHandle<Role> {
    public:
        PersonHandle(const Role& owner);
        void link(const Person& o0);
        void unlink(const Person& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Person> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<RoleRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const LITESQL_String type__;
    static const LITESQL_String table__;
    static const LITESQL_String sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<LITESQL_String> type;
protected:
    void defaults();
public:
    Role(const litesql::Database& db);
    Role(const litesql::Database& db, const litesql::Record& rec);
    Role(const Role& obj);
    const Role& operator=(const Role& obj);
    Role::PersonHandle person();
protected:
    LITESQL_String insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
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
    std::auto_ptr<Role> upcast();
    std::auto_ptr<Role> upcastCopy();
};
LITESQL_oStream & operator<<(LITESQL_oStream& os, Role o);
class Student : public Role {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class SchoolHandle : public litesql::RelationHandle<Student> {
    public:
        SchoolHandle(const Student& owner);
        void link(const School& o0);
        void unlink(const School& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<School> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<SchoolStudentRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const LITESQL_String type__;
    static const LITESQL_String table__;
    Student(const litesql::Database& db);
    Student(const litesql::Database& db, const litesql::Record& rec);
    Student(const Student& obj);
    const Student& operator=(const Student& obj);
    Student::SchoolHandle school();
protected:
    LITESQL_String insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
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
    std::auto_ptr<Student> upcast();
    std::auto_ptr<Student> upcastCopy();
};
LITESQL_oStream & operator<<(LITESQL_oStream& os, Student o);
class Employee : public Role {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class OfficeHandle : public litesql::RelationHandle<Employee> {
    public:
        OfficeHandle(const Employee& owner);
        void link(const Office& o0);
        void unlink(const Office& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Office> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<EmployeeOfficeRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const LITESQL_String type__;
    static const LITESQL_String table__;
    Employee(const litesql::Database& db);
    Employee(const litesql::Database& db, const litesql::Record& rec);
    Employee(const Employee& obj);
    const Employee& operator=(const Employee& obj);
    Employee::OfficeHandle office();
protected:
    LITESQL_String insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
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
    std::auto_ptr<Employee> upcast();
    std::auto_ptr<Employee> upcastCopy();
};
LITESQL_oStream & operator<<(LITESQL_oStream& os, Employee o);
class School : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class StudentsHandle : public litesql::RelationHandle<School> {
    public:
        StudentsHandle(const School& owner);
        void link(const Student& o0);
        void unlink(const Student& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Student> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<SchoolStudentRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const LITESQL_String type__;
    static const LITESQL_String table__;
    static const LITESQL_String sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<LITESQL_String> type;
    static const litesql::FieldType Name;
    litesql::Field<LITESQL_String> name;
protected:
    void defaults();
public:
    School(const litesql::Database& db);
    School(const litesql::Database& db, const litesql::Record& rec);
    School(const School& obj);
    const School& operator=(const School& obj);
    School::StudentsHandle students();
protected:
    LITESQL_String insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
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
    std::auto_ptr<School> upcast();
    std::auto_ptr<School> upcastCopy();
};
LITESQL_oStream & operator<<(LITESQL_oStream& os, School o);
class Office : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    class EmployeesHandle : public litesql::RelationHandle<Office> {
    public:
        EmployeesHandle(const Office& owner);
        void link(const Employee& o0);
        void unlink(const Employee& o0);
        void del(const litesql::Expr& expr=litesql::Expr());
        litesql::DataSource<Employee> get(const litesql::Expr& expr=litesql::Expr(), const litesql::Expr& srcExpr=litesql::Expr());
        litesql::DataSource<EmployeeOfficeRelation::Row> getRows(const litesql::Expr& expr=litesql::Expr());
    };
    static const LITESQL_String type__;
    static const LITESQL_String table__;
    static const LITESQL_String sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<LITESQL_String> type;
protected:
    void defaults();
public:
    Office(const litesql::Database& db);
    Office(const litesql::Database& db, const litesql::Record& rec);
    Office(const Office& obj);
    const Office& operator=(const Office& obj);
    Office::EmployeesHandle employees();
protected:
    LITESQL_String insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
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
    std::auto_ptr<Office> upcast();
    std::auto_ptr<Office> upcastCopy();
};
LITESQL_oStream & operator<<(LITESQL_oStream& os, Office o);
class ThingWithMethods : public litesql::Persistent {
public:
    class Own {
    public:
        static const litesql::FieldType Id;
    };
    static const LITESQL_String type__;
    static const LITESQL_String table__;
    static const LITESQL_String sequence__;
    static const litesql::FieldType Id;
    litesql::Field<int> id;
    static const litesql::FieldType Type;
    litesql::Field<LITESQL_String> type;
protected:
    void defaults();
public:
    ThingWithMethods(const litesql::Database& db);
    ThingWithMethods(const litesql::Database& db, const litesql::Record& rec);
    ThingWithMethods(const ThingWithMethods& obj);
    const ThingWithMethods& operator=(const ThingWithMethods& obj);
    virtual void sayHello(LITESQL_String text, int repeat);
protected:
    LITESQL_String insert(litesql::Record& tables, litesql::Records& fieldRecs, litesql::Records& valueRecs);
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
    std::auto_ptr<ThingWithMethods> upcast();
    std::auto_ptr<ThingWithMethods> upcastCopy();
};
LITESQL_oStream & operator<<(LITESQL_oStream& os, ThingWithMethods o);
class ExampleDatabase : public litesql::Database {
public:
    ExampleDatabase(LITESQL_String backendType, LITESQL_String connInfo);
protected:
    virtual std::vector<litesql::Database::SchemaItem> getSchema() const;
    static void initialize();
};
}
#endif