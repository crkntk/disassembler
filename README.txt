In this project a dissasembler is built that translates machine languate into assembly language.
The language in question is SIC/XE instruction set architecture.
It opens a object code file then parses the records int he object code file and outputes the 
corresponding assembly code to an assembly listing file.
Replaces the target address in the operand column with the corresponding labels, inserts RESB instructions to fill the gap between addresses in the LOC column, and inserts
 the literal definition and potentially LTORG statements.


Input Files:
    Object Code File: test.obj
    Object code file would have the typical SIC/XE object code structure that starts with a HEADER record, 
    followed by several TEXT records, followed by (possibly) several MODIFICATION records, and ends with an END record. Note your
    disassembling would only need to parse the TEXT records.
    Symbol Table File: test.sym:
    symbols, variables, constants and literals used in the assembly code.
    These two input file names are passed as command line arguments to the disassembler executable.

Output Files:
    Assembly Listing File: out.lst
Format for symbol table:
    The symbol table file (test.sym) contains SYMTAB and LITTAB tables generated from the assembling process, and they are needed for the disassembling process. 
    The first table in the symbol file is the SYMTAB and the second table is the LITTAB.
    It has symbol address and flag columns.
Literal table has 
    name literal constant length and address columns

Instruction formats supoorted are: 2 3 and 4

Supports byte variables for now and byte constants in the form of hexadeicmal for now int the future
will support other constants

Supports literal pools

supports the 59 set of intructions with its mneumonics given int the sicXE class

To assemble the porgrams a make file is provided. It takes two arguments in this formats
    ./disassem test.obj test.sym

The test.obj file holds the object code and the test.sym file holds the symbol and literal tables
Some initial tests are provided in the test folder along with their tables.

NOTE: This project is for learnign purposes only and is not intended to be used in any other functionality.
        This is a project from cs 530 at san diego state university addition to this project are from the book for this class
        In the future support for other formats constants and literals is ideal. 

