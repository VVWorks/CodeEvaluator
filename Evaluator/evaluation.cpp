
/*                       Code Evaluator 1.0.0 - an open source evaluator for Windows to test your programs.
                            Author of the project: Vornicescu Vasile  Contact info: vvworks22@gmail.com
      You can use it for free as long as you don't include this in a paid product. If you share the project give credits to the author.
*/

#include <bits/stdc++.h>
#include <iomanip>
#include <dirent.h>

#include <stdio.h>
#include <process.h>
#include <windows.h>
#include <chrono>

#define SET(A) SetConsoleTextAttribute(hConsole, A);

HANDLE  hConsole;
char USER_IN[]{'T','E','S','T','S','/','I','N','P','U','T','/','\0'}, USER_OUT[]{'T','E','S','T','S','/','O','U','T','P','U','T','/','\0'};

char BLACK = '0', BLUE = '1', GREEN = '2', AQUA = '3', RED = '4', PURPLE = '5', YELLOW = '6', WHITE = '7',
GRAY = '8', L_BLUE = '9', L_GREEN = 'a', L_AQUA = 'b', L_RED = 'c', L_PURPLE = 'd', L_YELLOW = 'e', L_WHITE = 'f';

int def_text_col = 22, def_evaluator_mess_col = 27, def_index_col = 26, def_error_col = 28, def_details_col = 19, def_paths_col = 24,
    def_commandname_col = 29, def_typing_col = 23;

struct Sample_Test
{
    int ID;
    std::string path_in, path_out, status;
    bool in = false, out = false;
};

struct Program
{
    std::string path;
    std::string name;

    Program(std::string p, std::string n){
        path = p; name = n;
    }
};

static std::map<int, Sample_Test> TESTS;
const int GLOBAL_TEST_MAX = 10000;
int N_MAX = 0, TEST_MAX = 100;

std::string IN_OUTfileNames = "file";

std::vector<Program> PROGRAMS;
int GLOBAL_TIME_MAX = 600000;//ms (60 seconds)
int TIME_MAX = 1000;//ms (1 second)

namespace wincol
{
    char BACKGROUND_COLOR_def = '0', TEXT_COLOR_def = '7';
    char COLOR_def[]{'c','o','l','o','r',' ','0','7','\0'};

    void BACKGROUND_COLOR(char color)
    {
        COLOR_def[6] = color; COLOR_def[7] = TEXT_COLOR_def;
        system(COLOR_def);
        BACKGROUND_COLOR_def = color;
    }

    void TEXT_COLOR(char color)
    {
        COLOR_def[6] = BACKGROUND_COLOR_def; COLOR_def[7] = color;
        system(COLOR_def);
        TEXT_COLOR_def = color;
    }

    void BACKGROUND_TEXT_COLOR(char B_color, char T_color)
    {
        COLOR_def[6] = B_color; COLOR_def[7] = T_color;
        system(COLOR_def);
        TEXT_COLOR_def = T_color;
        BACKGROUND_COLOR_def = B_color;
    }
}

BOOL IsProcessRunning(HANDLE &process)
{
    DWORD ret = WaitForSingleObject(process, 0);
    return ret == WAIT_TIMEOUT;
}



bool toInt(std::string &str, int &integer)
{
    bool num = true; int k = 0, zec = 1;
    for(int i=str.length()-1; i>=0; i--){
        if(str[i]<48 || str[i]>57){
            num = false;
        }else{
            k+=(str[i] - 48)*zec;
            zec*=10;
        }
    }

    integer = k;

    return num;
}

void CapsOFF(std::string &str)
{
    std::for_each(str.begin(), str.end(), [](char & c) {
        c = ::tolower(c);
    });
}

void INTRO()
{
    SET(2);
    std::cout<<"Program EVALUATOR 1.0.0  --Author: Vornicescu Vasile--"<<std::setw(186)<< std::setiosflags(std::ios::right)<<"."<<std::endl;
}

