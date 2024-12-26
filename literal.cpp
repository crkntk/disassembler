//
// Created by astro on 11/12/2024.
//
//author: Carlos Reyes
// REDID: 131068259

#include "literal.h"
//this is the constructtor for our literal along with the value attributes set in this case we
//have close names to our attributes
literal::literal(string litName, string litVal, string litLen, string litAdd){
    this->name = litName;
    this->value =litVal;
    this->length = litLen;
    this->address = litAdd;

}