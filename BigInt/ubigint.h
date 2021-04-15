//Alex Soliz ID: amsoliz
//Alexander Yegian ID: ayegian
// $Id: ubigint.h,v 1.5 2020-10-11 12:25:22-07 - - $

#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>
using namespace std;
//comment

#include "debug.h"

class ubigint {
    friend ostream& operator<< (ostream&, const ubigint&);
private:
    vector<uint8_t> uvalue;
public:
    void multiply_by_2();
    void divide_by_2();
    string makeString() const;
    ubigint();
    ubigint(unsigned long);
    ubigint(const string&);

    ubigint operator+ (const ubigint&) const;
    ubigint operator- (const ubigint&) const;
    ubigint operator* (const ubigint&) const;
    ubigint operator/ (const ubigint&) const;
    ubigint operator% (const ubigint&) const;

    bool operator== (const ubigint&) const;
    bool operator<  (const ubigint&) const;
};

#endif
#pragma once