void ShowHelp()
{
    SET(def_commandname_col);
    std::cout<<"\nCommands: -------------------------------------------------\n";
    SET(def_text_col);
    std::cout<<"   help                      - opens the help menu \n"//done
             <<"   exit                      - close the console \n"//done
             <<"   clear                     - clears the screen \n"//done
             <<"   view_ti                   - view all input files \n"//done
             <<"   view_ti[n]                - view the n-th input file \n"//done
             <<"   view_to                   - view all output files \n"//done
             <<"   view_to[n]                - view the n-th output file \n"//done
             <<"   dec[n]                    - ignore the n-th test case \n"//done
             <<"   act[n]                    - reactive the n-th test case \n"//done
             <<"   set(n, input, output)     - import a test case as the n-th test !(n=0 for auto assign)!\n"//done
             <<"   status                    - see details about the sample tests \n"//done
             <<"   update                    - update sample tests \n"//done
             <<"   see[n]                    - see the n-th input and expected output file \n"//done
             <<"   see                       - see all sample tests \n"//done
             <<"   max[n]                    - set n as the maximum number of tests \n"//done
             <<"   tim[n]                    - set the maximum execution time in ms (max = 60s)\n"//done
             <<"   fil[n]                    - set the name of how your program handles the input/output files\n"//done
             <<"   programs                  - see all programs \n"//done
             <<"   add(path, name)           - add a new program with the path to the *.exe file \n"//done
             <<"   evy[programID]            - evaluate the given program (not showing the correct output if program fails \n"//done
             <<"   evn[programID]            - evaluate the given program (show the correct output if program fails \n";//done
}


void SetMax(std::string n)
{
    SET(def_commandname_col);

    int k = 0;
    bool num = toInt(n, k);

    if(num){
        if(k<=GLOBAL_TEST_MAX && k>0){
            TEST_MAX = k;
            std::cout<<"Changed test max to "<<k<<". In the next update this will be applied\n";
        }else{
            SET(def_error_col);
            std::cout<<"The given maximum is out of limits [1, 10000]!\n";
        }
    }else{
        SET(def_error_col);
        std::cout<<"The given index is not valid!\n";
    }
}

void DisplayFile(std::ifstream &in)
{
    SET(def_text_col);
    if(in.is_open()){
        std::string line; int l = 0;
        while(std::getline(in, line)){
            std::cout<<"    "<<line<<"\n";
            l++;
        }

        if(l==0){
            std::cout<<"    No content!\n";
        }
    }else{
        SET(def_error_col);
        std::cout<<"\n File not found!\n";
    }
}

void ReadThisFile(std::ifstream &in, std::string &str)
{
    str="";
    if(in.is_open()){
        std::string line; int l = 0;
        while(std::getline(in, line)){
            str+=line+'\n';
        }
    }
}

void showStatus()
{
    int shown = 0;
    SET(def_commandname_col);
    std::cout<<"\nSample tests status: -------------------------------------------------\n";
    std::cout<<"      Maximum tests: "<<TEST_MAX<<'\n';
    std::cout<<"      Maximum execution time: "<<TIME_MAX<<"ms\n";
    std::cout<<"      Default in/out filename: "<<IN_OUTfileNames<<"\n\n";

    for(int i=1; i<=N_MAX; i++){
            if(!TESTS[i].in || !TESTS[i].out){
                TESTS[i].status = "MISSING";
            }

        SET(def_details_col);
        std::cout<<"  Sample test ["<<i<<"]:\n";
        std::cout<<"      Status: ";
        SET(def_index_col);
        if(TESTS[i].status!="ACTIVE"){
            SET(def_error_col);
        }
        std::cout<<TESTS[i].status<<"\n"; SET(def_details_col);

        std::cout<<"      INPUT:  ";
            if(TESTS[i].in){
                SET(def_paths_col);
                std::cout<<TESTS[i].path_in;
            }else{
                std::cout<<" --- ";
            }

        SET(def_details_col);
        std::cout<<"\n      OUTPUT: ";
            if(TESTS[i].out){
                SET(def_paths_col);
                std::cout<<TESTS[i].path_out;
            }else{
                std::cout<<" --- ";
            }
        std::cout<<"\n\n";
        shown++;
    }

    if(shown == 0){
        std::cout<<"No sample tests (try to update)!\n";
    }
}

