/* LiteSQL
 *
 * The list of contributors at http://litesql.sf.net/
 *
 * See LICENSE for copyright information. */

#ifndef litesql_datetime_hpp
#define litesql_datetime_hpp
/** \file datetime.hpp
    constains Date, Time and DateTime - classes */
#include "field.hpp"
#include <time.h>
namespace litesql {
/* holds struct tm */
class TimeStruct {
    struct tm mytm;
public:
    TimeStruct(time_t t=0);
    int day() const;
    int dayOfWeek() const;
    int dayOfYear() const;
    int month() const;
    int year() const;
    int hour() const;
    int min() const;
    int sec() const;
    time_t timeStamp() const;
    TimeStruct& setDay(int day);
    TimeStruct& setMonth(int month);
    TimeStruct& setYear(int year);
    TimeStruct& setHour(int hour);
    TimeStruct& setMin(int min);
    TimeStruct& setSec(int sec);
    TimeStruct& setTimeStamp(time_t t);
};
        
/** holds date */
class Date {
    time_t value;
public:
    /** crops time of day to 00:00:00 */
    Date(time_t t=0);
    Date(int day, int month, int year);
    int day() const;
    int dayOfWeek() const;
    int month() const;
    int year() const;
    time_t timeStamp() const;
    TimeStruct timeStruct() const;
    
    Date& setDay(int d);
    Date& setMonth(int m);
    Date& setYear(int y);
    Date& setTimeStamp(time_t t);
    LITESQL_String asString(LITESQL_String format=LITESQL_L("%u")) const;
};
/** holds time of day */
class Time {
    /** secs after midnight */
    int value;
public:
    Time(int secs=0);
    Time(int hour, int min, int sec);
    int hour() const;
    int min() const;
    int sec() const;
    int secs() const;
    LITESQL_String asString(LITESQL_String format=LITESQL_L("%u")) const;

    Time& setHour(int d);
    Time& setMin(int m);
    Time& setSec(int y);
    Time& setSecs(int secs);
};
/** holds date and time of day */
class DateTime {
    time_t value;
public:
    DateTime(time_t t=0);
    int day() const;
    int month() const;
    int year() const;
    int hour() const;
    int min() const;
    int sec() const;
    time_t timeStamp() const;
    TimeStruct timeStruct() const;
    LITESQL_String asString(LITESQL_String format=LITESQL_L("%u")) const;

    DateTime& setDay(int d);
    DateTime& setMonth(int m);
    DateTime& setYear(int y);
    DateTime& setHour(int d);
    DateTime& setMin(int m);
    DateTime& setSec(int y);
    Date& setTimeStamp(time_t t);
};
template <>
Date convert<const LITESQL_String&, Date>(const LITESQL_String& value);
template <>
Time convert<const LITESQL_String&, Time>(const LITESQL_String& value);
template <>
DateTime convert<const LITESQL_String&, DateTime>(const LITESQL_String& value);

template <>
LITESQL_String convert<const Date&, LITESQL_String>(const Date& value);
template <>
LITESQL_String convert<const Time&, LITESQL_String>(const Time& value);
template <>
LITESQL_String convert<const DateTime&, LITESQL_String>(const DateTime& value);

LITESQL_oStream& operator << (LITESQL_oStream& os, const Date& d);
LITESQL_oStream& operator << (LITESQL_oStream& os, const Time& d);
LITESQL_oStream& operator << (LITESQL_oStream& os, const DateTime& d);
}

#endif