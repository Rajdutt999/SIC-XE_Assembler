string ObjectCode_F3_F4()
{
    string objcode;
    int format_3or4_addressfield;
    int xbpe=0;
    //xbpe=0 0000       xbpe=1  0001       xbpe=2  0010       xbpe=3  0011       xbpe=4 0100
    //xbpe=5 0101       xbpe=6  0110       xbpe=7  0111       xbpe=8  1000

    format_3or4_addressfield = (opcode_operand_Format(opcode)=='+')?5:3;


    if(opcode_operand_Format(operand)=='#') //Immediate
    {
        if(operand.substr(operand.length()-2,2)==",X")
        {
            writeData = "Line: "+to_string(lineNumber)+" Index based addressing cannot be used with Indirect addressing";
            writeToFile(errorFile,writeData);
            objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+1,2);//ni=1
            objcode += (format_3or4_addressfield==5)?"100000":"0000";

            return objcode;

        }

        string tempOperand = operand.substr(1,operand.length()-1);
        if(is_Number(tempOperand)||((SYMTAB[tempOperand].exists=='y')&&(SYMTAB[tempOperand].relative==0)))
        {
            int immediate_operand;

            if(is_Number(tempOperand))
            {
                immediate_operand = stoi(tempOperand);

            }
            else
            {
                immediate_operand = stringHexToInt(SYMTAB[tempOperand].address);

            }

            if(immediate_operand>=(1<<4*format_3or4_addressfield))
            {
                writeData = "Line: "+to_string(lineNumber)+" Immediate value is too big";
                writeToFile(errorFile,writeData);
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+1,2);
                objcode += (format_3or4_addressfield==5)?"100000":"0000";
            }
            else
            {
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+1,2);
                objcode += (format_3or4_addressfield==5)?'1':'0';
                objcode += intToStringHex(immediate_operand,format_3or4_addressfield);
            }

            return objcode;
        }
        else if(SYMTAB[tempOperand].exists=='n')
        {

                writeData = "Line "+to_string(lineNumber);
                    writeData += "\t"+ tempOperand + " : Symbol doesn't exists in SYMTAB";

                writeToFile(errorFile,writeData);
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+1,2);
                objcode += (format_3or4_addressfield==5)?"100000":"0000";

                return objcode;

        }
        else
        {
            int operandAddress = stringHexToInt(SYMTAB[tempOperand].address);

            if(format_3or4_addressfield==5) //If format 4
            {
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+1,2);
                objcode += '1';
                objcode += intToStringHex(operandAddress,format_3or4_addressfield);


                M_Record += "M^" + intToStringHex(address+1,6) + '^';
                M_Record += (format_3or4_addressfield==5)?"05":"03";
                M_Record += '\n';

                return objcode;
            }

            PC = address ;
            PC += (format_3or4_addressfield==5)?4:3;
            int Displacement = operandAddress - PC;

            //PC relative
            if(Displacement>=(-2048) && Displacement<=2047)
            {
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+1,2);
                objcode += '2';
                objcode += intToStringHex(Displacement,format_3or4_addressfield);
                return objcode;
            }

            //base relative
            if(!nobase)
            {
                Displacement = operandAddress - Base_reg;
                if(Displacement>=0 && Displacement<=4095)
                {
                    objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+1,2);
                    objcode += '4';
                    objcode += intToStringHex(Displacement,format_3or4_addressfield);
                    return objcode;
                }
            }

            if(operandAddress<=4095)
            {
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+1,2);
                objcode += '0';
                objcode += intToStringHex(operandAddress,format_3or4_addressfield);

                //modification
                M_Record += "M^" + intToStringHex(address+1,6) + '^';
                M_Record += (format_3or4_addressfield==5)?"05":"03";
                M_Record += '\n';

                return objcode;
            }
        }
    }
    else if(opcode_operand_Format(operand)=='@')
    {
        string tempOperand = operand.substr(1,operand.length()-1);

        if(tempOperand.substr(tempOperand.length()-2,2)==",X" || SYMTAB[tempOperand].exists=='n' )
        {

                writeData = "Line "+to_string(lineNumber);

                if(SYMTAB[tempOperand].exists=='n')
                    writeData += "\t"+ tempOperand + " : Symbol doesn't exists in SYMTAB";
                else
                    writeData += " : Index based addressing cannot be used with Indirect addressing ";


                writeToFile(errorFile,writeData);
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+2,2);
                objcode += (format_3or4_addressfield==5)?"100000":"0000";

                return objcode;


        }
        int operandAddress = stringHexToInt(SYMTAB[tempOperand].address);

        PC = address ;
        PC += (format_3or4_addressfield==5)?4:3;

        if( format_3or4_addressfield==3 )
        {
            int Displacement = operandAddress - PC;
            //PC relative
            if(Displacement>=(-2048) && Displacement<=2047)
            {
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+2,2);
                objcode += '2';
                objcode += intToStringHex(Displacement,format_3or4_addressfield);
                return objcode;
            }
            //base relative
            if(!nobase)
            {
                Displacement = operandAddress - Base_reg;
                if(Displacement>=0 && Displacement<=4095)
                {
                    objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+2,2);
                    objcode += '4';
                    objcode += intToStringHex(Displacement,format_3or4_addressfield);

                    return objcode;
                }
            }

            if(operandAddress<=4095)
            {
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+2,2);
                objcode += '0';
                objcode += intToStringHex(operandAddress,format_3or4_addressfield);

                //modification
                M_Record += "M^" + intToStringHex(address+1,6) + '^';
                M_Record += (format_3or4_addressfield==5)?"05":"03";
                M_Record += '\n';

                return objcode;
            }
        }
        else //Format 4
        {
            objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+2,2);
            objcode += '1';
            objcode += intToStringHex(operandAddress,format_3or4_addressfield);

            //modification
            M_Record += "M^" + intToStringHex(address+1,6) + '^';
            M_Record += (format_3or4_addressfield==5)?"05":"03";
            M_Record += '\n';

            return objcode;
        }

        writeData = "Line: "+to_string(lineNumber);
        writeData += " cannot use either format 3 or 4 ";
        writeToFile(errorFile,writeData);
        objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+2,2);
        objcode += (format_3or4_addressfield==5)?"100000":"0000";

        return objcode;
    }
    else // direct
    {

        string tempOperand = operand;
        if(operand.substr(operand.length()-2,2)==",X")
        {
            tempOperand = operand.substr(0,operand.length()-2);//BASE address for indexed addressing
            xbpe = 8;
        }


        if(SYMTAB[tempOperand].exists=='n')
        {

            writeData = "Line "+to_string(lineNumber);
                writeData += "\t" + tempOperand +" : Symbol doesn't exists in SYMTAB.";


            writeToFile(errorFile,writeData);
            objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+3,2);
            objcode += (format_3or4_addressfield==5)?(intToStringHex(xbpe+1,1)+"00"):intToStringHex(xbpe,1);
            objcode += "000";

            return objcode;


        }
        else
        {

            int operandAddress = stringHexToInt(SYMTAB[tempOperand].address);
            PC = address ;

            PC += (format_3or4_addressfield==5)?4:3;

            if(format_3or4_addressfield==3)
            {
                int Displacement = operandAddress - PC;

                if( Displacement>=(-2048) && Displacement <= 2047)
                {
                    objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+3,2);
                    objcode += intToStringHex(xbpe+2,1);
                    objcode += intToStringHex(Displacement,format_3or4_addressfield);

                    return objcode;

                }

                if(!nobase)
                {
                    Displacement = operandAddress - Base_reg;

                    if(Displacement>=0 && Displacement<=4095)
                    {
                        objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+3,2);
                        objcode += intToStringHex(xbpe+4,1);
                        objcode += intToStringHex(Displacement,format_3or4_addressfield);

                        return objcode;

                    }
                }

                if(operandAddress<=4095)
                {
                    objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+3,2);
                    objcode += intToStringHex(xbpe,1);
                    objcode += intToStringHex(operandAddress,format_3or4_addressfield);

                    //modifacation
                    M_Record += "M^" + intToStringHex(address+1,6) + '^';
                    M_Record += (format_3or4_addressfield==5)?"05":"03";
                    M_Record += '\n';

                    return objcode;
                }
            }
            else//Format 4
            {
                objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+3,2);
                objcode += intToStringHex(xbpe+1,1);
                objcode += intToStringHex(operandAddress,format_3or4_addressfield);

                //modification
                M_Record += "M^" + intToStringHex(address+1,6) + '^';
                M_Record += (format_3or4_addressfield==5)?"05":"03";
                M_Record += '\n';

                return objcode;
            }

            //     not format 3 or 4
            writeData = "Line: "+to_string(lineNumber);
            writeData += " cannot use either format 3 or 4";
            writeToFile(errorFile,writeData);
            objcode = intToStringHex(stringHexToInt(OPTAB[real_Opcode(opcode)].opcode)+3,2);
            objcode += (format_3or4_addressfield==5)?(intToStringHex(xbpe+1,1)+"00"):intToStringHex(xbpe,1);
            objcode += "000";

            return objcode;
        }
    }
}

