//
// Created by astro on 11/12/2024.
//
//author: Carlos Reyes
// REDID: 131068259

#include "symbol.h"
    //constructor for our symbol along with setting its attributes
   symbol::symbol(string SymbolName, string Address, char Flag){
    this->symbolName = SymbolName;
    this->address = Address;
    this->flag = Flag;
}

