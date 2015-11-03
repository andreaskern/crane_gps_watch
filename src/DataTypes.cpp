// Copyright (C) 2014 - 2015 mru@sisyphus.teil.cc
//
// Data Types
//


#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>

#include <ctime>
#include <cstring>

#include "DataTypes.hpp"


put_time::put_time(const tm* time, const std::string& format) : str() {
    const size_t max_size = 800;
    char buf[max_size];
    size_t n = std::strftime(buf, max_size, format.c_str(), time);
    if (n == 0) {
        throw std::runtime_error (fmt() << "failed to format time string: " << strerror(errno) << " with format " << format);
    }
    str = buf;
}

std::ostream& operator << (std::ostream& o, const put_time& time) {
    return o << time.str;
}

GpsLocation& GpsLocation::operator += (const GpsLocation& other) {
    loc += other.loc;
    return *this;
}

std::string GpsLocation::format() const {
    return fmt() << std::setprecision(15) << (double)(*this);
}

GpsLocation::operator double() const {
    return (loc / 10000000.0);
}

GpsEle& GpsEle::operator += (const GpsEle& other) {
    ele += other.ele;
    return *this;
}
std::string GpsEle::format() const {
    return fmt() << ele;
}

GpsTime::GpsTime() : time () {
    // explicit for documentation purposes:
    // see 'man mktime' for details.
    /*
       tm_isdst  A  flag  that  indicates  whether  daylight saving time is in$
                 effect at the time described.  The value is positive if  day‐$
                 light  saving time is in effect, zero if it is not, and nega‐$
                 tive if the information is not available.$
    */
    /* 
     * Initialize tm_isdst to -1 glib's mktime() will set it to the systems dst.
     * If tm_isdst is not equal to the systems dst, time will be off by +/- 1 hour .
     */
    time.tm_isdst = -1;
}

GpsTime& GpsTime::operator=(const GpsTimeUpd& other) {
    time.tm_min = other.mm;
    time.tm_sec = other.ss;
    mktime();
    return *this;
}

time_t GpsTime::mktime() {

    //backup current time structure to preserve the watch's time-zone
	tm timesave = time; 

    time_t t = ::mktime(&time);

#ifndef  __MINGW32__ 
    // this does not work in cross-compile for windows
    time.tm_gmtoff = timesave.tm_gmtoff; //restore the watch's time-zone
#endif

    if (t == (time_t) -1) {
        throw std::runtime_error(fmt() << "failed to mktime: " << strerror(errno));
    }
    return t;
}

std::string GpsTime::format() const {

#ifdef  __MINGW32__ 
    // windows implements a different %z - only the name of the timezone can be shown
    std::string fmt_time = fmt() << put_time(&time, "%Y-%m-%dT%H:%M:%S+0000");
#else
    // linux uses %z = "+hhmm" format
    std::string fmt_time = fmt() << put_time(&time, "%Y-%m-%dT%H:%M:%S%z");
#endif
    if(fmt_time.length() >= 24){
	    //insert missing ':' in the time zone. put_time timezone:"+0100"
        //                                     XML      timezone:"+01:00"
        fmt_time.insert((fmt_time.length() -2) ,":");
    } 
    return fmt_time;
    // does not work with windows:
    //return fmt() << put_time(&time, "%FT%TZ");
}

std::string GpsTime::format_no_tz() const {
    std::string fmt_time = fmt() << put_time(&time, "%Y-%m-%dT%H:%M:%S");
    return fmt_time;
    // does not work with windows:
    //return fmt() << put_time(&time, "%FT%TZ");
}

Language& Language::operator= (unsigned char v) {
    language = (language_t) v;
    return *this;
}

std::string Language::format() const {
    switch (language) {
      case Language::English:
        return "English";
      case Language::French:
        return "French";
      case Language::Spanish:
        return "Spanish";
      case Language::German:
        return "German";
      case Language::Italiano:
        return "Italian";
    }
    return "";
}
