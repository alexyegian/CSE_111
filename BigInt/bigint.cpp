// $Id: bigint.cpp,v 1.3 2020-10-11 12:47:51-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;
//asdf
#include "bigint.h"

bigint::bigint(long that) : uvalue(that), is_negative(that < 0) {
    //DEBUGF('~', this << " -> " << uvalue)
}

bigint::bigint(const ubigint& uvalue_, bool is_negative_) :
    uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint(const string& that) {
    is_negative = that.size() > 0 and that[0] == '_';
    uvalue = ubigint(that.substr(is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
    return *this;
}

bigint bigint::operator- () const {
    return { uvalue, not is_negative };
}

bigint bigint::operator+ (const bigint& that) const {
    ubigint result = uvalue + that.uvalue;
    return result;
}

bigint bigint::operator- (const bigint& that) const {
    ubigint result = uvalue - that.uvalue;
    return result;
}


bigint bigint::operator* (const bigint& that) const {
    bigint result = uvalue * that.uvalue;
    return result;
}

bigint bigint::operator/ (const bigint& that) const {
    bigint result = uvalue / that.uvalue;
    return result;
}

bigint bigint::operator% (const bigint& that) const {
    bigint result = uvalue % that.uvalue;
    return result;
}

bool bigint::operator== (const bigint& that) const {
    string thisString = this->uvalue.makeString();
    string thatString = that.uvalue.makeString();

    if (thisString == thatString)
    {
        return true;
    }
    return false;
}

bool bigint::operator< (const bigint& that) const {
    string thisString = this->uvalue.makeString();
    string thatString = that.uvalue.makeString();

    if (thisString < thatString)
    {
        return true;
    }
    return false;
}
string bigint::makeString() const {
    ubigint holder = this->uvalue;
    return holder.makeString();
}
bool bigint::returnIs_negative() const {
    return this->is_negative;
}
ostream& operator<< (ostream& out, const bigint& that) {
    string returnString = that.makeString();
    return out << "bigint(" << (that.returnIs_negative() ? "-" : "+")
        << "," << returnString << ")";
}