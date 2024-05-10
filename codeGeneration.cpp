#include "codeGeneration.hpp"
#include "node.hpp"
#include "token.hpp"

#include <fstream>
#include <queue>
#include <string>

//Prototypes
std::string processN(token::Token tok);

namespace {
    static int _labelCntr=0; /* counting unique labels generated */
    static int _varCntr=0; /* counting unique temporaries generated */
    typedef enum {VAR, LABEL} nameType;
    static std::string Name; /* for creation of unique names */

    static std::ofstream _out; //Output file for asm
    std::queue<std::string> _varIds;
}

void codeGeneration::setOutputFile(std::string filename) {
    _out.open(filename, std::ios_base::app);
}

//Returns the code for assigning a value to a variable. Identifier is pushed onto a stack and declared at the end of the program
void codeGeneration::produceVars(std::string varName, std::string value) { 
    std::string output;
    output = "LOAD " + value + "\n";
    output.append("STORE " + varName + "\n");

    _varIds.push(varName);
    
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

void codeGeneration::produceCout(std::string tempVarNum) {
    std::string output;
    output = "WRITE T" + tempVarNum + "\n";
    _out << output;
}

//Returns the end result of the expression: a temporary var that hold the value of whatever the expr evaluated to
std::string codeGeneration::processExpr(node::Node *node) { //<expr> -> <N> - <expr> | <N>
    std::string nTemp = processN(node->getChildOne()); //Pass the subtree 

    std::string exprTemp; 
    if(node->getChildThree()) { //TODO TEST THAT THIS IF DOES NOT ERROR
        exprTemp = processExpr(node->getChildThree());
        _out << "LOAD T" + nTemp + "\n";
        _out << "SUB T" + exprTemp + "\n";
        std::string finalTempVar = std::to_string(_varCntr);
        _varCntr++;
        _out << "STORE T" + finalTempVar + "\n";
        return finalTempVar;
    }

    return nTemp;
}

std::string processN(node::Node *node) { //<N> -> <A><NPrime>
    //Get the number of the temp var that stores the result of <A>
    std::string aTempVar = processA(node->getChildOne());
    //Send that value to nPrime, retrieve the temp var holding the result
    std::string nPrimeTemp = processNPrime(node->getChildTwo(), aTempVar);

    return nPrimeTemp;
}

//Returns the number of the temp var that stores the result of processA
std::string processA(node::Node *node) { //<A> -> <M> * <A> | <M>
    std::string tempVarNumber;
    processM(node->getChildOne());
    _out << "STORE T" + std::to_string(_varCntr);
    tempVarNumber = std::to_string(_varCntr);
    _varCntr++;

    if(node->getChildThree()) {
        processA(node->getChildThree());

        _out << "MULT T" + tempVarNumber + "\n";
        _out << "STORE T" + std::to_string(_varCntr);
        _varCntr++;
    }

    return tempVarNumber;
}

std::string processNPrime(node::Node *node, std::string tempVarPrevious) { //<NPrime> -> empty | / <A> <NPrime> | + <A> <NPrime>
    if(node == NULL) return "";

    //process RHS before adding/dividing
    //Store value, load previous value, 
    std::string tempNumberA = processA(node->getChildTwo());
    _out << "STORE T" + std::to_string(_varCntr) + "\n";
    _varCntr++;

    if(node->getChildOne()->getData().tokenInstance == "+") {
        _out << "LOAD T" + tempVarPrevious + "\n";
        _out << "ADD T" + tempNumberA + "\n";
    }
    else if(node->getChildOne()->getData().tokenInstance == "/") {
        _out << "LOAD T" + tempVarPrevious + "\n";
        _out << "DIV T" + tempNumberA + "\n";
    }

    _out << "STORE T" + std::to_string(_varCntr) + "\n";
    std::string tempVarNPrime = std::to_string(_varCntr);
    return tempVarNPrime;
}

void processM(node::Node *node) { //<M> -> ^ <M> | <R>
    if(node->getChildTwo()) {
        processM(node->getChildTwo()); //LOAD into acc first, then MULT
        _out << "MULT -1\n";
    }
    else {
        processR(node->getChildOne());
    }
}

void processR(node::Node *node) { //<R> -> ( <expr> ) | Identifier | Integer
    if(node->getChildTwo()) {
        codeGeneration::processExpr(node->getChildTwo());
    }
    
    //TODO check to ensure incoming ints are stored in my compiler as strings
    _out << "LOAD " + node->getChildOne()->getData().tokenInstance + "\n";
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

