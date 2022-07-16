

void pass1(string filename)
{
    ifstream assembly_code;
    ofstream intermediateFile;
    ofstream errorFile;
    bool status;


    assembly_code.open("tests\\"+ filename +".txt");
    if(!assembly_code)
    {
        cout<<"Unable to open file: "<<filename<<endl;
        exit(1);
    }

    intermediateFile.open("tests\\"+ filename + "_intermediate.txt");
    if(!intermediateFile)
    {
        cout<<"Unable to open file: "<<filename<<"_intermediate"<<endl;
        exit(1);

    }
    writeToFile(intermediateFile,"Line\t LOC \tLabel\tOPCODE\tOPERAND\t");


    errorFile.open("tests\\"+ filename +"_error.txt");
    if(!errorFile)
    {
        cout<<"Unable to open file: error"<<filename<<endl;
        exit(1);

    }
    writeToFile(errorFile,"************PASS1************");


    string label="",opcode="",operand="",comment="",fileLine,writeData;

    int startAddress,LOCCTR,lineNumber,lastDeltaLOCCTR,lineNumberDelta=0,index=0;

    string tempOperand="";
    lineNumber = 0;
    lastDeltaLOCCTR = 0;


    getline(assembly_code,fileLine);
    lineNumber += 5;


    while(isComment(fileLine))
    {
        writeData = to_string(lineNumber) + "\t" + fileLine;

        writeToFile(intermediateFile,writeData);

        getline(assembly_code,fileLine);
        lineNumber += 5;
        index = 0;
    }

    read_word(fileLine,index,status,label);//label
    read_word(fileLine,index,status,opcode);//opcode


    if(opcode=="START")
    {
        read_word(fileLine,index,status,operand);//operand
        read_word(fileLine,index,status,comment,true);//complete whole line ...anyways will be ignored
        startAddress = stoi(operand,nullptr,16);

        LOCCTR = startAddress;//initialise the location counter

        writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
        writeToFile(intermediateFile,writeData);



        getline(assembly_code,fileLine);
        lineNumber += 5;
        index = 0;

        read_word(fileLine,index,status,label);
        read_word(fileLine,index,status,opcode);

    }
    else //if not START just start from0
    {
        startAddress = 0;
        LOCCTR = 0;

    }


    while(opcode!="END")
    {

        if(!isComment(fileLine))
        {
            if(label!="") //Label found
            {
                if(SYMTAB[label].exists=='n')
                {

                    SYMTAB[label].name = label;
                    SYMTAB[label].address = intToStringHex(LOCCTR);
                    SYMTAB[label].relative = 1;
                    SYMTAB[label].exists = 'y';

                }
                else
                {
                    // label already existed in the SYMTAB
                    writeData = "Line: "+to_string(lineNumber)+" : Duplicate symbol for '"+label+"'. Previously declared at "+SYMTAB[label].address;
                    writeToFile(errorFile,writeData);
                    error_flag = true;

                }
            }


            if(OPTAB[real_Opcode(opcode)].exists=='y') //if opcode exist in OPTAB
            {

                if(OPTAB[real_Opcode(opcode)].format==1)     //format 1
                {
                    operand = " ";
                    LOCCTR += 1;
                    lastDeltaLOCCTR += 1;
                }
                else if(OPTAB[real_Opcode(opcode)].format==2)   //format 2
                {
                    LOCCTR += 2;
                    lastDeltaLOCCTR += 2;
                    read_word(fileLine,index,status,operand);

                    if(operand[operand.length()-1] == ',')
                    {
                        read_word(fileLine,index,status,tempOperand);
                        operand += tempOperand;

                    }
                }
                else if(OPTAB[real_Opcode(opcode)].format==3)//if format 3 or format 4
                {

                    if(opcode_operand_Format(opcode)=='+')
                    {
                        LOCCTR += 4;
                        lastDeltaLOCCTR += 4;

                    }
                    else
                    {
                        LOCCTR += 3;
                        lastDeltaLOCCTR += 3;

                    }
                    if(real_Opcode(opcode)=="RSUB")
                    {
                        operand = " ";

                    }
                    else
                    {
                        read_word(fileLine,index,status,operand);

                        if(operand[operand.length()-1] == ',')// for indexed addressing
                        {
                            read_word(fileLine,index,status,tempOperand);
                            operand += tempOperand;

                        }
                    }

                }
            }

            else if(opcode == "WORD")
            {
                read_word(fileLine,index,status,operand);
                LOCCTR += 3;
                lastDeltaLOCCTR += 3;

            }
            else if(opcode == "RESW")
            {
                read_word(fileLine,index,status,operand);
                LOCCTR += 3*stoi(operand);
                lastDeltaLOCCTR += 3*stoi(operand);

            }
            else if(opcode == "RESB")
            {

                read_word(fileLine,index,status,operand);
                LOCCTR += stoi(operand);
                lastDeltaLOCCTR += stoi(operand);

            }
            else if(opcode == "BYTE")
            {
                read_Byte_Operand(fileLine,index,status,operand);

                if(operand[0]=='X')
                {
                    LOCCTR += (operand.length() -3)/2;
                    lastDeltaLOCCTR += (operand.length() -3)/2;

                }
                else if(operand[0]=='C')
                {
                    LOCCTR += operand.length() -3;
                    lastDeltaLOCCTR += operand.length() -3;

                }

            }
            else if(opcode=="BASE")
            {
                read_word(fileLine,index,status,operand);

            }

            //  if equ is what we want
            else if(opcode=="EQU")
            {
                read_word(fileLine,index,status,operand);
                tempOperand = "";
                bool relative;

                if(operand=="*")
                {
                    tempOperand = intToStringHex(LOCCTR-lastDeltaLOCCTR);
                    relative = 1;

                }
                else if(is_Number(operand))
                {
                    tempOperand = intToStringHex(stoi(operand));
                    relative = 0;

                }
                else
                {
                    char lastByte = operand[operand.length()-1];

                    while(lastByte=='+'||lastByte=='-'||lastByte=='/'||lastByte=='*')
                    {
                        read_word(fileLine,index,status,tempOperand);
                        operand += tempOperand;
                        lastByte = operand[operand.length()-1];

                    }
                    string operand1,operand2;

                    read_word(fileLine,index,status,tempOperand);
                    operand += tempOperand;
                    tempOperand="";

                    if(operand.find('-')!=-1)
                    {
                        int position=operand.find('-');
                        operand1=operand.substr(0,position);
                        operand2=operand.substr(position+1,operand.length()-1);

                        if(SYMTAB[operand1].exists=='y' && SYMTAB[operand2].exists=='y')
                        {
                            tempOperand=intToStringHex(stringHexToInt(SYMTAB[operand1].address)-stringHexToInt(SYMTAB[operand2].address));
                            relative=0;
                        }
                        else
                        {
                            writeData = "Line: "+to_string(lineNumber)+"  :Symbol not found in SYMTAB.";
                            writeToFile(errorFile,writeData);
                        }
                    }

                    if(operand.find('+')!=-1)
                    {
                        int position=operand.find('+');
                        operand1=operand.substr(0,position);
                        operand2=operand.substr(position+1,operand.length()-1);

                        if(SYMTAB[operand1].exists=='y' && SYMTAB[operand2].exists=='y')
                        {
                            tempOperand=intToStringHex(stringHexToInt(SYMTAB[operand1].address)+stringHexToInt(SYMTAB[operand2].address));
                            relative=0;
                        }
                        else
                        {
                            writeData = "Line: "+to_string(lineNumber)+"  :Symbol not found in SYMTAB.";
                            writeToFile(errorFile,writeData);
                        }
                    }

                    if(operand.find('/')!=-1)
                    {
                        int position=operand.find('/');
                        operand1=operand.substr(0,position);
                        operand2=operand.substr(position+1,operand.length()-1);

                        if(SYMTAB[operand1].exists=='y' && SYMTAB[operand2].exists=='y')
                        {
                            tempOperand=intToStringHex(stringHexToInt(SYMTAB[operand1].address)/stringHexToInt(SYMTAB[operand2].address));
                            relative=0;
                        }
                        else
                        {
                            writeData = "Line: "+to_string(lineNumber)+"  :Symbol not found in SYMTAB.";
                            writeToFile(errorFile,writeData);
                        }
                    }

                }

                SYMTAB[label].name = label;
                SYMTAB[label].address = tempOperand;
                SYMTAB[label].relative = relative;                          //                 why this relative no idea
                lastDeltaLOCCTR = LOCCTR - stringHexToInt(tempOperand);
            }

            else
            {
                read_word(fileLine,index,status,operand);
                writeData = "Line: "+to_string(lineNumber)+" : Invalid OPCODE.   " + opcode +"Not found in OPTAB";

                writeToFile(errorFile,writeData);
                error_flag = true;

            }
            read_word(fileLine,index,status,comment,true);


            if(opcode=="EQU" && SYMTAB[label].relative == 0)
            {
                writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment ;

            }

            else
            {
                writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment ;

            }

        }

        else//if it is a comment
        {
            writeData = to_string(lineNumber) + "\t" + fileLine;

        }
        writeToFile(intermediateFile,writeData);


        getline(assembly_code,fileLine);

        lineNumber += 5 + lineNumberDelta;
        lineNumberDelta = 0;
        index = 0;
        lastDeltaLOCCTR = 0;

        read_word(fileLine,index,status,label);
        read_word(fileLine,index,status,opcode);


    }

    if(opcode=="END")
    {
        first_Object_Code_LOC=intToStringHex(startAddress,6);

        program_length=LOCCTR-startAddress;

    }

    read_word(fileLine,index,status,operand);
    read_word(fileLine,index,status,comment,true);


    writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment ;
    writeToFile(intermediateFile,writeData);



    assembly_code.close();
    intermediateFile.close();
    errorFile.close();

}
