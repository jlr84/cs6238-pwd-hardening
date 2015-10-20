# cs6238-pwd-hardening
# README.md
# CS6238, Project I, Password Hardening
# Last Updated: 20 Oct 2015
# James Roberts | Lei Zhang

This is a project folder for CS6238, Fall Semester 2015; any students currently taking this course who happen to find this github repository should use their own work. If any of this is utilzed, please cite this source. 


Here are step by step instructions for compiling and running this program:

1.A) Clone Repository from github
  a) Install git if not already installed:
    $sudo apt-get install git

  b) Clone Repository:
    $git clone https://github.com/jlr84/cs6238-pwd-hardening.git

1.B) If not cloning from github, unzip/extract files to folder

2) Change directory to the root folder:
  $cd cs6238-pwd-hardening/

3) Install GMP Library, if not already installed:
  $sudo apt-get install libgmp3-dev

4) Install OPENSSL Library, if not already installed:
  $sudo apt-get install libssl-dev

5) Compile "server.c" program, using flags for gmp, crypto, and math libraries; we will use "server" for the compiled program name:
  $gcc server.c -lcrypto -lgmp -lm -o server

6) Execute program:
  a) Add input text file to current directory; we will assume the input text file is "input.txt".

  b.1) Execute server with the input file name as the only input:
    $./server input.txt

  b.2) Alternately, direct the standard output to a file for easier analysis after program execution:
    $./server input.txt > programOutput.txt

7) After execution, program output is found here:
  ./OutputFile.txt


NOTE, the "module_tests" folder contains individual portions of our code that "validate" specific sub-modules of our project against the listed project requirements. Specifically, see the following, which can each be compiled individually: 
    ./module_tests/instruction-table-creation/  -- Demonstration of initialization of instruction table
    ./module_tests/module2_encrypt-decrypt.c  -- Demonstration of encryption and decryption of constant size history file
    ./module_tests/coordinates-computing  -- Demonstration of computing x and y coordinates and hardened password (hpwd')
    ./module_tests/module4_update-history-file.c  -- Demonstration of correct updating of history file 
    ./module_tests/error-correction  -- Demonstration of simple error correction 
    ./module_tests/instruction-table-update  -- Demonstration of correct update of instruction table
    ./module_tests/math-test  -- Provides a test file that demonstrates calculation of both population and sample standard deviations

