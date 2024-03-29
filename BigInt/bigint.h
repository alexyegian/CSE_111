//Alex Soliz ID: amsoliz
//Alexander Yegian ID: ayegian
// $Id: bigint.h,v 1.2 2020-01-06 13:39:55-08 - - $

#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"
//comment

class bigint {
    friend ostream& operator<< (ostream&, const bigint&);
private:
    ubigint uvalue;
    bool is_negative{ false };
public:

    bigint() = default; // Needed or will be suppressed.
    bigint(long);
    bigint(const ubigint&, bool is_negative = false);
    explicit bigint(const string&);
    string makeString() const;
    bool returnIs_negative() const;

    bigint operator+() const;
    bigint operator-() const;

    bigint operator+ (const bigint&) const;
    bigint operator- (const bigint&) const;
    bigint operator* (const bigint&) const;
    bigint operator/ (const bigint&) const;
    bigint operator% (const bigint&) const;

    bool operator== (const bigint&) const;
    bool operator<  (const bigint&) const;
};

#endif
#pragma once