void UpdateTests()
{
    for(int i=0; i<=N_MAX; i++){
        TESTS[i].ID =0;
        TESTS[i].in = false; TESTS[i].out = false;
        TESTS[i].path_in = "";
        TESTS[i].path_out = "";
    }

    N_MAX = 0;
    SET(def_commandname_col);
    std::cout<<"\nUpdating sample tests -------------------------------------------------\n";

    char *directoryStr = USER_IN;
    int inFiles = 0, outFiles = 0;

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir (directoryStr)) != NULL) {
      while ((ent = readdir (dir)) != NULL){
        std::string path = ent->d_name;

        if(path.length()>=4){
            if(path[path.length()-1]=='n' && path[path.length()-2]=='i'){//open only *.in files
                int i = path.length() - 4, n = 0, zec = 1;
                bool next = true;
                while(i>=0 && next){
                    if(path[i]>=48 && path[i]<=57){
                        n+=zec*(path[i]-48);
                        i--; zec*=10;
                    }else{
                        next = false;
                    }
                }

                if(n!=0 && n<=TEST_MAX){
                    TESTS[n].ID = n;
                    TESTS[n].path_in = path;
                    TESTS[n].status = "MISSING";
                    TESTS[n].in = true;
                    inFiles ++;
                    if(n>N_MAX){
                        N_MAX = n;
                    }
                }
            }
        }

      }
    }

    directoryStr = USER_OUT;
    if ((dir = opendir (directoryStr)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
        std::string path = ent->d_name;

        if(path.length()>=4){
            if(path[path.length()-3]=='o' && path[path.length()-2]=='u' && path[path.length()-1]=='t'){//open only *.out files
                int i = path.length() - 5, n = 0, zec = 1;
                bool next = true;
                while(i>=0 && next){
                    if(path[i]>=48 && path[i]<=57){
                        n+=zec*(path[i]-48);
                        i--; zec*=10;
                    }else{
                        next = false;
                    }
                }

                if(n!=0 && n<=TEST_MAX){
                    TESTS[n].ID = n;
                    TESTS[n].path_out = path;
                    TESTS[n].out = true;

                    if(TESTS[n].in){
                        TESTS[n].status = "ACTIVE";
                    }

                    outFiles ++;
                    if(n>N_MAX){
                        N_MAX = n;
                    }
                }
            }
        }

      }
    }

    std::cout<<"  Sample tests updated: "<<inFiles<<" Input files and "<<outFiles<<" Output filse found\n";
    std::cout<<"In order to see more info type 'status'\n";
}

