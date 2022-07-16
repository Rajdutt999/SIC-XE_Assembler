//************************************************************INPUT***************************************************************//
void read_word(string line,int& index,bool& status,string& data,bool readTillEnd=false)
{
    data = "";
    status = true;
    if(readTillEnd)
    {
        data = line.substr(index,line.length() - index);
        if(data=="")
        {
            status = false;
        }
        return;
    }
    while(index<line.length()&&!is_Blank_Tab(line[index]))
    {
        data += line[index];
        index++;
    }

    if(data=="")
    {
        status = false;
    }

    while(index<line.length()&&is_Blank_Tab(line[index])) //Increase index reach next word
    {
        index++;
    }
}

void read_Byte_Operand(string line,int& index,bool& status,string& data)
{
    data = "";
    status = true;
    if(line[index]=='C')         // eg LABEL  BYTE    C'EOF'
    {
        data += line[index++];
        char end_char = line[index++];
        data += end_char;
        while(index<line.length() && line[index]!=end_char)
        {
            data += line[index];
            index++;
        }
        data += end_char;
        index++;
    }
    else
    {
        while(index<line.length()&&!is_Blank_Tab(line[index]))
        {
            data += line[index];
            index++;
        }
    }

    if(data=="")
    {
        status = false;
    }

    while(index<line.length()&&is_Blank_Tab(line[index])) //Increase index to pass all whitespace
    {
        index++;
    }
}

string readTillTab(string data,int& index)
{
    string tempBuffer = "";

    while(index<data.length() && data[index] != '\t')
    {
        tempBuffer += data[index];
        index++;
    }
    index++;
    if(tempBuffer==" ")
    {
        tempBuffer="-1" ;
    }
    return tempBuffer;
}

bool read_IntermediateFile(ifstream& readFile,bool& Is_Comment, int& lineNumber, int& address, string& label, string& opcode, string& operand, string& comment)
{
    string fileLine="";
    string tempBuffer="";
    bool tempStatus;
    int index=0;
    if(!getline(readFile, fileLine))
    {
        return false;
    }
    lineNumber = stoi(readTillTab(fileLine,index));

    Is_Comment = (fileLine[index]=='.')?true:false;
    if(Is_Comment)
    {
        read_word(fileLine,index,tempStatus,comment,true);
        return true;
    }
    address = stringHexToInt(readTillTab(fileLine,index));

    label = readTillTab(fileLine,index);
    if(label=="-1")
    {
        label=" " ;
    }
    opcode = readTillTab(fileLine,index);
    if(opcode=="BYTE")
    {
        read_Byte_Operand(fileLine,index,tempStatus,operand);
    }
    else
    {
        operand = readTillTab(fileLine,index);
        if(operand=="-1")
        {
            operand=" " ;
        }

    }
    read_word(fileLine,index,tempStatus,comment,true);
    return true;

}


//************************************************************************OUTPUT**********************************************//
void writeToFile(ofstream& file,string data,bool newline=true)
{
    if(newline)
    {
        file<<data<<endl;
    }
    else
    {
        file<<data;
    }
}

void write_T_Record(bool is_lastRecord)
{
    if(is_lastRecord)
    {
        if(currentRecord.length()>0) //Write last text record
        {
            writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
            writeToFile(objectFile,writeData);
            currentRecord = "";
        }
        return;
    }
    if(objectCode != "")
    {
        if(currentRecord.length()==0)
        {
            writeData = "T^" + intToStringHex(address,6) + '^';
            writeToFile(objectFile,writeData,false);
        }
        // objectCode length > 60
        if((currentRecord + objectCode).length()>60)
        {

            writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
            writeToFile(objectFile,writeData);


            currentRecord = "";
            writeData = "T^" + intToStringHex(address,6) + '^';
            writeToFile(objectFile,writeData,false);
        }

        currentRecord += objectCode;
    }
    else
    {

        if( opcode!="START" && opcode!="END" && opcode!="BASE" && opcode!="EQU" )
        {

            if(currentRecord.length()>0)
            {
                writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
                writeToFile(objectFile,writeData);
            }
            currentRecord = "";
        }
    }
}

void write_E_Record(bool write)
{
    if(write)
    {
        if(E_Record.length()>0)
        {
            writeToFile(objectFile,E_Record);

        }
        else
        {
            write_E_Record(false);
        }
    }
    if((operand==""||operand==" "||operand=="\t")) //If no operand of END
    {
        E_Record = "E^" + intToStringHex(startAddress,6);

    }
    else
    {

        string end_address=first_Object_Code_LOC;

        E_Record = "E^" + end_address+"\n";
    }

}
