// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <string>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"
//TESTING
ubigint::ubigint() {
}
ubigint::ubigint(unsigned long value){
    // DEBUGF('~', this << " -> " << uvalue)
    int mod_val = 10;
    int ctr = 0;
    while (value != 0) {
        uvalue[ctr] = value % mod_val;
        value -= value % mod_val;
        mod_val *= 10;
        ctr++;
    }
}

ubigint::ubigint(const string& that){
     DEBUGF('~', "that = \"" << that << "\"");
    for (char digit : that) {
        if (not isdigit(digit)) {
            throw invalid_argument("ubigint::ubigint(" + that + ")");
        }
        uvalue.push_back(digit - '0');
    }
}

//SMALLER VALUE GETS PASSED AS PASSED
//
//Add starting from least significant, carry over 1 number.IE: 8 + 8 = 16. This can be done easily like this.Add numbers(and carry number),
//set carry number equal to result / 10, and set the number in that position equal to result % 10. Basically 5 + 7 + 1(carry bit) = 13. 
//New carry bit is 13 / 10 = 1. And set that digit equal to 13 % 10 = 3.


//CONTINUE IF ADD DIGIT ISNT 0 OR THAT VALUE.SIZE IS LESS THAN CTR

//FOR ALL ctr AND ctr2 VARIABLES IN LOOP CONDITIONALS: casted to type 'unsigned' to remove warning on compile, has no effect on code
ubigint ubigint::operator+ (const ubigint& that) const {
    //DEBUGF('u', *this << "+" << that);
    //ubigint ret_big = new ubigint();
    ubigint ret_big;
    int add_digit = 0;
    int ctr = 0;
    while (add_digit !=0 || that.uvalue.size() > static_cast<unsigned>(ctr)) {
        int result = add_digit + that.uvalue[ctr] + uvalue[ctr];
        ret_big.uvalue[ctr] = result % 10;
        add_digit = result / 10;
        ctr++;
    }
    //DEBUGF('u', ret_big);
    return ret_big;
}

//WE ASSUME THAT THE VALUE ON LEFT IS GREATER THAN VALUE ON RIGHT, THUS THAT IS SMALLER THAN THIS.
//cycles through from least sig to most sig, and if that[ctr]>this[ctr], borrow ahead from this[ctr+1]
//subtract 1 from this[ctr+1], and add 10 to result

//10
//-9
//--
// 1
ubigint ubigint::operator- (const ubigint& that) const {
    //DEBUGF('u', *this << "-" << that);
    //ubigint ret_big = new ubigint();
    ubigint ret_big;
    //ubigint one_big = 1;
    int ctr = 0;
    int sub_num = 0;
    while (sub_num != 0 || that.uvalue.size() > static_cast<unsigned>(ctr)) {
        int result = uvalue[ctr] - that.uvalue[ctr]-sub_num;
        if (result < 0) {
            sub_num = 1;
            result += 10;
        }
        else {
            sub_num = 0;
        }
        ret_big.uvalue[ctr] = result;
        ctr++;
    }
    //DEBUGF('u', ret_big);
    return ret_big;
}

//Do digit multiplied by digit, with a carry bit.Resultand carry bit are obtained the same way. 8 * 8 = 64, carry bit for previous was 3, 
//result now is 64 + 3 = 67. new carry bit is 67 / 10 = 6, that digit is now 67 % 10 = 7.
//This gives the result of one multiplication ie.the 3 in 12 * 13. Still need to add it with the others.Way to do this is to have 2 bigints,
//each store a result.Ie.one stores the 3 result in 12 * 13, and one stores the 1 result in 12 * 13. Then once both are full, add them 
//together and store in one of the two.
//Then for next multiplication step override the one not in use.
//Ie. 123 * 456: Store result of 123 * 6 in result 1. Then store result of 123 * 50 in result 2. Then add and store addition in result 1.
//Then store result of 123 * 400 in result 2. Then add and store addition in result 1. Then return that value.
//Or could just make starting digit equal to digit 2 place, and add 1 each time.Ie : 12 * 13, starting digit place for 12 * 3 is 0, starting digit place for 12 * 10 is 1.