void View_INandOUT(std::string n, bool print, int tK)
{
    int k = 0;
    bool num = false;

    if(tK!=0){
        k = tK;
        num = true;
    }
    else{
        num = toInt(n, k);
    }

    SET(def_commandname_col);
    if(num && k <= N_MAX){
        if(!print){
            SET(def_details_col);
            std::cout<<"\nSample test "<<k<<":\n";
        }else{
            std::cout<<"\nSample test "<<k<<": -------------------------------------------------\n";
        }

        n = TESTS[k].path_in;

        SET(def_details_col);std::cout<<"Status: ";
        SET(def_index_col);
        if(TESTS[k].status!="ACTIVE"){
            SET(def_error_col);
        }
        std::cout<<TESTS[k].status<<"\n";

        if(TESTS[k].in){
            std::string path = "TESTS/INPUT/"+n;
            std::ifstream in(path);

            if(in.is_open()){
                SET(def_details_col);
                std::cout<<"Input: \n";
                DisplayFile(in);
            }

            in.close();
        }else{
            SET(def_error_col);
            std::cout<<"INPUT file missing.\n";
        }

        n = TESTS[k].path_out;
        if(TESTS[k].out){
            std::string path = "TESTS/OUTPUT/"+n;
            std::ifstream in(path);

            if(in.is_open()){
                SET(def_details_col);
                std::cout<<"Output: \n";
                DisplayFile(in);
            }

            in.close();
        }else{
            SET(def_error_col);
            std::cout<<"OUTPUT file missing.\n";
        }

    }else{
        SET(def_error_col);
        std::cout<<"\nNo such file (Try to update!)\n";
    }
}

void View_INandOuts()
{
    SET(def_commandname_col);
    std::cout<<"\nSample tests: -------------------------------------------------\n";
    for(int i=1; i<=N_MAX; i++){
        View_INandOUT("",false,i);
        SET(def_typing_col);
        std::cout<<"\n---------------------------------------------------------------\n";
    }

    if(N_MAX == 0){
        std::cout<<"No sample tests (try to update)!\n";
    }
}

void ViewAll_INorOUTs(bool inp)
{
    char* directoryStr = USER_IN; std::string operationName = "INPUT";

    SET(def_commandname_col);
    if(inp){
        std::cout<<"\nInput files: -------------------------------------------------\n";
    }else{
        std::cout<<"\nOutput files: -------------------------------------------------\n";
        directoryStr = USER_OUT; operationName = "OUTPUT";
    }

    std::ifstream in;

    DIR *dir;
    struct dirent *ent;

    int shown = 0;
    for(int i=1; i<=N_MAX; i++){
        if((TESTS[i].in && inp) || (TESTS[i].out && !inp)){
            std::string path = TESTS[i].path_in;
            if(!inp){
                path = TESTS[i].path_out;
            }

            if(path.length()>=3){
                    bool valFile = path[path.length()-1]=='n' && path[path.length()-2]=='i';
                    if(!inp){
                        valFile = path[path.length()-3]=='o' && path[path.length()-2]=='u' && path[path.length()-1]=='t';
                    }

                if(ValidateRect){//open only *.in/out files
                    in.open(directoryStr + path);
                    if(in.is_open()){
                        shown++;
                        SET(def_details_col);
                        std::cout<<"\n"<<operationName<<"[";
                        SET(def_index_col);std::cout<<i; SET(def_details_col);
                        std::cout<<"], Filename: ";
                        SET(def_paths_col);std::cout<<path; SET(def_details_col);
                        std::cout<<"\n";

                        std::cout<<"File content:\n";
                        DisplayFile(in);
                    }
                    in.close();
                }
            }
        }
    }

    if(shown == 0){
        std::cout<<"No "<<operationName<<" files. (Try to update!)\n";
    }
}

void View_INorOUT(std::string n, bool inp)
{
    int k = 0;
    bool num = toInt(n, k);

    std::string OperationNameUP = "INPUT", OperationNameLow = "input";
    if(!inp){
        OperationNameUP = "OUTPUT";
        OperationNameLow = "output";
    }

    SET(def_commandname_col);
    if(((TESTS[k].in && inp) || (TESTS[k].out && !inp)) && num){
        std::cout<<"\nOpening the "<<k<<"-th "<<OperationNameLow<<" file -------------------------------------------------\n";

        n = TESTS[k].path_in;
        if(!inp){
            n = TESTS[k].path_out;
        }

        std::string path = "TESTS/"+OperationNameUP+"/"+n;
        std::ifstream in(path);

        if(in.is_open()){
            SET(def_details_col);
            std::cout<<OperationNameUP<<" file: "<<n<<"\n";
            std::cout<<"File content:\n";
            DisplayFile(in);
        }

        in.close();
    }else{
        SET(def_error_col);
        std::cout<<"\nNo such file (Try to update!)\n";
    }
}

