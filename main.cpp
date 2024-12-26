// author: Carlos Reyes
//  REDID: 131068259

#include <iostream>
#include <string>
#include <fstream>
#include "sicXe.h"
#include "symbol.h"
#include "literal.h"
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <queue>
#include <map>
using namespace std;
// This are our register names connected to their numbers in order to obtain them for format 2
//  instructions
const static string RegisterTable[] = {
    "A", "X", "L", "B", "S", "T", "F", "", "PC", "SW"};

unsigned int charToHexNum(char character)
{
    // This function turns a character in 8 bits into a hexadecimal number
    unsigned int hex = character - '0';
    // branch for number higher than 9 like A-F hexadecimal
    if (hex > 9)
    {
        return hex - 7;
    }
    else
    {
        return hex;
    }
}
bool is_hex(char value)
{
    // this function returns a booleans that checks if the input char value is a hexadecimal
    bool inNumRange = ('0' <= value) && (value <= '9');
    bool inCharHexRange = ('A' <= value) && (value <= 'F');
    return inNumRange || inCharHexRange;
}
int main(int argc, char **argv)
{
    // our main function setting up the file name and file object to read from
    // we set both our object file as ifs and our symbol file streams
    ifstream ifs;
    ifstream symStream;
    // our file name
    // Here wer are setting up both our file names
    string fileName = argv[1];
    string symFileName = argv[2];
    // this is initialization of the instructions record queue in order to write after parsing the object code into
    // ints sic Xe instruction format
    std::queue<sicXe *> instRecordsQueue;
    // open both files
    ifs.open(fileName, ifstream::in);
    symStream.open(symFileName, ifstream::in);
    // create the tables for symbols and literals using maps the addresses will be the keys and the object will be the values
    std::unordered_map<std::string, symbol *> symTable;
    std::unordered_map<std::string, literal *> litTable;
    // In this while loop we parse through our sym file to obtain
    // both our symbol table and our literal table
    char tableline[257];
    tableline[256] = '\0';
    while (symStream.good())
    {
        // get first line and line string declaration
        // we parse through both our first lines since they have the headers of our symbol table only
        symStream.getline(tableline, 256);
        symStream.getline(tableline, 256);
        // We create our literal table header for comparing our line
        string literalHead = "Name    Lit_Const  Length Address:";
        // We get our first line for our symbol table
        symStream.getline(tableline, 256);
        // We parse here our symbols and put them in our symbol table
        // for debuging
        int count = 0;
        string interLines = tableline;
        // we parse throu our table file and check to see if there are no white lines
        while (strlen(tableline) != 1 && tableline[0] != 0 && strcmp(tableline, literalHead.c_str()) != 0)
        {
            // symbol name to obtain from line
            string firstSymbolAttribute = "";
            // symbol address
            string secondSymbolAttribute = "";
            // symbol flag
            string thirdSymbolAttribute = "";
            // we start a string stream from the table line extracted from file
            // we grab the first line using space delimeter
            string recordLine = (const char *)tableline;
            stringstream ss(recordLine.c_str());
            ss >> firstSymbolAttribute;
            ss >> secondSymbolAttribute;
            ss >> thirdSymbolAttribute;
            // create a symbol object for our symbole with attributes
            symbol *recSymbol = new symbol(firstSymbolAttribute, secondSymbolAttribute, thirdSymbolAttribute[0]);
            // we put the address of the symbol as key and get next line
            symTable[secondSymbolAttribute] = recSymbol;
            symStream.getline(tableline, 256);
            interLines = tableline;
        }
        //  We get rid of the white lines between tables
        interLines = tableline;
        while (strlen(tableline) == 1)
        {
            symStream.getline(tableline, 256);
            interLines = tableline;
        }
        //  parse the header of the literal table
        symStream.getline(tableline, 256);
        symStream.getline(tableline, 256);
        // We that we do have a line with characters adn the literal table still has lines
        // In this loop we parse to populate our literal table
        while (symStream.good() && strlen(tableline) != 1 && tableline[0] != 0)
        {
            printf("%s\n", tableline);
            // our attributes for our literal table
            // Name of literal
            string firstLitAttribute = "";
            // value
            string secondLitAttribute = "";
            // length
            string thirdLitAttribute = "";
            // address
            string fourthLitAttribute = "";
            // we start our streams for literal line
            string recordLine = (const char *)tableline;
            stringstream ss(recordLine.c_str());
            // we get the first attribute from streem
            ss >> firstLitAttribute;
            // we get the other attributes from stream
            ss >> secondLitAttribute;
            ss >> thirdLitAttribute;
            ss >> fourthLitAttribute;
            // We create our literal object with our attributes
            if (fourthLitAttribute == "")
            {
                literal *recLiteral = new literal("", firstLitAttribute, secondLitAttribute, thirdLitAttribute);
                litTable[thirdLitAttribute] = recLiteral;
            }
            else
            {
                literal *recLiteral = new literal(firstLitAttribute, secondLitAttribute, thirdLitAttribute, fourthLitAttribute);
                litTable[fourthLitAttribute] = recLiteral;
            }
            // We put the address as key and our liter object in our literal maps
            symStream.getline(tableline, 256);
        }
        break;
    }
    // extracting our character from file meaning the first 8 bits
    // each extracted char represents a hexadecimal so conversion has to take place
    char textChar;
    // while our file has not reached or errored out
    // we want to check our file does open correctly
    if (ifs.good())
    {
        // We set our output file to write to and open to create it
        ofstream outFile;
        outFile.open("out.lst");
        // goes through header meaning it reads the first line
        // we need name of program start location and length
        // We need name for our first and last line and we need the starting location for our Location counter
        string progName = "";
        string progStartLoc = "";
        string progLength = "";
        // Our base register declaration
        string BASE;
        // firstH in our file
        textChar = ifs.get();
        int firstLineCount = 0;
        // getting program length and name and start location
        // In this loop we are building our strings charaacter by character in order to get this information
        while (textChar != '\n')
        {
            textChar = ifs.get();
            if (firstLineCount < 6)
            {
                progName = progName + textChar;
            }
            else if (firstLineCount < 12)
            {
                progStartLoc = progStartLoc + textChar;
            }
            else
            {
                progLength = progLength + textChar;
            }
            firstLineCount += 1;
        }
        // We initialize our location counter to an integer from our program string assumed to be
        // a hexadecimal string integer is easier to add and subtract in order to increment through location counter
        int LOC = stoi(progStartLoc, nullptr, 16);
        int countInit = 0;
        // write first line
        // we are using 12 chars for columns eperations we first write program start location
        // we need write oru first line with our program location
        int numberSeperation = 12;
        string firstLineWrite = progStartLoc.substr(2);
        int whiteSpace = numberSeperation - firstLineWrite.length();
        outFile.write(firstLineWrite.c_str(), firstLineWrite.length());
        string columnSpace = "";
        // white space between column creation
        while (whiteSpace != 0)
        {
            columnSpace += " ";
            whiteSpace -= 1;
        }
        outFile.write(columnSpace.c_str(), columnSpace.length());
        // writing our program name in first line
        outFile.write(progName.c_str(), progName.length());
        whiteSpace = numberSeperation - progName.length();
        // White space after program name writing into file
        columnSpace = "";
        while (whiteSpace != 0)
        {
            columnSpace += " ";
            whiteSpace -= 1;
        }
        // We write start into our first file as directive of start location
        outFile.write(columnSpace.c_str(), columnSpace.length());
        string firstSymbol = "Start";
        outFile.write(firstSymbol.c_str(), firstSymbol.length());
        whiteSpace = numberSeperation - firstSymbol.length();
        // white space between columns
        columnSpace = "";
        while (whiteSpace != 0)
        {
            columnSpace += " ";
            whiteSpace -= 1;
        }
        // Here we write our program location
        // we need to write 0 for location starting at 0 and we add
        // more as we need for program relocation this should be changed
        // to reflect the 4 hex address
        outFile.write(columnSpace.c_str(), columnSpace.length());
        firstLineWrite = progStartLoc.substr(progStartLoc.length() - 1);
        whiteSpace = numberSeperation - firstLineWrite.length();
        outFile.write(firstLineWrite.c_str(), firstLineWrite.length());
        // white space between columns
        columnSpace = "";
        while (whiteSpace != 0)
        {
            columnSpace += " ";
            whiteSpace -= 1;
        }
        columnSpace += "\n";
        outFile.write(columnSpace.c_str(), columnSpace.length());
        // first characters unrelated to instruction conversion
        // we need this for instruction starting address relative to starting of program
        // and we need the length of the text record
        // The last instruction was a pc instruction but pc starts at a different location after flag
        bool pcInstLast = false;
        // our instruction to write to file
        sicXe *instWrite;
        // our hexadecimal storage to translate between integer and hexadecimal
        char Operand_hex_string[4];
        Operand_hex_string[4] = '\0';
        // our index register as integer
        int OperandIndex = 0;
        // our index register
        int INDEX;
        // our addressing type flag to write with operand
        string addressingTypeflag;
        // the length of the value that was written to file
        int writenLength;
        // this is to check if we loaded our base register if we did we want this flag to be true in order
        // to write the directive right after
        bool firstInstBase;
        // We parse throu our object file and write to our output file in this loop
        bool wroteLTORG = false;
        while (ifs.good())
        {
            // our first char
            textChar = ifs.get();
            if (textChar == '\n')
            {
                textChar = ifs.get();
            }
            // We check that we arent at a line where its the end of the file or
            //  its a modification record if we are we write the ending of our file with our
            // program name. We close and return for security purposes
            if (textChar == 'E' || textChar == 'M')
            {
                // This is the branch for the end of our program
                char LOC_hex_string_inst[6];
                sprintf(LOC_hex_string_inst, "%X", LOC);
                string LOCsymbolAdd = LOC_hex_string_inst;
                if (LOCsymbolAdd.length() < 6)
                {
                    LOCsymbolAdd.insert(0, 6 - LOCsymbolAdd.length(), '0');
                }

                // We look for our variable in our symbol table
                // make sure its not the first variable
                unordered_map<std::string, symbol *>::const_iterator var = symTable.find(LOCsymbolAdd.c_str());
                map<int, symbol *> varList;
                int startingProgInt = stoi(progStartLoc, nullptr, 16);
                int programLength = stoi(progLength, nullptr, 16);
                int EndingAddInt = startingProgInt + programLength;
                // we populate our variablee array with the variable than need to be at the end of the text record
                // this branch is the ending of our text record
                // we add only the text records that are between the next and curretn text record ending
                if (var != symTable.end())
                {
                    varList[LOC] = (var->second);
                }
                for (auto symB : symTable)
                {
                    int symbolInteger = stoi(symB.first, nullptr, 16);
                    if (symbolInteger > LOC && symbolInteger < EndingAddInt)
                    {
                        varList[symbolInteger] = symB.second;
                    }
                }
                // we start our iterator to begin writing to our file
                std::map<int, symbol *>::iterator iter = varList.begin();
                while (iter != varList.end())
                {
                    // we start to declare our current symbol to write adn string of the symbol to write
                    // to our output file
                    symbol *currSym = iter->second;
                    string writeSymbolSt = "";
                    // write the first symbol if found
                    // here we set the symbol if there is not symbol nothing will be written since its intitialized
                    // to an empty string
                    int numberSeperation = 12;
                    char LOC_hex_string[5];
                    LOC_hex_string[4] = '\0';
                    // we convert our integer location counter to string hex representation
                    sprintf(LOC_hex_string, "%X", LOC);
                    string LOCWrite = LOC_hex_string;
                    // we pad oru location hex string
                    if (LOCWrite.length() < 4)
                    {
                        LOCWrite.insert(0, 4 - LOCWrite.length(), '0');
                    }
                    // Here we create our column white space to keep a 12 space width and we write our white space
                    // to our file
                    int whiteSpace = numberSeperation - LOCWrite.length();
                    outFile.write(LOCWrite.c_str(), LOCWrite.length());
                    // write white space
                    string columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    outFile.write(columnSpace.c_str(), columnSpace.length());
                    writeSymbolSt = (currSym->symbolName);

                    whiteSpace = numberSeperation - writeSymbolSt.length();
                    outFile.write(writeSymbolSt.c_str(), writeSymbolSt.length());
                    // write white space
                    columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    outFile.write(columnSpace.c_str(), columnSpace.length());

                    string keyWord = "RESB";
                    whiteSpace = numberSeperation - keyWord.length();
                    outFile.write(keyWord.c_str(), keyWord.length());
                    // write white space
                    columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    outFile.write(columnSpace.c_str(), columnSpace.length());
                    // we move our iterator to the next element
                    iter++;
                    int variableSize;
                    // here we calculate the amount of bytes to be reserved based on the next symbol resb byte anc current location
                    // in our first case there is another symbol to reserve byte for
                    // If we are at the end we just get the next record address and subtract by current address
                    if (iter != varList.end())
                    {
                        variableSize = iter->first - LOC;
                    }
                    else
                    {
                        variableSize = EndingAddInt - LOC;
                    }
                    // we convert our integer location counter to string hex representation
                    string varStringSize = std::to_string(variableSize);
                    outFile.write(varStringSize.c_str(), varStringSize.length());

                    whiteSpace = numberSeperation - varStringSize.length();
                    // write white space
                    columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    outFile.write(columnSpace.c_str(), columnSpace.length());
                    whiteSpace = numberSeperation;
                    // write white space
                    columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    columnSpace += "\n";
                    outFile.write(columnSpace.c_str(), columnSpace.length());

                    LOC = iter->first;

                    LOC_hex_string_inst[6];
                    sprintf(LOC_hex_string_inst, "%X", LOC);
                    string LOCsymbolAdd = LOC_hex_string_inst;
                    if (LOCsymbolAdd.length() < 6)
                    {
                        LOCsymbolAdd.insert(0, 6 - LOCsymbolAdd.length(), '0');
                    }
                }
                // we update oru current Location counter at the end of reserbing all the byte symbols to the
                // next text record in this case unecessary since we are at the end of our program
                LOC = EndingAddInt;
                // write program name
                string endFile = "                        END         " + progName + "\n";
                outFile.write(endFile.c_str(), endFile.length());
                outFile.close();
                return 0;
            }
            // This is the starting address of our text record
            string startingAdd;
            int lastRecEndAdd;
            // In this while loop we are parse through our text records in order to make the instruction objects
            // with their information like format addressing modes and types
            while (textChar != '\n')
            {
                // get first part of text record info
                // In this branch we are getting our text record information
                // we wish to gain the starting location of our text record
                if (textChar == 'T')
                {
                    countInit = 0;
                    while (countInit < 8)
                    {
                        textChar = ifs.get();
                        if (countInit < 6)
                        {
                            startingAdd += textChar;
                        }
                        countInit++;
                    }
                    lastRecEndAdd = LOC;
                    // INSERTING VARIABLES AND CHECKING IF LAST WAS PC RELATIVE AND THERE ARE VARIABLES OR THE LAST WAS JUST
                    // PC RELATIVE
                    char LOC_hex_string_inst[7];
                    LOC_hex_string_inst[6] = '\0';
                    sprintf(LOC_hex_string_inst, "%X", LOC);
                    string LOCsymbolAdd = LOC_hex_string_inst;
                    if (LOCsymbolAdd.length() < 6)
                    {
                        LOCsymbolAdd.insert(0, 6 - LOCsymbolAdd.length(), '0');
                    }
                    // We look for our variable in our symbol table
                    // make sure its not the first variable
                    // we ,ale pir arrau pf symbols that are at the start of this text record and this means its
                    // at the end of the previous text record
                    // We ad only symbols that are within the range of our current locations counter the ending of the previous
                    // text record and the start of this text record
                    unordered_map<std::string, symbol *>::const_iterator var = symTable.find(LOCsymbolAdd.c_str());
                    map<int, symbol *> varList;
                    int NextAddInt = stoi(startingAdd, nullptr, 16);
                    if (var != symTable.end())
                    {
                        varList[LOC] = (var->second);
                    }
                    for (auto symB : symTable)
                    {
                        int symbolInteger = stoi(symB.first, nullptr, 16);
                        if (symbolInteger > LOC && symbolInteger < NextAddInt)
                        {
                            varList[symbolInteger] = symB.second;
                        }
                    }
                    // We create an iterator to start writing our symbol resb bytes
                    std::map<int, symbol *>::iterator iter = varList.begin();
                    // We iterate through our whole iterator and make sure we arent at the start of the program at location 0
                    if (!varList.empty() && LOC != 0)
                    {
                        // this branch is only if the last instruction was a pc relative we need this branch
                        if (pcInstLast == true)
                        {
                            LOC += instWrite->format;
                            // we get our displacement as an integer to add the pc address
                            int dispInt = stoi(instWrite->disp, nullptr, 16);
                            // We calculate our integer with our location counter which now the starting address of this
                            // text record and our index value
                            int OperandNum = LOC + dispInt + OperandIndex;
                            // we convert our operand number that was calculated to a hex a decimal representation
                            sprintf(Operand_hex_string, "%X", OperandNum);
                            string OpWrite = Operand_hex_string;
                            // We must cutt off the top 4 th char since we only have 12 bit address
                            if (OpWrite.length() > 3)
                            {
                                OpWrite = OpWrite.substr(1);
                            }
                            string unpaddedOperand = OpWrite;
                            // We want to pad our operand with zeros to have 4 hex numbers
                            if (OpWrite.length() < 4)
                            {
                                OpWrite.insert(0, 4 - OpWrite.length(), '0');
                            }
                            // we check for the pc relative instruction if its in our literal table or symbol table
                            unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                                ("00" + OpWrite).c_str());
                            unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + OpWrite).c_str());
                            // check if we have a symbol and its not the first symbol
                            if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                            {
                                OpWrite = (sym->second)->symbolName;
                            }
                            // we check to see if we have aliteral to write as the pc instruction operand
                            else if (lit != litTable.end())
                            {
                                // we check the name to see if its empty this means it is a literal that belongs in the ltorg pool
                                if ((lit->second)->name != "")
                                {
                                    OpWrite = (lit->second)->name;
                                }
                                else
                                {
                                    OpWrite = (lit->second)->value;
                                }
                            }
                            else
                            {
                                OpWrite = unpaddedOperand;
                            }

                            // We add our addressing flag at the start of our operand string
                            string opToWrite = addressingTypeflag + OpWrite;
                            // This is the length of what we have written and write the operand value to our file
                            writenLength = opToWrite.length();
                            outFile.write(opToWrite.c_str(), opToWrite.length());
                            // set our index register if we are loding onto our index register
                            // create white space for column spacing
                            whiteSpace = 12 - writenLength;
                            columnSpace = "";
                            while (whiteSpace != 0)
                            {
                                columnSpace += " ";
                                whiteSpace -= 1;
                            }
                            outFile.write(columnSpace.c_str(), columnSpace.length());
                            // We write our object code of our instruction in our last column
                            outFile.write(((instWrite->objCode) + "\n").c_str(),
                                          (instWrite->objCode).length() + sizeof(char));
                            // We check if we had loaded our base register in this last instruction so we can write
                            // our directive line
                            if (firstInstBase == true)
                            {
                                unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                                    ("00" + BASE).c_str());
                                unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + BASE).c_str());
                                string BaseWrite;
                                if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                                {
                                    BaseWrite = (sym->second)->symbolName;
                                }
                                else if (lit != litTable.end() && (lit->second)->name != "")
                                {
                                    BaseWrite = (lit->second)->name;
                                }

                                string lineBase = "                        BASE        " + BaseWrite + "\n";
                                outFile.write(lineBase.c_str(), lineBase.length());
                                firstInstBase = false;
                            }
                            pcInstLast = false;
                        }
                        else
                        {
                        }
                        // Here we have our normal flow of execution we dont have pc relative but we have symbol to reserve bytes for
                        //  we created the iterator earlier so we use it here
                        while (iter != varList.end())
                        {
                            // we start to declare our current symbol to write adn string of the symbol to write
                            // to our output file
                            symbol *currSym = iter->second;
                            string writeSymbolSt = "";
                            // write the first symbol if found
                            // here we set the symbol if there is not symbol nothing will be written since its intitialized
                            // to an empty string

                            int numberSeperation = 12;
                            char LOC_hex_string[5];
                            LOC_hex_string[4] = '\0';
                            // we convert our integer location counter to string hex representation
                            sprintf(LOC_hex_string, "%X", LOC);
                            string LOCWrite = LOC_hex_string;
                            // we pad oru location hex string
                            if (LOCWrite.length() < 4)
                            {
                                LOCWrite.insert(0, 4 - LOCWrite.length(), '0');
                            }
                            // Here we create our column white space to keep a 12 space width and we write our white space
                            // to our file
                            int whiteSpace = numberSeperation - LOCWrite.length();
                            outFile.write(LOCWrite.c_str(), LOCWrite.length());
                            // write white space
                            string columnSpace = "";
                            while (whiteSpace != 0)
                            {
                                columnSpace += " ";
                                whiteSpace -= 1;
                            }
                            outFile.write(columnSpace.c_str(), columnSpace.length());
                            writeSymbolSt = (currSym->symbolName);

                            whiteSpace = numberSeperation - writeSymbolSt.length();
                            outFile.write(writeSymbolSt.c_str(), writeSymbolSt.length());
                            // write white space
                            columnSpace = "";
                            while (whiteSpace != 0)
                            {
                                columnSpace += " ";
                                whiteSpace -= 1;
                            }
                            outFile.write(columnSpace.c_str(), columnSpace.length());

                            string keyWord = "RESB";
                            whiteSpace = numberSeperation - keyWord.length();
                            outFile.write(keyWord.c_str(), keyWord.length());
                            // write white space
                            columnSpace = "";
                            while (whiteSpace != 0)
                            {
                                columnSpace += " ";
                                whiteSpace -= 1;
                            }
                            outFile.write(columnSpace.c_str(), columnSpace.length());
                            // We move our iterator to the next element
                            iter++;
                            int variableSize;
                            // We want to get the calculation to write the number of bytes to reserb based on the next symbol in our iterator
                            // or if its at the end we use the current location counter and the starting address of the text record currently att
                            // this branch is at the start of our text record
                            if (iter != varList.end())
                            {
                                variableSize = iter->first - LOC;
                            }
                            else
                            {
                                variableSize = stoi(startingAdd.substr(2), nullptr, 16) - LOC;
                            }
                            // we convert our integer location counter to string hex representation
                            string varStringSize = std::to_string(variableSize);
                            outFile.write(varStringSize.c_str(), varStringSize.length());

                            whiteSpace = numberSeperation - varStringSize.length();
                            // write white space
                            columnSpace = "";
                            while (whiteSpace != 0)
                            {
                                columnSpace += " ";
                                whiteSpace -= 1;
                            }
                            outFile.write(columnSpace.c_str(), columnSpace.length());
                            whiteSpace = numberSeperation;
                            // write white space
                            columnSpace = "";
                            while (whiteSpace != 0)
                            {
                                columnSpace += " ";
                                whiteSpace -= 1;
                            }
                            columnSpace += "\n";
                            outFile.write(columnSpace.c_str(), columnSpace.length());
                            // we set our location counter to the current iterator to continue iterating through our symbol iterator
                            LOC = iter->first;
                            // We need to write the current location counter so we need to convert the integer Location counter to hex and padd
                            // with zeros and at the end we set our location counter to the current location
                            LOC_hex_string_inst[6];
                            sprintf(LOC_hex_string_inst, "%X", LOC);
                            string LOCsymbolAdd = LOC_hex_string_inst;
                            if (LOCsymbolAdd.length() < 6)
                            {
                                LOCsymbolAdd.insert(0, 6 - LOCsymbolAdd.length(), '0');
                            }
                        }

                        LOC = stoi(startingAdd.substr(2), nullptr, 16);
                    }
                }
                // THIS TO WRITE THE LAST INSTRUCTION WHEN THE LAST INSTRUCTION WAS pc relative
                //  If our last text record was pc relative we the the next instruction starting address
                // so we use this flag in order to write that instruction
                // ideally we need to implement our 2 pass where our insturction is attached to a address but time constraints

                // INSERTING VARIABLES AND CHECKING IF LAST WAS PC RELATIVE AND THERE ARE VARIABLES OR THE LAST WAS JUST
                // PC RELATIVE

                if (pcInstLast)
                {
                    // we get our displacement as an integer to add the pc address
                    int dispInt = stoi(instWrite->disp, nullptr, 16);
                    // We calculate our integer with our location counter which now the starting address of this
                    // text record and our index value
                    int OperandNum = LOC + dispInt + OperandIndex;
                    // we convert our operand number that was calculated to a hex a decimal representation
                    sprintf(Operand_hex_string, "%X", OperandNum);
                    string OpWrite = Operand_hex_string;
                    // We must cutt off the top 4 th char since we only have 12 bit address
                    if (OpWrite.length() > 3)
                    {
                        OpWrite = OpWrite.substr(1);
                    }
                    string unpaddedOperand = OpWrite;
                    // We want to pad our operand with zeros to have 4 hex numbers
                    if (OpWrite.length() < 4)
                    {
                        OpWrite.insert(0, 4 - OpWrite.length(), '0');
                    }
                    // we look for the current intruction in our literal table or symbol table
                    unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                        ("00" + OpWrite).c_str());
                    unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + OpWrite).c_str());
                    // check if we have a symbol and its not the first symbol
                    if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                    {
                        OpWrite = (sym->second)->symbolName;
                    }
                    // we check to see if we have aliteral to write as the pc instruction operand
                    else if (lit != litTable.end() && (lit->second)->name != "")
                    {
                        // we check the name to see if its empty this means it is a literal that belongs in the ltorg pool
                        if ((lit->second)->name != "")
                        {
                            OpWrite = (lit->second)->name;
                        }
                        else
                        {
                            OpWrite = (lit->second)->value;
                        }
                    }
                    else
                    {
                        OpWrite = unpaddedOperand;
                    }

                    // We add our addressing flag at the start of our operand string
                    string opToWrite = addressingTypeflag + OpWrite;
                    // This is the length of what we have written and write the operand value to our file
                    writenLength = opToWrite.length();
                    outFile.write(opToWrite.c_str(), opToWrite.length());
                    // set our index register if we are loding onto our index register
                    // create white space for column spacing
                    whiteSpace = 12 - writenLength;
                    columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    outFile.write(columnSpace.c_str(), columnSpace.length());
                    // We write our object code of our instruction in our last column
                    outFile.write(((instWrite->objCode) + "\n").c_str(),
                                  (instWrite->objCode).length() + sizeof(char));
                    // We check if we had loaded our base register in this last instruction so we can write
                    // our directive line
                    if (firstInstBase == true)
                    {
                        unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                            ("00" + BASE).c_str());
                        unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + BASE).c_str());
                        string BaseWrite;
                        if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                        {
                            BaseWrite = (sym->second)->symbolName;
                        }
                        else if (lit != litTable.end() && (lit->second)->name != "")
                        {
                            BaseWrite = (lit->second)->name;
                        }
                        string lineBase = "                        BASE        " + BaseWrite + "\n";
                        outFile.write(lineBase.c_str(), lineBase.length());
                        firstInstBase = false;
                    }
                    // we set the last flag to false again
                    pcInstLast = false;
                }
                // Here we are writing our location counter to the start of our line
                // We also pad it with zeros in order
                int numberSeperation = 12;
                char LOC_hex_string[5];
                LOC_hex_string[4] = '\0';
                // We convert our integer location counter into a hex string representation
                sprintf(LOC_hex_string, "%X", LOC);
                string LOCWrite = LOC_hex_string;
                if (LOCWrite.length() < 4)
                {
                    LOCWrite.insert(0, 4 - LOCWrite.length(), '0');
                }
                // write white space

                // we check if we have a literal to skip from pushing into our queue
                // We want to check our table for this literal in order to not add it to our queue for isntructions
                unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                    ("00" + LOCWrite).c_str());
                literal *currLit;
                string writelitlSt;
                // if we found a literal we go through this iff statement in order to skipp through the chars in this
                //  object file the chars we skip are the length of our literal in bytes
                // since we are dealing with only hex numbers we divide by two since two hex numbers
                // represent our byte to add to our location counter We use the normal length to skip the characters
                if (lit != litTable.end())
                {
                    bool blankLine = false;
                    currLit = lit->second;
                    int literalLength = (stoi(currLit->length, nullptr, 10) / 2);
                    for (int i = 0; i < stoi(currLit->length, nullptr, 10); i++)
                    {
                        textChar = ifs.get();
                        if (textChar == '\n')
                        {
                            blankLine = true;
                            break;
                        }
                    }
                    if (blankLine == true)
                    {
                        blankLine = false;
                        continue;
                    }
                    LOC += literalLength;
                    continue;
                }
                char currentOperand;
                // parse instruction record
                // get first to characters to get operand out of
                // grab our first char value not in hex form yet
                char firstHexChar = (char)ifs.get();
                // We check that we still hex values left and we dont have any other new lines or vertical tabs
                if (!is_hex(firstHexChar))
                {
                    break;
                }
                // grab our second hex value for our operand and flags
                char secondHexChar = (char)ifs.get();
                // we turn our char values to hex values
                unsigned int firstHex = charToHexNum(firstHexChar);
                unsigned int secondHex = charToHexNum(secondHexChar);
                // create our instruction object with our first and second hex
                sicXe *instruction = new sicXe(firstHex, secondHex);
                // we start building our object code string in order to output to our file
                instruction->buildObjCode(firstHexChar);
                instruction->buildObjCode(secondHexChar);
                // we check that our instruction is a format 2
                if (instruction->format2Check)
                {
                    printf("THIS IS INSTRUCTION 2 \n");
                    // in this case we have our format 2 so our first and second operand will occupy 8 hex bits
                    // so it will ocupy two input chars so 2 hex numbers
                    char firstOper = ifs.get();
                    char secondOper = ifs.get();
                    // we set our format 2 operands
                    instruction->setFormat2Operands(firstOper, secondOper);
                    // build our object code string for output file using our first and second operand
                    instruction->buildObjCode(firstOper);
                    instruction->buildObjCode(secondOper);
                    // we push our instruction to our queue
                    instRecordsQueue.push(instruction);
                    // THIS IS WHERE THE INSTRUCTION IS FINALLY MADE
                    // we add to our location counter the format since this represent the bytes
                    printf("THIS IS THE INSTRUCTION CODE %s\n\n", (instruction->objCode).c_str());
                    LOC += 2;
                }
                else
                {
                    // in this logical branch we dont have a format 2 so we should have a
                    // format 3 or 4
                    // We grab our next hex represented as a char and turn it into a hex Number in order
                    // to do bit masking
                    char modeTypeBits = ifs.get();
                    unsigned int modeTypeHex = charToHexNum(modeTypeBits);
                    // Here we set our format our addressing mode and type using our bit modtypebit
                    // that represents the flags that dictate this attributes and build our object code strin
                    // for output
                    instruction->setFormatThreeFour(modeTypeHex);
                    instruction->setAddressingModes(modeTypeHex);
                    instruction->setAdrressingType(secondHex);
                    instruction->buildObjCode(modeTypeBits);
                    // Based on our format we will have a different set of bits needed for the displacemtn format 3
                    // or address format 4
                    if (instruction->format == 3)
                    {
                        // in our format 3 we only need 3 hex numbers to represent the 12 bits in disp
                        // here we are setting that up
                        char disp[4];
                        disp[4] = '\0';
                        ifs.get(disp, 4);
                        instruction->setDisplacement(disp);
                        // we build our object code string for output file and write our string
                        // we built to output file
                        instruction->objCode.append(disp);
                        // we add to our queue and we add to our location counter
                        instRecordsQueue.push(instruction);
                        // THIS IS WHERE THE INSTRUCTION IS FINALLY MADE
                        LOC += 3;
                    }
                    else
                    {
                        // In our format 4 we need 5 hex numbers since our address needs 12 bits
                        char addr[5];
                        addr[5] = '\0';
                        ifs.get(addr, 6);
                        instruction->setAddress(addr);
                        // we build our object code string for output file and write our string
                        // we built to output file we also add our address part to our object code
                        instruction->objCode.append(addr);
                        // we push onto our queue and add the format bytes  to our location counter
                        instRecordsQueue.push(instruction);
                        // THIS IS WHERE THE INSTRUCTION IS FINALLY MADE
                        LOC += 4;
                    }
                }
            }
            // done parsing our instruction record now we start writing our location counter in for to our output file
            //  we put our location counter as our starting address in order to write to our file basically
            // reinitializing it in this way we are esentially doing a two pass but not in a very optimized way
            LOC = stoi(startingAdd.substr(2), nullptr, 16);
            // get and write base register
            // We are going throguh our whole queue we made early made out of our instructions this is a FIFO queue
            // We check it is not empty
            int numberSeperation = 12;
            char LOC_hex_string[5];
            LOC_hex_string[4] = '\0';
            // we convert our integer location counter to string hex representation
            sprintf(LOC_hex_string, "%X", LOC);
            string LOCWrite = LOC_hex_string;
            // we padd oru location hex string
            if (LOCWrite.length() < 4)
            {
                LOCWrite.insert(0, 4 - LOCWrite.length(), '0');
            }
            // we look for the instruction symbol in our literal table and symbol table to write it in our output
            unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                ("00" + LOCWrite).c_str());
            unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + LOCWrite).c_str());
            // We set the last line as not bein a literal at the start of iteration
            // this is for literal writing
            bool pastLineLit = false;
            // We check to see if our instruction queue we made is not empty and that our literal table is not empty
            // If they are not there are stil instruction to write for this text record
            if (!instRecordsQueue.empty() || lit != litTable.end())
            {
                // This is the above check ensured
                while (!instRecordsQueue.empty() || lit != litTable.end())
                {
                    char LOC_hex_string[5];
                    LOC_hex_string[4] = '\0';
                    // we convert our integer location counter to string hex representation
                    sprintf(LOC_hex_string, "%X", LOC);
                    LOCWrite = LOC_hex_string;
                    if (LOCWrite.length() < 4)
                    {
                        LOCWrite.insert(0, 4 - LOCWrite.length(), '0');
                    }
                    lit = litTable.find(("00" + LOCWrite).c_str());
                    sym = symTable.find(("00" + LOCWrite).c_str());
                    // we check when we are in the middle of our iteration if we dont have any more instructions
                    // or we dont have any more literals
                    if (instRecordsQueue.empty() && lit == litTable.end())
                    {
                        break;
                    }
                    // if we are at the start of our literals meaning the past line was not a literal
                    // we nee and LTORG first before we start writing literals in our literal pool
                    if (pastLineLit == false && lit != litTable.end())
                    {
                        if ((lit->second)->name == "" && !wroteLTORG)
                        {
                            string lineLTORG = "                        LTORG        \n";
                            outFile.write(lineLTORG.c_str(), lineLTORG.length());
                            pastLineLit = true;
                            wroteLTORG = true;
                        }
                    }
                    // instruction parsing to write
                    // write instruction location
                    // we seperate for our white space
                    // Here we create our column white space to keep a 12 space width and we write our white space
                    // to our file
                    int whiteSpace = numberSeperation - LOCWrite.length();
                    outFile.write(LOCWrite.c_str(), LOCWrite.length());
                    // write white space
                    string columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    outFile.write(columnSpace.c_str(), columnSpace.length());
                    // we check if we have a literal is not our table using our location counter
                    // if we fine a locaction counter then that literal belongs at that address

                    literal *currLit;
                    string writelitlSt;
                    // this to check if we found a literal
                    if (lit != litTable.end())
                    {
                        // if we found our literal we must write our literal line to our file
                        // get literal object
                        currLit = lit->second;
                        // get literal name to wrote
                        writelitlSt = (currLit->name);
                        // write literal name to file
                        outFile.write((writelitlSt).c_str(), writelitlSt.length());
                        // write white space for column space in fiel
                        whiteSpace = 12 - writelitlSt.length();
                        columnSpace = "";
                        while (whiteSpace != 0)
                        {
                            columnSpace += " ";
                            whiteSpace -= 1;
                        }
                        outFile.write(columnSpace.c_str(), columnSpace.length());
                        // write byte directive to our file
                        writelitlSt = "BYTE";
                        if (currLit->name == "")
                        {
                            writelitlSt = "*";
                        }
                        outFile.write((writelitlSt).c_str(), writelitlSt.length());
                        // write white space for column to file for spaceing
                        whiteSpace = 12 - writelitlSt.length();
                        columnSpace = "";
                        while (whiteSpace != 0)
                        {
                            columnSpace += " ";
                            whiteSpace -= 1;
                        }
                        outFile.write(columnSpace.c_str(), columnSpace.length());
                        // We get our current literal value in this case we use our whole literal with its hex
                        //  flage at the start
                        writelitlSt = currLit->value;
                        outFile.write((writelitlSt).c_str(), writelitlSt.length());
                        // write white space for column to the file
                        whiteSpace = 12 - writelitlSt.length();
                        columnSpace = "";
                        while (whiteSpace != 0)
                        {
                            columnSpace += " ";
                            whiteSpace -= 1;
                        }
                        outFile.write(columnSpace.c_str(), columnSpace.length());
                        // here we are writing our value into the object code field of the literal without
                        // the hex flag
                        if (currLit->name == "")
                        {
                            writelitlSt = (currLit->value).substr(3, (currLit->value).length() - 4);
                        }
                        else
                        {
                            writelitlSt = (currLit->value).substr(2, (currLit->value).length() - 3);
                        }
                        outFile.write((writelitlSt).c_str(), writelitlSt.length());
                        // write white space for column for file spaceing
                        whiteSpace = 12 - writelitlSt.length();
                        columnSpace = "";
                        while (whiteSpace != 0)
                        {
                            columnSpace += " ";
                            whiteSpace -= 1;
                        }
                        outFile.write((columnSpace + "\n").c_str(), columnSpace.length());
                        // We are writing the length of our literal in bytes so we divide by 2 in
                        // this case and add to wour location counter with new line at end
                        LOC += (stoi(currLit->length, nullptr, 10) / 2);
                        outFile.write("\n", 1);
                        continue;
                    }
                    pastLineLit = false;
                    // Here we are starting to write actual instructions to our file
                    // we first check if our instructions queue is empty so we can get and pop from it
                    if (!instRecordsQueue.empty())
                    {
                        instWrite = instRecordsQueue.front();
                        instRecordsQueue.pop();
                    }
                    else
                    {
                        continue;
                    }
                    // get base register and set
                    // we set our flags as well in order to check if we have loaded our base
                    // base register
                    bool firstInstBase = false;
                    // branch to load and set our base register we check for our mnuemonicas in this case
                    if (instWrite->format != 2 && instWrite->op == "68")
                    {
                        firstInstBase = true;
                        // we ssour flag to write our directive line after and we construct and set our base
                        // register as a string
                        if (instWrite->format == 3)
                        {
                            BASE = "0" + instWrite->disp;
                        }
                        else
                        {
                            BASE = (instWrite->addr).substr(1);
                        }
                    }
                    // look for starting address in symbol table
                    // we look for our symbol to write if applicable
                    // we start by converting our location counter integer into
                    //  a hex string representation and padding it to look in our symbol table
                    // since our keys are 6 chars long
                    char LOC_hex_string_inst[7];
                    LOC_hex_string_inst[6] = '\0';
                    sprintf(LOC_hex_string_inst, "%X", LOC);
                    string LOCsymbolAdd = LOC_hex_string_inst;
                    if (LOCsymbolAdd.length() < 6)
                    {
                        LOCsymbolAdd.insert(0, 6 - LOCsymbolAdd.length(), '0');
                    }
                    // We look in our symbol table mapping with the hex string addring for our current location
                    // we start to delcare our current symbol to write adn string of the symbol to write
                    // to our output file
                    symbol *currSym;
                    string writeSymbolSt = "";
                    // write the first symbol if found
                    // we may want to write the first symbol or not
                    if (sym != symTable.end())
                    {
                        // here we set the symbol if there is not symbol nothing will be written since its intitialized
                        // to an empty string
                        currSym = sym->second;
                        writeSymbolSt = (currSym->symbolName);
                    }
                    outFile.write((writeSymbolSt).c_str(), writeSymbolSt.length());
                    // write white space for column into file for seperation
                    whiteSpace = 12 - writeSymbolSt.length();
                    columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    outFile.write(columnSpace.c_str(), columnSpace.length());

                    // write the instruction
                    // we declare our index register and our mneumonic to for the instructions
                    // to be written to our output file
                    int INDEX;
                    string instrMneum;
                    // In this case the mnsumonic of our instruction needs a flag if it is in format 4
                    if (instWrite->format != 4)
                    {
                        instrMneum = instWrite->mnemonic;
                    }
                    else
                    {
                        instrMneum = "+" + instWrite->mnemonic;
                    }
                    outFile.write(instrMneum.c_str(), instrMneum.length());
                    // We create our white space and write it to our file
                    whiteSpace = 12 - instrMneum.length();
                    columnSpace = "";
                    while (whiteSpace != 0)
                    {
                        columnSpace += " ";
                        whiteSpace -= 1;
                    }
                    outFile.write(columnSpace.c_str(), columnSpace.length());

                    // We declare both our operands here for instruction 2 formats
                    string firstinstOperand;
                    string secondOpSt;
                    int seconOpInt;
                    // length of instruction to be written along with the string
                    string stringToWrite;
                    int writenLength;
                    // we check if we have a format 2 first the operand way of
                    // writing to the output file will be different
                    if (instWrite->format == 2)
                    {
                        // each one of this format 2 instructions has a different way of expressing the
                        // operand we express it and write to file
                        if ((instWrite->mnemonic) == "CLEAR" || (instWrite->mnemonic) == "TIX")
                        {
                            firstinstOperand = RegisterTable[charToHexNum(instWrite->operand1)];
                            writenLength = firstinstOperand.length();
                            outFile.write(firstinstOperand.c_str(), firstinstOperand.length());
                        }
                        else if ((instWrite->mnemonic) == "SHIFTR" || (instWrite->mnemonic) == "SHIFTL")
                        {
                            firstinstOperand = RegisterTable[charToHexNum(instWrite->operand1)];
                            seconOpInt = charToHexNum(instWrite->operand2);
                            stringToWrite = firstinstOperand + "," + to_string(seconOpInt);
                            writenLength = stringToWrite.length();
                            outFile.write((stringToWrite).c_str(), stringToWrite.length());
                        }
                        else
                        {
                            // this is for instructions that use r1 and r2 as operands
                            firstinstOperand = RegisterTable[charToHexNum(instWrite->operand1)];
                            secondOpSt = RegisterTable[charToHexNum(instWrite->operand2)];
                            stringToWrite = firstinstOperand + "," + secondOpSt;
                            writenLength = stringToWrite.length();
                            outFile.write((stringToWrite).c_str(), stringToWrite.length());
                        }
                        // we create and write white space for column spaceing in outpute file
                        whiteSpace = 12 - writenLength;
                        columnSpace = "";
                        while (whiteSpace != 0)
                        {
                            columnSpace += " ";
                            whiteSpace -= 1;
                        }
                        outFile.write(columnSpace.c_str(), columnSpace.length());
                        // we write our object code for our last column in format 2
                        outFile.write(((instWrite->objCode) + "\n").c_str(),
                                      (instWrite->objCode).length() + sizeof(char));
                        // we check that we hadnt written a base register which would most likeley be impossible since
                        // it is a format 2
                        if (firstInstBase == true)
                        {
                            unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                                ("00" + BASE).c_str());
                            unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + BASE).c_str());
                            string BaseWrite;
                            if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                            {
                                BaseWrite = (sym->second)->symbolName;
                            }
                            else if (lit != litTable.end() && (lit->second)->name != "")
                            {
                                BaseWrite = (lit->second)->name;
                            }
                            string lineBase = "                        BASE        " + BaseWrite + "\n";
                            outFile.write(lineBase.c_str(), lineBase.length());
                            firstInstBase = false;
                        }
                        // we increase our location counter by our format bytes of current instruction
                        LOC += 2;
                    }
                    else
                    {
                        // in this case we have a format 3 or 4 instruction and we write the operand and object code
                        // we need our addressing flag for operand
                        // we delcare the outpute variables and addressing type
                        addressingTypeflag = "";
                        int OperandIndex = 0;
                        string addressingType = instWrite->addressingType;
                        string FinalOperand;
                        int writenLength;
                        // in this block we check our addressing type for our addressing type flag
                        if (addressingType == "indirect")
                        {
                            addressingTypeflag = "@";
                        }
                        else if (addressingType == "immediate")
                        {
                            addressingTypeflag = "#";
                        }
                        // we get our addressing mode first from the instruction
                        string addMode = instWrite->addressingMode;
                        // we check if its index to set the operand index so we can add it to the
                        // operand
                        // seeting up variables to extract the operanc string
                        string Operandstring;
                        char Operand_hex_string[5];
                        Operand_hex_string[4] = '\0';
                        // setting variable for string to write to file
                        string opToWrite;
                        // We check if we have a format 4 or 3 as writign will be different for both
                        if (instWrite->format == 4)
                        {
                            string addrMode = (instWrite->addressingMode);
                            // We first get first for four hex string chars of our address since we can only have 4 hex
                            // number and turn it into a integer we add our index register in case we ahve index value
                            int OperandVal = stoi((instWrite->addr).substr(1), nullptr, 16) + OperandIndex;
                            sprintf(Operand_hex_string, "%X", OperandVal);
                            string OpWrite = Operand_hex_string;
                            string unpaddedOperand = OpWrite;
                            // we padd our hex string to be written
                            if (OpWrite.length() < 4)
                            {
                                OpWrite.insert(0, 4 - OpWrite.length(), '0');
                            }
                            // we look for the label in our symbol and literal table

                            unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                                ("00" + OpWrite).c_str());
                            unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + OpWrite).c_str());
                            // check if we have a symbol and its not the first symbol
                            if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                            {
                                OpWrite = (sym->second)->symbolName;
                            }
                            // we check to see if we have aliteral to write
                            else if (lit != litTable.end())
                            {
                                // we check the name to see if its empty this means it is a literal that belongs in the ltorg pool
                                if ((lit->second)->name != "")
                                {
                                    OpWrite = (lit->second)->name;
                                }
                                else
                                {
                                    OpWrite = (lit->second)->value;
                                }
                            }
                            else
                            {
                                OpWrite = unpaddedOperand;
                            }
                            if (addMode == "direct_indexed")
                            {
                                opToWrite = addressingTypeflag + OpWrite + ",X";
                            }
                            else
                            {
                                opToWrite = addressingTypeflag + OpWrite;
                            }
                            writenLength = opToWrite.length();
                            outFile.write(opToWrite.c_str(), opToWrite.length());
                            // We add location counter by 4 bytes
                            LOC += 4;
                        }
                        else if (instWrite->format == 3)
                        {
                            // In this case we have an instruction 4 format and we get our addressing mode
                            string addrMode = (instWrite->addressingMode);
                            // we first check if we have a pc relative mode
                            if (addrMode == "pc_indexed" || addrMode == "pc")
                            {
                                // we need as a flag of when we are in pc relative mode in the current instruction
                                //  but we are at the end of our text record, and we need the next instruction
                                // address that is at the next starting location text record
                                // we set this flag and continue to net iteration in order to get that address
                                if (instRecordsQueue.empty())
                                {
                                    pcInstLast = true;
                                    continue;
                                }
                                // we get our diplacement for our operand calculations by converting our string
                                // into an integer
                                int dispInt = stoi(instWrite->disp, nullptr, 16);
                                // we calculate using our operand in this case we know we arent at the end
                                // of our text record so we can just add the loc by the format of this instruction and
                                //  add our displacement and index to get our operand integer
                                int OperandNum = LOC + ((instWrite->format)) + dispInt + OperandIndex;
                                sprintf(Operand_hex_string, "%X", OperandNum);
                                string OpWrite = Operand_hex_string;
                                // we cut out the first char if we need to since we only have 12 bit addresses
                                // regardless of our calculation
                                if (OpWrite.length() > 3)
                                {
                                    OpWrite = OpWrite.substr(1);
                                }
                                string unpaddedOperand = OpWrite;
                                // we pad the string
                                if (OpWrite.length() < 4)
                                {
                                    OpWrite.insert(0, 4 - OpWrite.length(), '0');
                                }
                                // we add our flag to our stiring then we write to our otput file
                                // we check for the pc relative instruction if its in our literal table or symbol table
                                unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                                    ("00" + OpWrite).c_str());
                                unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + OpWrite).c_str());
                                // check if we have a symbol and its not the first symbol
                                if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                                {
                                    OpWrite = (sym->second)->symbolName;
                                }
                                // we check to see if we have aliteral to write as the pc instruction operand
                                else if (lit != litTable.end())
                                {
                                    // we check the name to see if its empty this means it is a literal that belongs in the ltorg pool
                                    if ((lit->second)->name != "")
                                    {
                                        OpWrite = (lit->second)->name;
                                    }
                                    else
                                    {
                                        OpWrite = (lit->second)->value;
                                    }
                                }
                                else
                                {
                                    OpWrite = unpaddedOperand;
                                }
                                string opToWrite;
                                // We check to see if we are pc indexed in order to write the needed flags for the operand for pc index
                                if (addMode == "pc_indexed")
                                {
                                    opToWrite = addressingTypeflag + OpWrite + ",X";
                                }
                                else
                                {
                                    opToWrite = addressingTypeflag + OpWrite;
                                }
                                writenLength = opToWrite.length();
                                outFile.write(opToWrite.c_str(), opToWrite.length());
                            }
                            else if (addrMode == "absolute")
                            {
                                // in this case we have an absolute in which we use our displacement
                                // as operanc adn we write it into our output file
                                string operandWrit = "0" + (instWrite->objCode).substr(3);
                                unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                                    ("00" + operandWrit).c_str());
                                unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + operandWrit).c_str());
                                if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                                {
                                    operandWrit = (sym->second)->symbolName;
                                }
                                else if (lit != litTable.end())
                                {
                                    if ((lit->second)->name != "")
                                    {
                                        operandWrit = (lit->second)->name;
                                    }
                                    else
                                    {
                                        operandWrit = (lit->second)->value;
                                    }
                                }
                                else
                                {
                                    int num = stoi(operandWrit);
                                    operandWrit = to_string(num);
                                }

                                string opToWrite = addressingTypeflag + operandWrit;
                                writenLength = opToWrite.length();
                                outFile.write(opToWrite.c_str(), opToWrite.length());
                            }
                            else
                            {
                                // here we would have base relative addressing and calculate our operand
                                // using our base register
                                int dispInt = stoi(instWrite->disp, nullptr, 16);
                                // we turn our base register string hex into a integer to add
                                int baseRegNum = stoi(BASE, nullptr, 16);
                                // we add oru displacement our index register adn base register index only not 0
                                // if indexed
                                int OperandNum = baseRegNum + dispInt + OperandIndex;
                                // convert to string to write to output file
                                sprintf(Operand_hex_string, "%X", OperandNum);
                                string OpWrite = Operand_hex_string;
                                string unpaddedOperand = OpWrite;
                                // padding with zeros on left
                                if (OpWrite.length() < 4)
                                {
                                    OpWrite.insert(0, 4 - OpWrite.length(), '0');
                                }
                                // we check for the pc relative instruction if its in our literal table or symbol table
                                unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                                    ("00" + OpWrite).c_str());
                                unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + OpWrite).c_str());
                                // check if we have a symbol and its not the first symbol
                                if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                                {
                                    OpWrite = (sym->second)->symbolName;
                                }
                                // we check to see if we have aliteral to write as the pc instruction operand
                                else if (lit != litTable.end())
                                {
                                    // we check the name to see if its empty this means it is a literal that belongs in the ltorg pool
                                    if ((lit->second)->name != "")
                                    {
                                        OpWrite = (lit->second)->name;
                                    }
                                    else
                                    {
                                        OpWrite = (lit->second)->value;
                                    }
                                }
                                else
                                {
                                    OpWrite = unpaddedOperand;
                                }

                                // we add our addressing type flag if it has
                                string opToWrite;
                                // we check to see if its based indexed in order to add appropriate flags to operand for base index
                                if (addMode == "base_indexed")
                                {
                                    opToWrite = addressingTypeflag + OpWrite + ",X";
                                }
                                else
                                {
                                    opToWrite = addressingTypeflag + OpWrite;
                                }
                                writenLength = opToWrite.length();
                                // write our string full operand
                                outFile.write(opToWrite.c_str(), opToWrite.length());
                            }
                            // we increase our location counter by format 3 so 3 bytes
                            LOC += 3;
                        }
                        // we check that we didnt load our index register earlier
                        // this is column spacing for output file
                        whiteSpace = 12 - writenLength;
                        columnSpace = "";
                        while (whiteSpace != 0)
                        {
                            columnSpace += " ";
                            whiteSpace -= 1;
                        }
                        outFile.write(columnSpace.c_str(), columnSpace.length());
                        // We write our object code to our output file in this format 3 branch
                        outFile.write(((instWrite->objCode) + "\n").c_str(),
                                      (instWrite->objCode).length() + sizeof(char));
                        // we check we didnt just load the base register so we can write our base register loading directive
                        if (firstInstBase == true)
                        {

                            unordered_map<std::string, literal *>::const_iterator lit = litTable.find(
                                ("00" + BASE).c_str());
                            unordered_map<std::string, symbol *>::const_iterator sym = symTable.find(("00" + BASE).c_str());
                            string BaseWrite;
                            if (sym != symTable.end() && (sym->second)->symbolName != "FIRST")
                            {
                                BaseWrite = (sym->second)->symbolName;
                            }
                            else if (lit != litTable.end() && (lit->second)->name != "")
                            {
                                BaseWrite = (lit->second)->name;
                            }
                            string lineBase = "                        BASE        " + BaseWrite + "\n";
                            outFile.write(lineBase.c_str(), lineBase.length());
                            firstInstBase = false;
                        }
                    }
                }
            }
            // proceses record for file writing
        }
    }
    // closing both our files
    ifs.close();
    return 0;
}
