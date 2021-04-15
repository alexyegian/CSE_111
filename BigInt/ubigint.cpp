//Alex Soliz ID: amsoliz
//Alexander Yegian ID: ayegian
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
//comment

ubigint::ubigint() {
}
ubigint::ubigint(unsigned long value){
    int ctr = 0;
    while (value != 0 || ctr == 0) {
        uvalue.push_back(value % 10);
        value -= value % 10;
        value = value / 10;
        ctr++;
    }
}


ubigint::ubigint(const string& that) {
    int count = 0;
    while (static_cast<unsigned>(count) < that.size()) {
        if (not isdigit(that[that.size() - count - 1])) {
            throw invalid_argument("ubigint::ubigint(" + that + ")");
        }
        uvalue.push_back(that[that.size() - count - 1] - '0');
        count++;
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
    ubigint ret_big;
    int add_digit = 0;
    int ctr = 0;
    while (add_digit !=0 || that.uvalue.size() > static_cast<unsigned>(ctr)|| uvalue.size()>static_cast<unsigned>(ctr)) {
        int result = add_digit;
        if (that.uvalue.size() > static_cast<unsigned>(ctr)) {
            result += that.uvalue[ctr];
        }
        if (uvalue.size() > static_cast<unsigned>(ctr)) {
            result += uvalue[ctr];
        }
        ret_big.uvalue.push_back(result % 10);
        add_digit = result / 10;
        ctr++;
    }
    return ret_big;
}

//WE ASSUME THAT THE VALUE ON LEFT IS GREATER THAN VALUE ON RIGHT, THUS THAT IS SMALLER THAN THIS.
//cycles through from least sig to most sig, and if that[ctr]>this[ctr], borrow ahead from this[ctr+1]
//subtract 1 from this[ctr+1], and add 10 to result



ubigint ubigint::operator- (const ubigint& that) const {
    ubigint ret_big;
    int ctr = 0;
    int sub_num = 0;
    while (sub_num != 0 || that.uvalue.size() > static_cast<unsigned>(ctr) || uvalue.size() > static_cast<unsigned>(ctr)) {
        int result = -1*sub_num;
        if (uvalue.size() > static_cast<unsigned>(ctr)) {
            result += uvalue[ctr];
        }
        if (that.uvalue.size() > static_cast<unsigned>(ctr)) {
            result -= that.uvalue[ctr];
        }
        if (result < 0) {
            sub_num = 1;
            result += 10;
        }
        else {
            sub_num = 0;
        }
        ret_big.uvalue.push_back(result);
        ctr++;
    }
    for (int i = ret_big.uvalue.size() - 1; i > 0; i--) {
        if (ret_big.uvalue[i] == 0) {
            ret_big.uvalue.pop_back();
        }
        else {
            break;
        }
    }
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
    ubigint ret_big;
    int carry_digit = 0;
    int ctr = 0;
    while (that.uvalue.size() > static_cast<unsigned>(ctr)) {
        ubigint temp_big;
        int ctr2 = 0;
        for (int i = 0; i < ctr; i++) {
            temp_big.uvalue.push_back(0);
        }
        while (carry_digit != 0 || uvalue.size() > static_cast<unsigned>(ctr2)) {
            int result = carry_digit;
            if (uvalue.size() > static_cast<unsigned>(ctr2)) {
                result += (that.uvalue[ctr] * uvalue[ctr2]);
            }
            temp_big.uvalue.push_back(result % 10);
            carry_digit = result / 10;
            ctr2++;
        }
        ret_big = temp_big+ret_big;
        ctr++;
    }
    return ret_big;
}

void ubigint::multiply_by_2() {
    int ctr = 0;
    while (uvalue.size() > static_cast<unsigned>(ctr)) {
        ctr++;
    }
    ubigint two = 2;
    *this = *this * two;
    ctr = 0;
    while (uvalue.size() > static_cast<unsigned>(ctr)) {
        ctr++;
    }
}


//I ASSUME REMAINDER IS DISCARDED, AS THERER IS NO ROOM FOR IT
//LOOP THROUGH ALL VALUES IN UVALUE, FOR EACH VALUE IF VALUE%2 == 1
//ADD 5 TO VALUE[CTR-1], UNLESS CTR IS 0, IN WHICH CASE DISCARD
//THEN DIVIDE THE VALUE BY 2

//NEED TO POP ALL VALUES THAT ARE 0 FROM END, IE VECTOR 100 SIZE 3, DIVIDED BY 2 IS 050, SIZE 3, SHOULD BE 50, SIZE 2.
void ubigint::divide_by_2() {
    int ctr = 0;
    while (uvalue.size() > static_cast<unsigned>(ctr)) {
        ctr++;
    }
    ctr = 0;
    while (uvalue.size() > static_cast<unsigned>(ctr)) {
        if (uvalue[ctr] % 2 == 1 && ctr != 0) {
            uvalue[ctr - 1] += 5;
        }
        uvalue[ctr] = uvalue[ctr] / 2;
        ctr++;
    }
    //TRYING TO POP FROM END
    for (int i = uvalue.size() - 1; i > 0; i--) {
        if (uvalue[i] == 0) {
            uvalue.pop_back();
        }
        else {
            break;
        }
    }
    ctr = 0;
    while (uvalue.size() > static_cast<unsigned>(ctr)) {
        ctr++;
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
        if (divisor < remainder || divisor == remainder) {
            remainder = remainder - divisor;
            quotient = quotient + power_of_2;
        }
        divisor.divide_by_2();
        power_of_2.divide_by_2();
    }
    return { .quotient = quotient, .remainder = remainder };
}

ubigint ubigint::operator/ (const ubigint& that) const {
    return udivide(*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
    return udivide(*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
    string thisString = this->makeString();
    string thatString = that.makeString();

    if (thisString == thatString)
    {
        return true;
    }
    return false;
}

bool ubigint::operator< (const ubigint& that) const {
    string thisString = this->makeString();
    string thatString = that.makeString();
    
    if(thisString.size() < thatString.size())
    {
        return true;
    }
    if (thisString.size() > thatString.size())
    {
        return false;
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
            return true;
        }
        if (thisInt > thatInt)
        {
            return false;
        }
        count++;
    }
    return false;
}

string ubigint::makeString() const {
    int count = 0;
    string returnString;
    while (static_cast<unsigned>(count) < this->uvalue.size())
    {
        returnString = returnString + to_string(this->uvalue[this->uvalue.size() - 1 - count]);
        count++;
        if (count % 69 == 0)
        {
            returnString = returnString + "\\" + "\n";
        }
    }
    return returnString;
}
ostream& operator<< (ostream& out, const ubigint& that) {
    string holdString = that.makeString();
    return out << holdString;
}