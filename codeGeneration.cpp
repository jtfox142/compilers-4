#include "codeGeneration.hpp"
#include "node.hpp"
#include "token.hpp"

#include <fstream>
#include <queue>
#include <string>

namespace {
    static int LabelCntr=0; /* counting unique labels generated */
    static int VarCntr=0; /* counting unique temporaries generated */
    typedef enum {VAR, LABEL} nameType;
    static std::string Name; /* for creation of unique names */

    static std::ofstream _out; //Output file for asm
    std::queue<std::string> _varIds;
}

void codeGeneration::setOutputFile(std::string filename) {
    _out.open(filename, std::ios_base::app);
}

//Returns the code for assigning a value to a variable. Identifier is pushed onto a stack and declared at the end of the program
void codeGeneration::produceVars(token::Token tok, std::string value) { 
    std::string output;
    output = "LOAD " + value + "\n";
    output.append("STORE " + tok.tokenInstance + "\n");

    _varIds.push(tok.tokenInstance);
    
    _out << output;
}

//Utilzed for labels and for funcs
void codeGeneration::produceLabel(token::Token tok) {
    std::string output;
    output = tok.tokenInstance + ": NOOP\n";

    _varIds.push(tok.tokenInstance);

    _out << output;
}

void codeGeneration::produceJump(token::Token tok) {
    std::string output;
    output = "BR " + tok.tokenInstance + "\n";

    _out << output;
}

void codeGeneration::produceCin(token::Token tok) {
    std::string output;
    output = "READ " + tok.tokenInstance + "\n";

    _out << output;
}

//TODO process expr
void codeGeneration::produceCout(std::string expr) {
    std::string output;
    output = "WRITE " + expr + "\n"; //Need to process expression before going to next line

    _out << output;
}

//Returns the end result of the expression: a temporary var that hold the value of whatever the expr evaluated to
std::string codeGeneration::processExpr(token::Token tok) { //<expr> -> <N> - <expr> | <N>
    std::string temp1 = codeGeneration::processN(); 
        if(root->getChildTwo()) { //TODO TEST THAT THIS IF DOES NOT ERROR
            _out << "SUB ";

        }
}

//Declares all stored identifiers
void codeGeneration::declareAllVars() {
    std::string var;
    std::string output;

    while(!_varIds.empty()) {
        var = _varIds.front();
        output.append(var + " 0\n");
        _varIds.pop();
    }

    _out << output;
}

