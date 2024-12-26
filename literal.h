//
// Created by astro on 11/12/2024.
//
//author: Carlos Reyes
// REDID: 131068259

#ifndef ASSG_1_LITERAL_H
#define ASSG_1_LITERAL_H
#include <string>
using namespace std;
//this is a literal object we create in order to store all the info we need for each line in our literal table file
class literal {
    public:
    //This is a constructor for a literal that take in the name value length and address of the literal
    //we only express hex so the length calculation later and type can be calculated without worrying about
    //literal type name is used for writing purposes value is used for operand purposes and length is used for
    //changing our location counter the address is where the literal belongs on that address in our outupt file
        literal(string, string, string, string);
        string name;
        string value;
        string length;
        string address;
};


#endif //ASSG_1_LITERAL_H