void Activate(std::string n)
{
    SET(def_commandname_col);

    int k = 0;
    bool num = toInt(n, k);

    if(num && k<=N_MAX){
        if(TESTS[k].in && TESTS[k].out){
            TESTS[k].status = "ACTIVE";
            std::cout<<"Test "<<k<<" activated\n";
        }else{
            std::cout<<"Test "<<k<<" cannot be activated\n";
        }
    }else{
        SET(def_error_col);
        std::cout<<"\nNo such test (Try to update!)\n";
    }
}

void Deactivate(std::string n)
{
    SET(def_commandname_col);

    int k = 0;
    bool num = toInt(n, k);

    if(num && k<=N_MAX){
        if(TESTS[k].in && TESTS[k].out){
            TESTS[k].status = "INACTIVE";
        }
        std::cout<<"Test "<<k<<" deactivated \n";
    }else{
        SET(def_error_col);
        std::cout<<"\nNo such test (Try to update!)\n";
    }
}

void SetTest(std::string n)
{
    std::string c[3];
    int j = 0;

    bool valid = true;

    for(int i=0; i<n.length(); i++){
        if(n[i]==','){
            j++;
            if(j>2 || c[j-1].length()==0){
                valid = false;
            }
        }else if(valid && n[i]!=' '){
            c[j].push_back(n[i]);
        }
    }

    if(c[2].length()==0){
        valid = false;
    }

    if(valid){
        SET(def_commandname_col);
        std::cout<<"\nAdding a new sample test -------------------------------------------------\n";

        int k = 0;
        bool num = toInt(c[0], k);

        if(num && k<=TEST_MAX){//set(k, path1, path2)
            bool p1=false, p2=false;

            std::ifstream f(c[1]);
            if(f.is_open()){
                p1 = true;
                ReadThisFile(f, c[1]);
            }else{
                SET(def_error_col);
                std::cout<<"Could not open: "<<c[1]<<"\n";
            }
            f.close();

            if(p1){
                f.open(c[2]);
                if(f.is_open()){
                    p2 = true;
                    ReadThisFile(f, c[2]);
                }else{
                    SET(def_error_col);
                    std::cout<<"Could not open: "<<c[2]<<"\n";
                }
                f.close();
            }

            if(p1 && p2){
                std::cout<<"Sample test "<<k<<" added\n";
                //add the sample test;

                bool good = true;

                if(k==0){
                    int freeSpace = 0;
                    for(int i=0; i<=N_MAX; i++){
                        if(!TESTS[i].in && !TESTS[i].out){
                            freeSpace = i;
                            i = N_MAX + 1;
                        }
                    }

                    if(freeSpace!=0){
                        k = freeSpace;
                    }else{
                        if(N_MAX + 1<=TEST_MAX){
                            k = N_MAX + 1;
                        }else{
                            SET(def_error_col);
                            std::cout<<"No free space left, try increasing the limit (use max[n])\n";
                            good = false;
                        }
                    }

                }

                if(good){
                    std::string path_in = "input_" + std::to_string(k) + ".in";
                    std::string path_out = "output_" + std::to_string(k) + ".out";

                    std::ofstream copF("TESTS/INPUT/"+path_in);
                    copF<<c[1];
                    copF.close();

                    copF.open("TESTS/OUTPUT/"+path_out);
                    copF<<c[2];
                    copF.close();

                    TESTS[k].ID = k;
                    TESTS[k].in = true;
                    TESTS[k].out = true;
                    TESTS[k].path_in = path_in;
                    TESTS[k].path_out = path_out;
                    TESTS[k].status = "ACTIVE";

                    if(k>N_MAX){
                        N_MAX = k;
                    }
                }
            }

        }else{
            SET(def_error_col);
            std::cout<<c[0]<<" is not an integer or is out of the limits [1, 10000]!\n";
        }

    }else{
        SET(def_error_col);
        std::cout<<"Invalid indexes\n";
    }
}

