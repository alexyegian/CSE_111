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

//-2, -2 +
bigint bigint::operator+ (const bigint& that) const {
    const bool thisNegative = this->returnIs_negative();
    const bool thatNegative = that.is_negative;
    const bigint thisCopy = bigint(this->uvalue, this->is_negative);
    //printf("THIS NEGATIVE: %d THAT NEGATIVE: %d\n", thisNegative, thatNegative);
    //good
    if (thisNegative == true && thatNegative == true)
    {
        ubigint result = uvalue + that.uvalue;
        bigint returnBigint = bigint(result, true);
        //printf("RETURN BIG INT: NEGATIVE: %d VALUE: %s\n", returnBigint.is_negative, returnBigint.uvalue.makeString().c_str());
        return returnBigint;
    }
    //good
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
            return returnBigint;
        }
        else
        {
            ubigint result = that.uvalue - uvalue;
            bigint returnBigint = bigint(result, false);
            return returnBigint;
        }
    }
    if (thisNegative == false && thatNegative == true)
    {
        const bigint thatCopy = bigint(that.uvalue, false);
        if (thisCopy < thatCopy)
        {
            ubigint result = that.uvalue - uvalue;
            bigint returnBigint = bigint(result, true);
            return returnBigint;
        }
        else
        {
            ubigint result = uvalue - that.uvalue;
            bigint returnBigint = bigint(result, false);
            return returnBigint;
        }
    }
}

bigint bigint::operator- (const bigint& that) const {
    const bool thisNegative = this->is_negative;
    const bool thatNegative = that.is_negative;
    const bigint thisCopy = bigint(this->uvalue, this->is_negative);
    //-4 - -5 = 1, subtraction, -5 - -4 = -1, subtraction
    //lesser in back
    //THESE OKAY
    //printf("SUBTRACTION: THIS NEG: %d THAT NEG: %d THIS UVAL: %s THAT UVAL: %s\n", thisNegative, thatNegative, thisCopy.uvalue.makeString().c_str(), that.uvalue.makeString().c_str());
    if (thisNegative == true && thatNegative == true)
    {
        //add
        if (thisCopy < that)
        {
            //-4 - -5 = 1, same as 5 - 4
            //ubigint result = uvalue - that.uvalue;
            ubigint result = uvalue - that.uvalue;
            bigint returnBigint = bigint(result, true);
            return returnBigint;
        }
        else
        {
            //-5 - -4 = -1, same as -(5-4)
            ubigint result = that.uvalue - uvalue;
            //ubigint result = uvalue - that.uvalue;
            bigint returnBigint = bigint(result, false);
            return returnBigint;
        }
    }
    //4 - 5 = -1, subtraction, 5 - 4, subtraction
    //lesser in back
    //THESE NOT OKAY
    if (thisNegative == false && thatNegative == false)
    {
        if (thisCopy < that)
        {
            //4 - 5, same as 5 - 4 = -(1)
            //ubigint result = that.uvalue - uvalue;
            //printf("NO NEGS THIS LESS THAN THAT\n");
            ubigint result = that.uvalue - uvalue;
            bigint returnBigint = bigint(result, true);
            return returnBigint;
        }
        else
        {
            //5-4, same as 5-4 = 1
            //ubigint result = uvalue - that.uvalue;
            //printf("NO NEGS THIS MORE THAN THAT\n");
            //printf("SUBTRACTING THIS FROM THAT NO NEGS\n");
            ubigint result = uvalue - that.uvalue;
            bigint returnBigint = bigint(result, false);
            return returnBigint;
        }
    }
    //5 - -4 = 9, addition 
    //THESE OKAY
    if (thisNegative == false && thatNegative == true)
    {
        ubigint result = uvalue + that.uvalue;
        return result;
    }
    //-5 - 4 = -9, subtraction
    //THESE OKAY
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
   // printf("LESS THAN TESTING\n\n");
   // printf("THIS NEG: %d THAT NEG: %d THIS: %s THAT: %s\n", thisNegative, thatNegative, thisString.c_str(), thatString.c_str());
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
        //printf("THIS LESS THAN NEG: %d\n", thisNegative);
        //flipped again
        //5(size 1) < 10 (size 2), this neg false, return false, meaning greater
        if (thisNegative == true) return false;
        else return true;
    }
    if (thisString.size() > thatString.size())
    {
        //printf("THIS LARGER THAN NEG: %d\n", thisNegative);
        //Flipped again
        //10 (size 2) > 5 (size 1), this neg false, return true, meaning less
        if (thisNegative == true) return true;
        else return false;
    }
    unsigned long count = 0;
    while (count < thisString.size())
    {
        char thisChar = thisString[count];
        char thatChar = thatString[count];
        //printf("THIS CHAR: %c THAT CHAR: %c\n", thisChar, thatChar);
        int thisInt = thisChar - '0';
        int thatInt = thatChar - '0';
       // printf("THIS INT: %d THAT INT: %d\n", thisInt, thatInt);
        if (thisInt < thatInt)
        {
        //    printf("THIS INT LESS IS TRUE THIS NEG: %d\n", thisNegative);
            //I THINK THESE ARE FLIPPED
            //this: 4 that: 5, this negative is false, return false, meaning more
            if (thisNegative == true) return false;
            else return true;
        }
        if (thisInt > thatInt)
        {
         //   printf("THIS INT MORE IS TRUE THIS NEG: %d\n", thisNegative);
            //THINK THESE ARE FLIPPED
            //this: 5, that 4, this negative is false, return true, meaning less.
            if (thisNegative == true) return true;
            else return false;
        }
        count++;
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
    //printf("TRYING TO PRINT NEGATIVE: %d\n", that.is_negative);
    string returnString = that.makeString();
    if (that.returnIs_negative() == false) return out << returnString;

    return out << (that.returnIs_negative() ? "-" : "+")
        << returnString;
}