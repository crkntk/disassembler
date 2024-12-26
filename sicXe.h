//
// Created by Carlos Reyes on 9/20/2024.
// REDID: 131068259
//

#ifndef ASSG_1_SICXE_H
#define ASSG_1_SICXE_H
#include <string>
using namespace std;
// This class is a representation of the structure of the
//  sic/XE instruction set along with modes formats and types
// we don't represent format 1 in this structure
class sicXe
{
public:
    // our object constructor using two int values to get first hex
    // value and 2nd hex value
    sicXe(unsigned int, unsigned int);
    // format for instruction depending on bits and mneumonic string
    unsigned int format;
    // string with object code and operand string and mneumonic for instruction
    std::string objCode;
    std::string op;
    std::string mnemonic;
    // for format 2: these are the operands at the end of the instruction
    char operand1;
    char operand2;
    // bool that checks if the instuction is format 2
    bool format2Check;
    // for format 3 and 4: addressing modes set by other hex bits strings to write to the file output
    string addressingMode;
    string addressingType;
    // for format 3
    string disp;
    // for format 4
    string addr;
    // Function to build the object code written to file
    void buildObjCode(char);
    // function that sets the attribute operands for format 2
    void setFormat2Operands(char, char);
    // functions that sets format attribute for format 3 and 4 depending on flag bits
    void setFormatThreeFour(unsigned int formatFlagUnmasked);
    // functions for setting attributes for modes and types dependent on flag bits for format 3 and 4
    void setAddressingModes(unsigned int);
    void setAdrressingType(unsigned int);
    // setter functions to set address for format 4 and displacement for format 3
    void setAddress(string);
    void setDisplacement(string);

private:
    // function so class can get the operation index that pertains the the data structure array
    unsigned int getopIndex(string);
    // converstion of integer number to hex number
    char NumToHex(unsigned int);
    // attribute of the index that belongs to the operation arraay and corresponds to the index in the mneoumonic array and
    // format2 operands array
    unsigned int arrIndex;
};
#endif // ASSG_1_SICXE_H