void ChangeTime(std::string n)
{
    int k = 0;
    bool num = toInt(n, k);

    if(num){
        if(k > 0 && k <=GLOBAL_TIME_MAX){
            SET(def_commandname_col);
            std::cout<<"Execution time limit set to "<<k<< "ms\n";
            TIME_MAX = k;
        }else{
            SET(def_error_col);
            std::cout<<"The given time is out of the limits [1, 60000]ms\n";
        }
    }else{
        SET(def_error_col);
        std::cout<<n<<" is not a number\n";
    }
}

void AddProgram(std::string n)
{
    std::string c[2];
    int j = 0;

    bool valid = true;

    for(int i=0; i<n.length(); i++){
        if(n[i]==','){
            j++;
            if(j>1 || c[j-1].length()==0){
                valid = false;
            }
        }else if(valid && n[i]!=' '){
            c[j].push_back(n[i]);
        }
    }

    if(c[1]==""){valid = false;}

    if(c[0]=="evaluation.exe"){
        valid = false;
    }

    int c0s = c[0].length();//check if *.exe file
    if(c0s<5){
        valid = false;
    }else{
        if(c[0][c0s - 1]!='e' || c[0][c0s - 2]!='x' || c[0][c0s - 3]!='e' || c[0][c0s - 4]!='.'){
            valid = false;
        }
    }

    if(valid){
        bool found = false;

        std::ifstream p(c[0]);
        if(p.is_open()){
            found = true;
        }
        p.close();

        if(found){
            PROGRAMS.push_back(Program(c[0], c[1]));
            SET(def_commandname_col);
            std::cout<<"Program "<<c[1]<<" added with the index "<<PROGRAMS.size()<<'\n';
            std::cout<<c[0]<<'\n';
        }else{
            SET(def_error_col);
            std::cout<<"Could not find the executable\n";
        }
    }else{
        SET(def_error_col);
        std::cout<<"Invalid arguments!\n";
    }
}

void SeePrograms()
{
    SET(def_commandname_col);
    std::cout<<"List of programs: \n";

    for(int i=0; i<PROGRAMS.size(); i++){
        SET(def_details_col);
        std::cout<<"\n  Program ";
        SET(def_paths_col);
        std::cout<<PROGRAMS[i].name;
        SET(def_details_col);
        std::cout<<"\n  Index: ";
        SET(def_paths_col);
        std::cout<<i+1<<'\n';
        SET(def_details_col);
        std::cout<<"  Path: ";
        SET(def_paths_col);
        std::cout<<PROGRAMS[i].path<<'\n';
    }

    if(PROGRAMS.size()==0){
        std::cout<<"No programs! (Try to add some with add)\n";
    }
}

