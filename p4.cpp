#include "scanner.hpp"
#include "parser.hpp"
#include "tree.hpp"
#include "statSem.hpp"

#include <iostream>
#include <deque>
#include <string>
#include <stdexcept>
#include <fstream>

//If there are no provided args, place user input into default.txt
std::string setFileName(int argc, char* argv[]) {
    std::string fileName;
    //The first argument should always be the path to the program, so argv[1] should be the filename
    if(argc <= 1) {
        //std::cout << "Opening a default file to handle input." << std::endl;

        std::ofstream defaultFile;
        fileName = "default.txt";

        //Need to convert string to c string for .open() function call
        defaultFile.open(fileName.c_str(), std::ios::out | std::ios::trunc);

        std::string userInput;
        std::getline(std::cin, userInput);

        defaultFile << userInput;
        defaultFile.close();

        //std::cout << "\nDefault file has received input.\n";
    }
    else { //Otherwise, attempt to use the filename provided to the executable
        fileName = argv[1];
    }

    return fileName;
}

int main(int argc, char* argv[]) {
    //Process any command line input and intake keyboard input, if necessary 
    std::string fileName = setFileName(argc, argv);

    //Try to open the file and read the data onto a deque for processing
    try {
        scanner::startStream(fileName);
        node::Node* tree = parser::parse();
        statSem::driver(tree);
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        exit(1);
    }

    //testScanner::print();
    return 0;
}