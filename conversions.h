string itoa(int num, string &str, int base,int d)
{

    if( num<0 )
        num += 4096;

    if( num == 0 )
    {
        str="0";
    }

    while( num != 0 )
    {
        int rem = num % base;
        str += (rem > 9)? (rem-10) + 'A' : rem + '0';

        num = num/base;
    }

    reverse(str.begin(), str.end());

    while( str.length() < d )
        str="0"+str;

    return str;
}

string intToStringHex( int n ,int d = 5 )
{
    string temp;
    itoa(n,temp,16,d);

    return temp;
}

int stringHexToInt(string x)
{
    return stoul(x,nullptr,16);
}

string stringToHexString(const string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

string fill_String(string data,int length,char fill_up_string,bool i=false)
{
    if(i)
    {
        if(length<=data.length())
        {
            return data.substr(0,length);
        }
        else
        {
            for(int i = length-data.length(); i>0; i--)
            {
                data += fill_up_string;
            }
        }
    }
    else
    {
        if(length<=data.length())
        {
            return data.substr(data.length()-length,length);
        }
        else
        {
            for(int i = length-data.length(); i>0; i--)
            {
                data = fill_up_string + data;
            }
        }
    }
    return data;
}


bool isComment(string line)
{
    if(line[0]=='.')
    {
        return true;
    }
    return false;
}
bool is_Blank_Tab(char x)
{
    if(x==' ' || x=='\t')
    {
        return true;
    }
    return false;
}

string real_Opcode(string opcode)
{
    if( opcode[0] == '+' )
    {
        return opcode.substr(1,opcode.length() - 1);//return without +
    }
    return opcode;
}
char opcode_operand_Format(string data)
{
    if(data[0] == '#' || data[0] == '+' || data[0] == '@' || data[0] == '=')
    {
        return data[0];
    }
    return ' ';
}


bool is_Number(string x)
{
    bool is_all_num = true;
    int i = 0;

    while( is_all_num && ( i<x.length() ) )
    {
        is_all_num &= isdigit(x[i]);
        i++;
    }

    return is_all_num;
}

