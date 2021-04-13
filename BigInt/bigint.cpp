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
    const bool thisNegative = this->returnIs_negative();
    const bool thatNegative = that.is_negative;
    const bigint thisCopy = bigint(this->uvalue, this->is_negative);
    if (thisNegative == true && thatNegative == true)
    {
        ubigint result = uvalue + that.uvalue;
        bigint returnBigint = bigint(result, true);
        return result;
    }
    if (thisNegative == false && thatNegative == false)
    {
        ubigint result = uvalue + that.uvalue;
        return result;
    }
    if (thisNegative == true && thatNegative == false)
    {
        const bigint thisCopy1 = bigint(thisCopy.uvalue, false);
        if (that < thisCopy1)
        {
            ubigint result = uvalue - that.uvalue;
            bigint returnBigint = bigint(result, true);
            return result;
        }
        else
        {
            ubigint result = that.uvalue - uvalue;
            bigint returnBigint = bigint(result, false);
            return result;
        }
    }
    if (thisNegative == false && thatNegative == true)
    {
        const bigint thatCopy = bigint(that.uvalue, false);
        if (thisCopy < thatCopy)
        {
            ubigint result = that.uvalue - uvalue;
            bigint returnBigint = bigint(result, true);
            return result;
        }
        else
        {
            ubigint result = uvalue - that.uvalue;
            bigint returnBigint = bigint(result, false);
            return result;
        }
    }
}

bigint bigint::operator- (const bigint& that) const {
    const bool thisNegative = this->is_negative;
    const bool thatNegative = that.is_negative;
    const bigint thisCopy = bigint(this->uvalue, this->is_negative);
    if (thisNegative == true && thatNegative == true)
    {
        if (thisCopy < that)
        {
            ubigint result = uvalue - that.uvalue;
            bigint returnBigint = bigint(result, true);
            return result;
        }
        else
        {
            ubigint result = that.uvalue - uvalue;
            bigint returnBigint = bigint(result, false);
            return result;
        }
    }
    if (thisNegative == false && thatNegative == false)
    {
        if (thisCopy < that)
        {
            ubigint result = that.uvalue - uvalue;
            bigint returnBigint = bigint(result, true);
            return result;
        }
        else
        {
            ubigint result = uvalue - that.uvalue;
            bigint returnBigint = bigint(result, false);
            return result;
        }
    }
    if (thisNegative == false && thatNegative == true)
    {
        ubigint result = uvalue + that.uvalue;
        return result;
    }
    if (thisNegative == true && thatNegative == false)
    {
        ubigint result = uvalue + that.uvalue;
        bigint returnBigint = bigint(result, true);
        return returnBigint;
    }
}


bigint bigint::operator* (const bigint& that) const {
    bigint result = uvalue * that.uvalue;
    const bool thisNegative = this->is_negative;
    const bool thatNegative = that.is_negative;
    if (thisNegative == true && thatNegative == true) return result;
    if (thisNegative == false && thatNegative == false) return result;
    bigint returnBigint = bigint(result.uvalue, true);
    return returnBigint;
}

bigint bigint::operator/ (const bigint& that) const {
    bigint result = uvalue / that.uvalue;
    const bool thisNegative = this->is_negative;
    const bool thatNegative = that.is_negative;
    if (thisNegative == true && thatNegative == true) return result;
    if (thisNegative == false && thatNegative == false) return result;
    bigint returnBigint = bigint(result.uvalue, true);
    return returnBigint;
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
    const bool thisNegative = this->is_negative;
    const bool thatNegative = that.is_negative;
    cout << "BIGINT LESS THAN OPERATOR: " << thisString << " " << thatString << ": ";

    if (thisNegative == true && thatNegative == false)
    {
        return true;
    }
    if (thisNegative == false && thatNegative == true)
    {
        return false;
    }

    if (thisString.size() < thatString.size())
    {
        cout << "TRUE" << endl;
        if (thisNegative == true) return true;
        else return false;
    }
    if (thisString.size() > thatString.size())
    {
        cout << "FALSE" << endl;
        if (thisNegative == true) return false;
        else return true;
    }
    unsigned long count = 0;
    while (count < thisString.size())
    {
        char thisChar = thisString[count];
        char thatChar = thatString[count];
        int thisInt = thisChar - '0';
        int thatInt = thatChar - '0';
        if (thisInt < thatInt)
        {
            cout << "TRUE" << endl;
            if (thisNegative == true) return true;
            else return false;
        }
        if (thisInt > thatInt)
        {
            cout << "FALSE" << endl;
            if (thisNegative == true) return false;
            else return true;
        }
        count++;
    }
    cout << "FALSE" << endl;
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