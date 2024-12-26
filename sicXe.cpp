//
// Created by Carlos Reyes on 9/20/2024.
// Red ID: 131068259
//

#include "sicXe.h"
#include <string>
#include <stdio.h>
using namespace std;
// this structure dictates the ops number hexadecimal the index is connected to mneumonic
//  and format3 checks array
const static string ops[] = {
    "18", "58", "90", "40", "B4", "28",
    "88", "A0", "24", "64", "9C", "C4",
    "C0", "F4", "3C", "30", "34", "38",
    "48", "00", "68", "50", "70", "08",
    "6C", "74", "04", "D0", "20", "60",
    "98", "C8", "44", "D8", "AC", "4C",
    "A4", "A8", "F0", "EC", "0C", "78",
    "54", "80", "D4", "14", "7C", "E8",
    "84", "10", "1C", "5C", "94", "B0",
    "E0", "F8", "2C", "B8", "DC"};
const static string mnemonics[] = {
    "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP",
    "COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX",
    "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT",
    "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL",
    "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
    "MULR", "NORM", "OR", "RD", "RMO", "RSUB",
    "SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB",
    "STCH", "STF", "STI", "STL", "STS", "STSW",
    "STT", "STX", "SUB", "SUBF", "SUBR", "SVC",
    "TD", "TIO", "TIX", "TIXR", "WD"};
const static bool format2[] = {
    false, false, true, false, true, false,
    false, true, false, false, true, false,
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    true, false, false, false, true, false,
    true, true, false, false, false, false,
    false, false, false, false, false, false,
    false, false, false, false, true, true,
    false, false, false, true, false};
sicXe::sicXe(unsigned int first8bits, unsigned int second8bits)
{
    // Constructor for instruction and its attributes
    // takes in the first two hex numbers thos are the bits that
    // dictated the operand
    this->objCode = "";
    string hexString = "";
    // turn the first hex number to a number hex bit representation by 8 bits char
    char firstHex = NumToHex(first8bits);
    // We start building a mask in order to mask the first two bits
    unsigned int secondNumMask = 0b11111100;

    // we mask our bits with bitwise and operation
    unsigned int secondOpNum = second8bits & secondNumMask;
    // we turn ouur masked number into a hex number
    char secondHex = NumToHex((char)secondOpNum);
    char firstHexSt[2];
    firstHexSt[0] = firstHex;
    firstHexSt[1] = '\0';
    char secondHexSt[2];
    secondHexSt[0] = secondHex;
    secondHexSt[1] = '\0';
    hexString.append(firstHexSt);
    hexString.append(secondHexSt);
    hexString = hexString.substr(0, 2);
    // we save the hex string as an attribute and find the index in our hex oprand array
    // so we can find the corresponding mneumonic in the mneumonics array
    this->op = hexString;
    this->arrIndex = getopIndex(hexString);
    unsigned int opsMneumonicIndex = arrIndex;
    this->mnemonic = mnemonics[opsMneumonicIndex];
    // we check to see if our mneumonic is a format 2 specfiic mneumonic
    this->format2Check = format2[arrIndex];
    // we check if its a format 2 if it is we set it to format 2
    if (this->format2Check)
    {
        this->format = 2;
    }
}
void sicXe::setFormat2Operands(char op1, char op2)
{
    // setter to set the format 2 operands this is a char representation of the hex number
    this->operand1 = op1;
    this->operand2 = op2;
}
unsigned int sicXe::getopIndex(string operand)
{
    // looks for the operand input in the ops and returns corresponding index returns 0 otherwise
    unsigned int index = 0;
    int opsLength = sizeof(ops) / sizeof(ops[0]);
    for (int i = 0; i < opsLength; i++)
    {
        if (ops[i] == operand)
        {
            return i;
        }
    }
    return 0;
}
void sicXe::setFormatThreeFour(unsigned int formatFlagUnmasked)
{
    // this function sets the format to either format 3 or 4 based on our
    // hex number bit that determines the format which is the first bit
    // we create a mask and bitwise and our format bit 0 means its format 3 and 1 means its format 4
    unsigned int formatCheck = formatFlagUnmasked & 1;
    if (formatCheck == 0)
    {
        this->format = 3;
    }
    else
    {
        this->format = 4;
    }
}
void sicXe::setAddressingModes(unsigned int addrUnmasked)
{
    // this function sets the addressing mode for both format 3 and 4
    // We create our masks to set our mode as
    // pc, base, or absolute  pc_indexed, base_indexed, or direct_indexed.
    unsigned int baseMask = 4;
    unsigned int pcMask = 2;
    unsigned int indexMask = 8;
    // we and in order to isolate the bit then shift to the right so we get a single bit either
    //  1 or 0
    unsigned int baseFlag = (addrUnmasked & baseMask) >> 2;
    unsigned int pcFlag = (addrUnmasked & pcMask) >> 1;
    unsigned int indexFlag = (addrUnmasked & indexMask) >> 3;
    // Format 3 and 4 each have their different type of addressing and format 4 does have base or pc
    // so we check for the format first then we check for the flag corresponding to that
    // flag. After that we check if we are using an indexed based mode and go down our logic tree furthu
    if ((this->format) == 3)
    {
        // base or pc flag logical branch check
        if (baseFlag == 1)
        {
            // index flag check
            if (indexFlag == 1)
            {
                this->addressingMode = "base_indexed";
            }
            else
            {
                this->addressingMode = "base";
            }
        }
        else if (pcFlag == 1)
        {
            // index flag check
            if (indexFlag == 1)
            {
                this->addressingMode = "pc_indexed";
            }
            else
            {
                this->addressingMode = "pc";
            }
        }
        else
        {
            this->addressingMode = "absolute";
        }
    }
    else if ((this->format) == 4)
    {
        // index flag check for format 4
        if (indexFlag == 1)
        {
            this->addressingMode = "direct_indexed";
        }
        else
        {
            this->addressingMode = "absolute";
        }
    }
}
void sicXe::setAdrressingType(unsigned int addrUnmasked)
{
    // This function sets the addressing type for format 3 and 4 our address input
    // is from the second hex in our instruction we set them too
    //  simple, immediate, indirect
    //  we create masks and shift the bits for our flags so we get a 1 or 0 depending on our bit set in our
    // input unmasked number and dependent on sic/Xe instruction structure
    unsigned int immedMask = 1;
    unsigned int indMask = 2;
    unsigned int immFlag = addrUnmasked & immedMask;
    unsigned int indFlag = (addrUnmasked & indMask) >> 1;
    // we check our flags simultaneously in order to not create over logic branching
    // we set our addressing type according to our flag number
    if (immFlag == 1 && indFlag == 0)
    {
        this->addressingType = "immediate";
    }
    else if (immFlag == 0 && indFlag == 1)
    {
        this->addressingType = "indirect";
    }
    else
    {
        this->addressingType = "simple";
    }
}

void sicXe::setAddress(string address)
{
    // sets the address for format 4 instruction
    this->addr = address;
}

void sicXe::setDisplacement(string displacement)
{
    // Function that sets the displacement attribute for format 3
    this->disp = displacement;
}

void sicXe::buildObjCode(char codeChar)
{
    // This function appends a character to our object code by using the input character
    // we need null termination for this compiler
    char singleCharStr[2];
    singleCharStr[0] = codeChar;
    singleCharStr[1] = '\0';
    this->objCode += singleCharStr;
}

char sicXe::NumToHex(unsigned int Number)
{
    // Turn Number input into hex number
    if (Number <= 9)
    {
        return Number + '0';
    }
    else
    {
        return 'A' + (Number % 10);
    }
}
