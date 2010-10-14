// include LiteSQL's header file and generated header file
#include <iostream>
#include "litesql.hpp"
#include "exampledatabase.hpp"
// provide implementation for Person::sayHello
void example::Person::sayHello() {
    std::LITESQL_cout <<  LITESQL_L( "Hi! My name is " ) << name 
        <<  LITESQL_L( " and I am " ) << age <<  LITESQL_L( " years old." ) << std::endl;
}

void example::user::sayHello() {
    std::LITESQL_cout <<  LITESQL_L( "Hi! My name is " ) << name << std::endl;
}

// no name collisions expected
using namespace litesql;
using namespace example;

int main(int argc, LITESQL_Char **argv) {

    try {
        // using SQLite3 as backend
        ExampleDatabase db( LITESQL_L( "sqlite3" ),  LITESQL_L( "database=example.db" ));
        // create tables, sequences and indexes
        db.verbose = true;
        
        if (db.needsUpgrade())
        {
          db.upgrade();
        }
        // start transaction
        db.begin();

        // create couple of Person-objects
        Person jeff(db);
        jeff.name =  LITESQL_L( "Jeff" );
        jeff.sex = Person::Sex::Male;
        jeff.age = 32;
        jeff.aDoubleValue = 0.32;
        Blob image_jeff( LITESQL_L( "abc" ),4);
        jeff.image = image_jeff;
        // store Jeff to database
        jeff.update();
        Person jill(db);
        jill.name =  LITESQL_L( "Jill" );
        jill.sex = Person::Sex::Female;
        jill.image = Blob::nil;
        jill.age = 33;
        jill.update();
        Person jack(db);
        jack.name =  LITESQL_L( "Jack" );
        jack.sex = Person::Sex::Male;
        jack.update();
        Person jess(db);
        jess.name =  LITESQL_L( "Jess" );
        jess.sex = Person::Sex::Female;
        jess.update();
        // build up relationships between Persons 
        jeff.children().link(jack);
        jill.children().link(jack);
        jill.children().link(jess);
        jack.father().link(jeff);
        jack.mother().link(jill);
        jess.mother().link(jill);
        jack.siblings().link(jill);
        // roles (linking examples)
        Office office(db);
        office.update();
        School school(db);
        school.update();

        Employee jeffRole(db);
        jeffRole.update();
        jeff.roles().link(jeffRole);
        jeffRole.office().link(office);

        Student jackRole(db), jessRole(db);
        jackRole.update();
        jessRole.update();
        jack.roles().link(jackRole);
        jess.roles().link(jessRole);
        
        jackRole.school().link(school);
        jessRole.school().link(school);
        
        // count Persons
        LITESQL_cout <<  LITESQL_L( "There are " ) << select<Person>(db).count() 
             <<  LITESQL_L( " persons." ) << std::endl;
	
        // select all Persons and order them by age
        std::vector<Person> family = select<Person>(db).orderBy(Person::Age).all();
        // show results
        for (std::vector<Person>::iterator i = family.begin(); i != family.end(); i++)
            LITESQL_cout << toString(*i) << std::endl;
             
        // select intersection of Jeff's and Jill's children and
        // iterate results with cursor
        Cursor<Person> cursor = intersect(jeff.children().get(), 
                                          jill.children().get()).cursor();
        // Jack should say hello
        for (;cursor.rowsLeft();cursor++) 
            (*cursor).sayHello();
        
        // select a non-existing Person
        try {
            select<Person>(db, Person::Id == 100).one();
        } catch (NotFound e) {
            LITESQL_cout <<  LITESQL_L( "No Person with id 100" ) << std::endl;
        }
        // commit transaction
        db.commit();
        // clean up 
//        db.drop();
    } catch (Except e) {
        LITESQL_cerr << e << std::endl;
        return -1;
    }
    return 0;
}