#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <algorithm>
#include <bits/stdc++.h>
#include <string>

using namespace std;


struct struct_opcode
{
    string opcode;
    int format;
    char exists;

    struct_opcode()
    {
        opcode="undefined";
        format=0;
        exists='n';
    }
};

struct struct_label
{
    string address;
    string name;
    int relative;
    int blockNumber;
    char exists;

    struct_label()
    {
        name="undefined";
        address="0";
        blockNumber = 0;
        exists='n';
        relative = 0;
    }
};

struct struct_register
{
    char num;
    char exists;

    struct_register()
    {
        num = 'F';
        exists='n';
    }
};

typedef map<string,struct_label> SYMBOL_TABLE_TYPE;
typedef map<string,struct_opcode> OPCODE_TABLE_TYPE;
typedef map<string,struct_register> REG_TABLE_TYPE;

SYMBOL_TABLE_TYPE SYMTAB;
OPCODE_TABLE_TYPE OPTAB;
REG_TABLE_TYPE REGTAB;

//**************************************pass1***********************************
bool error_flag=false;
int program_length;

string first_Object_Code_LOC;

void evaluateExpression(string expression, bool& relative,string& tempOperand,int lineNumber, ofstream& errorFile,bool& error_flag);
void pass1(string filename);

//**************************************pass2***********************************
ifstream intermediateFile;
ofstream errorFile,objectFile,intermediate_File2,Symtab ;

string writestring ;

bool Is_Comment;
string label,opcode,operand,comment,operand1,operand2;

int lineNumber,address,startAddress;
string objectCode, writeData, currentRecord, M_Record="M^", E_Record;

int PC, Base_reg, currentTextRecordLength;
bool nobase;

string readTillTab(string data,int& index);
bool read_IntermediateFile(ifstream& readFile,bool& Is_Comment, int& lineNumber, int& address, string& label, string& opcode, string& operand, string& comment);
string ObjectCode_F3_F4();
void write_T_Record(bool is_lastRecord=false);
void write_E_Record(bool write=true);
void pass2(string fileName);