void pass2(string fileName)
{

    string tempBuffer;

    intermediateFile.open("tests\\"+ fileName+"_intermediate.txt");
    if(!intermediateFile)
    {
        cout<<"Unable to open file: intermediate_"<<fileName<<endl;
        exit(1);
    }

    getline(intermediateFile, tempBuffer);


    objectFile.open("tests\\"+ fileName +"_object.txt");
    if(!objectFile)
    {
        cout<<"Unable to open file: _object"<<fileName<<endl;
        exit(1);
    }


    intermediate_File2.open("tests\\"+ fileName +"_intermediate_2.txt");
    if(!intermediate_File2)
    {
        cout<<"Unable to open file: _intermediate_2"<<fileName<<endl;
        exit(1);
    }
    writeToFile(intermediate_File2,"Line\t LOC\tLabel\tOPCODE\tOPERAND\tObjectCode\t");


    errorFile.open("tests\\"+ fileName +"_error.txt",fstream::app);
    if(!errorFile)
    {
        cout<<"Unable to open file: error_"<<fileName<<endl;
        exit(1);
    }
    writeToFile(errorFile,"\n\n************PASS2************");



    objectCode = "";
    currentTextRecordLength=0;
    currentRecord = "";
    M_Record = "";

    nobase = true;

    read_IntermediateFile(intermediateFile,Is_Comment,lineNumber,address,label,opcode,operand,comment);


    while(Is_Comment)
    {
        writeData = to_string(lineNumber) + "\t" + comment;
        writeToFile(intermediate_File2,writeData);
        read_IntermediateFile(intermediateFile,Is_Comment,lineNumber,address,label,opcode,operand,comment);
    }

    if(opcode=="START")
    {
        startAddress = address;
        writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t"+ label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
        writeToFile(intermediate_File2,writeData);
    }
    else
    {
        label = "";
        startAddress = 0;
        address = 0;
    }


    writeData = "H^"+fill_String(label,6,' ',true)+'^'+intToStringHex(address,6)+'^'+intToStringHex(program_length,6);
    writeToFile(objectFile,writeData);

    read_IntermediateFile(intermediateFile,Is_Comment,lineNumber,address,label,opcode,operand,comment);
    currentTextRecordLength  = 0;


    while(opcode!="END" )
    {
        if(!Is_Comment)
        {
            if(OPTAB[real_Opcode(opcode)].exists=='y')
            {
                if(OPTAB[real_Opcode(opcode)].format==1)
                {
                    objectCode = OPTAB[real_Opcode(opcode)].opcode;
                }
                else if(OPTAB[real_Opcode(opcode)].format==2)
                {
                    operand1 = operand.substr(0,operand.find(','));//
                    operand2 = operand.substr(operand.find(',')+1,operand.length()-operand.find(',') -1 );

                    if(operand2==operand) //if only one register
                    {
                        if(REGTAB[operand1].exists=='y')
                        {
                            objectCode = OPTAB[real_Opcode(opcode)].opcode + REGTAB[operand1].num + '0';

                        }
                        else
                        {
                            objectCode = real_Opcode(opcode) + '0' + '0';
                            writeData = "Line: "+to_string(lineNumber)+" Register 1 not found.";
                            writeToFile(errorFile,writeData);
                        }
                    }
                    else //if two registers
                    {
                        if(REGTAB[operand1].exists=='n')
                        {
                            objectCode = OPTAB[real_Opcode(opcode)].opcode + "00";
                            writeData = "Line: "+to_string(lineNumber)+" Register 1 not found.";
                            writeToFile(errorFile,writeData);
                        }

                        else if(REGTAB[operand2].exists=='n')
                        {
                            objectCode = OPTAB[real_Opcode(opcode)].opcode + "00";
                            writeData = "Line: "+to_string(lineNumber)+" Register 2 not found";
                            writeToFile(errorFile,writeData);
                        }
                        else
                        {
                            objectCode = OPTAB[real_Opcode(opcode)].opcode + REGTAB[operand1].num + REGTAB[operand2].num;
                        }
                    }
                }
                else if(OPTAB[real_Opcode(opcode)].format==3)
                {
                    if(real_Opcode(opcode)=="RSUB")
                    {
                        objectCode = OPTAB[real_Opcode(opcode)].opcode;
                        objectCode += (opcode_operand_Format(opcode)=='+')?"000000":"0000";
                    }
                    else
                    {
                        objectCode = ObjectCode_F3_F4();
                    }
                }
            }//If opcode in optab

            else if(opcode=="BYTE")
            {
                if(operand[0]=='X')
                {
                    objectCode = operand.substr(2,operand.length()-3);

                }

                else if(operand[0]=='C')
                {
                    objectCode = stringToHexString(operand.substr(2,operand.length()-3));

                }
            }

            else if(opcode=="WORD")
            {
                objectCode = intToStringHex(stoi(operand),6);

            }
            else if(opcode=="BASE")
            {
                if(SYMTAB[operand].exists=='y')
                {
                    Base_reg = stringHexToInt(SYMTAB[operand].address);//address stored in base
                    nobase = false;

                }
                else
                {
                    writeData = "Line "+to_string(lineNumber)+" : "+ operand +" Symbol doesn't exists in SYMTAB";
                    writeToFile(errorFile,writeData);

                }
                objectCode = "";
            }

            else
            {
                objectCode = "";

            }

            write_T_Record();


            writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;

        }//if not comment
        else
        {
            writeData = to_string(lineNumber) + "\t" + comment;

        }
        writeToFile(intermediate_File2,writeData);

        read_IntermediateFile(intermediateFile,Is_Comment,lineNumber,address,label,opcode,operand,comment);
        objectCode = "";
    }

    write_T_Record();


    write_E_Record(false);

    if(!Is_Comment)
    {
        writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t" + opcode + "\t" + operand + "\t" + "" +"\t" + comment;

    }
    else
    {
        writeData = to_string(lineNumber) + "\t" + comment;

    }
    writeToFile(intermediate_File2,writeData);


    write_T_Record(true);
    if(!Is_Comment)
    {

        writeToFile(objectFile,M_Record,false);
        write_E_Record(true);
        M_Record="";

    }

}

