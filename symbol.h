//
// Created by astro on 11/12/2024.
//
//author: Carlos Reyes
// REDID: 131068259

#ifndef ASSG_1_SYMBOL_H
#define ASSG_1_SYMBOL_H
#include <string>
using namespace std;
//This class represents a symbol line in the sym file it is a record of the symbole
//along with its information needed for our dissasembler
class symbol {
    public:
    //construction takes in a string for symbol name, address of the symbol in our text record and the flag indication
    //what type it. We are only dealing with relative addrssing in this case.
        symbol(string, string, char);
        //name of our symbol
        string symbolName;
        //address location where this symbol belongs
        string address;
        //flag to indicate type
        char flag;

};


#endif //ASSG_1_SYMBOL_H