ubigint ubigint::operator* (const ubigint& that) const {
    //DEBUGF('u', *this << "+" << that);
    //ubigint ret_big = new ubigint();
    //ubigint temp_big = new ubigint();    
    ubigint ret_big;
    ubigint temp_big;
    int carry_digit = 0;
    int ctr = 0;
    while (that.uvalue.size() > static_cast<unsigned>(ctr)) {
        int ctr2 = 0;
        while (carry_digit != 0 || uvalue.size() > static_cast<unsigned>(ctr2)) {
            int result = carry_digit + (that.uvalue[ctr2] * uvalue[ctr]);
            temp_big.uvalue[ctr+ctr2] = result % 10;
            carry_digit = result / 10;
            ctr2++;
        }
        ret_big = ret_big + temp_big;
        ctr++;
    }
    //DEBUGF('u', ret_big);
    return ret_big;
}

void ubigint::multiply_by_2() {
    ubigint two = 2;
    *this = *this * two;
}


//I ASSUME REMAINDER IS DISCARDED, AS THERER IS NO ROOM FOR IT
//LOOP THROUGH ALL VALUES IN UVALUE, FOR EACH VALUE IF VALUE%2 == 1
//ADD 5 TO VALUE[CTR-1], UNLESS CTR IS 0, IN WHICH CASE DISCARD
//THEN DIVIDE THE VALUE BY 2
void ubigint::divide_by_2() {
    int ctr = 0;
    while (uvalue.size() > static_cast<unsigned>(ctr)) {
        if (uvalue[ctr] % 2 == 1 && ctr != 0) {
            uvalue[ctr - 1] += 5;
        }
        uvalue[ctr] = uvalue[ctr] / 2;
    }
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide(const ubigint& dividend, const ubigint& divisor_) {
    // NOTE: udivide is a non-member function.
    ubigint divisor{ divisor_ };
    ubigint zero{ 0 };
    if (divisor == zero) throw domain_error("udivide by zero");
    ubigint power_of_2{ 1 };
    ubigint quotient{ 0 };
    ubigint remainder{ dividend }; // left operand, dividend
    while (divisor < remainder) {
        divisor.multiply_by_2();
        power_of_2.multiply_by_2();
    }
    while (power_of_2 > zero) {
        if (divisor <= remainder) {
            remainder = remainder - divisor;
            quotient = quotient + power_of_2;
        }
        divisor.divide_by_2();
        power_of_2.divide_by_2();
    }
    //DEBUGF('/', "quotient = " << quotient);
    //DEBUGF('/', "remainder = " << remainder);
    return { .quotient = quotient, .remainder = remainder };
}

ubigint ubigint::operator/ (const ubigint& that) const {
    return udivide(*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
    return udivide(*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
    if (this->uvalue.size() < that.uvalue.size() || this->uvalue.size() > that.uvalue.size()) 
    {
        return false;
    }

    int count = 0;
    while (static_cast<unsigned>(count) < this->uvalue.size())
    {
        if (this->uvalue[count] > that.uvalue[count] || this->uvalue[count] < that.uvalue[count])
        {
            return false;
        }
        count++;
    }
    return true;
}

bool ubigint::operator< (const ubigint& that) const {
    if (this->uvalue.size() < that.uvalue.size())
    {
        return true;
    }
    if (this->uvalue.size() > that.uvalue.size())
    {
        return false;
    }
    int count = 0;
    while (static_cast<unsigned>(count) < this->uvalue.size())
    {
        if (this->uvalue[count] < that.uvalue[count])
        {
            return true;
        }
        if (this->uvalue[count] > that.uvalue[count])
        {
            return false;
        }
        count++;
    }
    return false;
}

void ubigint::makeString() {
    int count = 0;
    string returnString;
    while (static_cast<unsigned>(count) < this->uvalue.size())
    {
        returnString = returnString + to_string(this->uvalue[count]);
        count++;
    }
    this->uvalueString = returnString;
}
ostream& operator<< (ostream& out, ubigint& that) {
    that.makeString();
    string returnString = that.uvalueString;
    return out << "ubigint(" << returnString << ")";
}
