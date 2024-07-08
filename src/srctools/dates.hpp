#ifndef __SRCTOOLS__DATES__
#define __SRCTOOLS__DATES__

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#include <boost/date_time/gregorian/gregorian.hpp>

namespace srctools
{

    inline int days_diff(const std::string &first, const std::string &second)
    {

        boost::gregorian::date date1(boost::gregorian::from_string(first));
        boost::gregorian::date date2(boost::gregorian::from_string(second));

        boost::gregorian::date_duration diff = date1 - date2;
        int diff_int = diff.days();
        return diff_int;
    }

}

#endif