void evaluate(std::string n, bool show)
{
    char* directoryStr = USER_IN;

    int k = 0;
    bool num = toInt(n, k);
    k--;

    if(num && k<PROGRAMS.size() && k>=0){
        SET(def_commandname_col);
        std::cout<<"Evaluating program ";
        SET(def_paths_col);std::cout<<PROGRAMS[k].name<<'\n';

        std::ifstream targ(PROGRAMS[k].path, std::ios::binary);
        if(targ.is_open()){
            //set up working space:


            std::ofstream prog("program.exe", std::ios::binary);
            prog<<targ.rdbuf();

            prog.close();
            targ.close();

            for(int i=1; i<=N_MAX; i++){

                if(TESTS[i].status=="ACTIVE"){
                    SET(def_details_col);
                    std::cout<<"Test "<<i<<": "<<TESTS[i].status<<'\n';


                    directoryStr = USER_IN;
                    //make a copy of the input
                    std::ofstream in(IN_OUTfileNames+".in");
                    std::ifstream in_targ(directoryStr + TESTS[i].path_in);

                    if(in_targ.is_open()){
                        if(in.is_open()){

                            in<<in_targ.rdbuf();

                        }else{
                            SET(def_paths_col);
                            std::cout<<"Could not evaluate test "<<i<<'\n';
                        }
                    }else{
                        SET(def_paths_col);
                        std::cout<<"Could not find the test "<<i<<'\n';
                    }

                    in.close();
                    in_targ.close();

                    //enviroment ready for program execution

                    int exit_code; bool killed = false;

                    HANDLE handle = (HANDLE)spawnl(P_NOWAIT, "program.exe", "program.exe", NULL);
                    auto start = std::chrono::high_resolution_clock::now();

                    while(IsProcessRunning(handle)){
                        auto now = std::chrono::high_resolution_clock::now();
                        int delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
                        //std::cout<<delta<<'\n';
                        if(delta>TIME_MAX){
                            TerminateProcess(handle, exit_code);
                            killed = true;
                        }
                    }

                    auto done = std::chrono::high_resolution_clock::now();

                    if(!killed){
                        SET(def_details_col);
                        std::cout<<"    Execution time: ";
                        std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(done - start).count()<<"ms ";
                    }else{
                        SET(def_error_col);
                        std::cout<<"Execution killed, time limit exceeded: ";
                        std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(done - start).count()<<'/'<<TIME_MAX<<"ms \n";
                    }

                    //evaluate the output

                    if(!killed){
                        directoryStr = USER_OUT;
                        std::ifstream prog_out(IN_OUTfileNames+".out");
                        std::ifstream expected_out(directoryStr + TESTS[i].path_out);
                        if(prog_out.is_open()){

                            if(expected_out.is_open()){

                                //compare the output
                                std::string l1, l2; bool correct = true;

                                while(std::getline(expected_out, l1) && correct){
                                    std::getline(prog_out, l2);
                                    if(l1!=l2){
                                        correct = false;
                                    }
                                }

                                if(correct){
                                    SET(def_index_col);
                                    std::cout<<" Output is correct!\n";
                                }else{
                                    SET(def_error_col);
                                    std::cout<<" Output is not correct!\n";
                                    //show expected output:

                                    if(show){
                                        prog_out.clear();
                                        prog_out.seekg(0, std::ios::beg);
                                        expected_out.clear();
                                        expected_out.seekg(0, std::ios::beg);

                                        SET(def_details_col);
                                        std::cout<<"Your output:\n";
                                        DisplayFile(prog_out);

                                        SET(def_details_col);
                                        std::cout<<"\nExpected output:\n";
                                        DisplayFile(expected_out);
                                    }
                                }

                            }else{
                                SET(def_error_col);
                                std::cout<<"Output is not opening!\n";
                            }

                        }else{
                            SET(def_error_col);
                            std::cout<<" Output not generated!\n";
                        }
                        prog_out.close();
                        expected_out.close();

                    }
                    //end

                    std::cout<<'\n';

                }else{
                    SET(def_paths_col);
                    std::cout<<"Test "<<i<<": "<<TESTS[i].status<<"\n\n";
                }
            }

        }else{
            SET(def_error_col);
            std::cout<<"Could not open "<<PROGRAMS[k].path<<'\n';
        }

        targ.close();

    }else{
        SET(def_error_col);
        std::cout<<"Invalid argument!\n";
    }
}

void ChangeFileName(std::string n)
{
    SET(def_commandname_col);
    std::cout<<"Changing filename to "<<n<<'\n';
    IN_OUTfileNames = n;
}

