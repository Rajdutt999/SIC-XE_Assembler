#include"declarations.h"
#include"TABs.h"
#include"conversions.h"
#include"I-O.h"
#include"pass1.h"
#include"pass2.h"

using namespace std;

int main()
{
    string fileName;


    cout<<"****Input file should be inside <tests> folder****"<<endl<<endl;
    cout<<"Enter name of input file (with or without extension)  : ";
    cin>>fileName;

    if(fileName.substr(fileName.length()-4,fileName.length()-1)==".txt")
        fileName=fileName.substr(0,fileName.length()-4);


    load_tables();

    cout<<"\nPerforming PASS1"<<endl;

    pass1(fileName);
    cout<<"pass1 successful.\n\n";


    Symtab.open("tests\\"+ fileName+"_SYMTAB.txt") ;
    writeToFile(Symtab,"**********************************SYMBOL TABLE*****************************\n") ;

    for (auto const& it: SYMTAB)
    {
        writestring+=it.first+":-\t"+ "name:"+it.second.name+"\t|"+ "address:"+it.second.address+"\t|"+ "relative:"+intToStringHex(it.second.relative)+" \n" ;
    }
    writeToFile(Symtab,writestring) ;


    cout<<"\nPerforming PASS2"<<endl;

    pass2(fileName);

    cout<<"pass2 successful.\n\n";

    cout<<fileName<<"_error.txt created \n";
    cout<<fileName<<"_intermediate.txt created \n";
    cout<<fileName<<"_intermediate_2.txt created \n";
    cout<<fileName<<"_object.txt created \n";
    cout<<fileName<<"_SYMTAB.txt created \n";


    return 0;
}
