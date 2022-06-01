# CodeEvaluator, Author: Vornicescu Vasile
An open source program that allows you to evaluate your code with sample tests.
This program was build for windows but you can easily modify it for other systems.

# Important: -------------------------------------------------
   -This program does not compile your code, you have to do this yourself, it tests the executable. 
   -Do not try to test unknown executables, otherwise it could be harmufull for your system.
 
# How to use: -------------------------------------------------
   First copy your inputs in the *TESTS/INPUT* folder and outputs in the *TESTS/OUTPUT* folder. Then use the command *update* to link the tests with the program. You can view certain tests, deactivate them and so on. To see the commands for doing this type *help*. Next, you should link your executable as well using the command 
add(path, name). You can link how many executables you want just make sure to name them differently. To test a program use the command *evy* or *evn* (more about them in the help menu). You also have control under a set of parameters like execution time limit, number of tests, number of programs.

# Commands: -------------------------------------------------
   #   *help*                      - opens the help menu;
   #   *exit*                      - close the console;
   *clear*                     - clears the screen;
   *view_ti*                   - view all input files;
   *view_ti[n]*                - view the n-th input file;
   *view_to*                   - view all output files;
   *view_to[n]*                - view the n-th output file;
   *dec[n]*                    - ignore the n-th test case;
   *act[n]*                    - reactive the n-th test case;
   *set(n, input, output)*     - import a test case as the n-th test (n=0 for auto assign);
   *status*                    - see details about the sample tests;
   *update*                    - update sample tests;
   *see[n]*                    - see the n-th input and expected output file;
   *see*                       - see all sample tests;
   *max[n]*                    - set n as the maximum number of tests;
   *tim[n]*                    - set the maximum execution time in ms (max = 60s);
   *programs*                  - see all programs;
   *add(path, name)*           - add a new program with the path to the *.exe file;
   *evy[programID]*            - evaluate the given program (not showing the correct output if program fails;
   *evn[programID]*            - evaluate the given program (show the correct output if program fails;
   
 