std::string command;
void CommandCenter()
{
    SET(def_evaluator_mess_col);
    std::cout<<"\n<Evaluator>:  ";
    SET(def_typing_col);
    std::getline (std::cin, command);

    CapsOFF(command);

    if(command == "help"){
        ShowHelp();
    }

    if(command == "update"){
        UpdateTests();
    }

    if(command == "status"){
        showStatus();
    }

    if(command == "exit" || command == "close" || command == "quit"){
        SET(def_error_col);
        std::cout<<"Quiting...";
        SET(def_text_col);
        exit(0);
    }

    if(command == "clear"){
        system("cls");
        INTRO();
    }

    if(command == "programs"){
        SeePrograms();
    }

    std::string com = command.substr(0, 7);
    if(com == "view_ti"){
        if(command.length() == 7){
            ViewAll_INorOUTs(true);
        }else if(command[7] =='[' && command[command.length()-1]==']'){
            View_INorOUT(command.substr(8, command.length()- 9), true);
        }
    }
    if(com == "view_to"){
        if(command.length() == 7){
            ViewAll_INorOUTs(false);
        }else if(command[7] =='[' && command[command.length()-1]==']'){
            View_INorOUT(command.substr(8, command.length()- 9), false);
        }
    }

    //fil[file name]
    if(command.length()>5 && command[0]=='f' && command[1]=='i' && command[2] == 'l' && command[3]=='[' && command[command.length() - 1] == ']'){
        ChangeFileName(command.substr(4, command.length() - 5));
    }

    //evn[n]
    if(command.length()>5 && command[0]=='e' && command[1]=='v' && command[2] == 'n' && command[3]=='[' && command[command.length() - 1] == ']'){
        evaluate(command.substr(4, command.length() - 5), false);
    }

    //evy[n]
    if(command.length()>5 && command[0]=='e' && command[1]=='v' && command[2] == 'y' && command[3]=='[' && command[command.length() - 1] == ']'){
        evaluate(command.substr(4, command.length() - 5), true);
    }

    //tim[n]
    if(command.length()>5 && command[0]=='t' && command[1]=='i' && command[2] == 'm' && command[3]=='[' && command[command.length() - 1] == ']'){
        ChangeTime(command.substr(4, command.length() - 5));
    }

    //max[n]
    if(command.length()>5 && command[0]=='m' && command[1]=='a' && command[2] == 'x' && command[3]=='[' && command[command.length() - 1] == ']'){
        SetMax(command.substr(4, command.length() - 5));
    }

    //add(path, name)
    if(command.length()>5 && command[0]=='a' && command[1]=='d' && command[2] == 'd' && command[3]=='(' && command[command.length() - 1] == ')'){
        AddProgram(command.substr(4, command.length() - 5));
    }

    //set(n, in, out)
    if(command.length()>5 && command[0]=='s' && command[1]=='e' && command[2] == 't' && command[3]=='(' && command[command.length() - 1] == ')'){
        SetTest(command.substr(4, command.length() - 5));
    }

    //see[n]
    if(command.length()>5 && command[0]=='s' && command[1]=='e' && command[2] == 'e' && command[3]=='[' && command[command.length() - 1] == ']'){
        View_INandOUT(command.substr(4, command.length() - 5), true, 0);
    }

    //see
    if(command.length()==3 && command[0]=='s' && command[1]=='e' && command[2] == 'e'){
        View_INandOuts();
    }

    //act[n]
    if(command.length()>5 && command[0]=='a' && command[1]=='c' && command[2] == 't' && command[3]=='[' && command[command.length() - 1] == ']'){
        Activate(command.substr(4, command.length() - 5));
    }

    //dec[n]
    if(command.length()>5 && command[0]=='d' && command[1]=='e' && command[2] == 'c' && command[3]=='[' && command[command.length() - 1] == ']'){
        Deactivate(command.substr(4, command.length() - 5));
    }

    CommandCenter();
}

int main()
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    wincol::BACKGROUND_TEXT_COLOR(BLUE, WHITE);

    INTRO();

    CommandCenter();
}
