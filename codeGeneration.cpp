#include "codeGeneration.hpp"
#include "node.hpp"
#include "token.hpp"

#include <fstream>
#include <queue>
#include <string>

//Prototypes
std::string processN(node::Node *node);
std::string processA(node::Node *node);
std::string processNPrime(node::Node *node, std::string tempVarPrevious);
void processM(node::Node *node);
void processR(node::Node *node);
void processStat(node::Node *node);
void processStats(node::Node *node);
void processMStat(node::Node *node);
void processBlock(node::Node *node);
void processAssign(node::Node *node);
void codeGeneration::processIf(node::Node *node);


namespace {
    int _labelCntr=0; /* counting unique labels generated */
    int _varCntr = 0; /* counting unique temporaries generated */
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

void codeGeneration::produceLabel(std::string label) {
    std::string output;
    output.append(label + ": NOOP\n");

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
    if(node->getChildThree()) {
        exprTemp = processExpr(node->getChildThree());
        _out << "LOAD T" + nTemp + "\n";
        _out << "SUB T" + exprTemp + "\n";
        std::string finalTempVar = std::to_string(_varCntr);
        _varCntr++;
        _varIds.push("T" + finalTempVar);
        _out << "STORE T" + finalTempVar + "\n";
        //std::cout << "DEBUGGER: Value returned from expr is " << finalTempVar << std::endl; 
        return finalTempVar;
    }

    return nTemp;
}

std::string processN(node::Node *node) { //<N> -> <A><NPrime>
    //Get the number of the temp var that stores the result of <A>
    std::string tempVar = processA(node->getChildOne());
    //Send that value to nPrime, retrieve the temp var holding the result
    if(node->getChildTwo()) {
        tempVar = processNPrime(node->getChildTwo(), tempVar);
    }
    return tempVar;
}

//Returns the number of the temp var that stores the result of processA
std::string processA(node::Node *node) { //<A> -> <M> * <A> | <M>
    std::string tempVarNumber;
    processM(node->getChildOne());
    _out << "STORE T" + std::to_string(_varCntr) + "\n";
    tempVarNumber = std::to_string(_varCntr);
    _varCntr++;
    _varIds.push("T" + tempVarNumber);

    if(node->getChildThree()) {
        processA(node->getChildThree());

        _out << "MULT T" + tempVarNumber + "\n";
        _out << "STORE T" + std::to_string(_varCntr) + "\n";
        tempVarNumber = std::to_string(_varCntr);
        _varCntr++;
        _varIds.push("T" + tempVarNumber);
    }

    return tempVarNumber;
}

std::string processNPrime(node::Node *node, std::string tempVarPrevious) { //<NPrime> -> empty | / <A> <NPrime> | + <A> <NPrime>
    if(node == NULL) return "";

    //process RHS before adding/dividing
    //Store value, load previous value, 
    std::string tempNumberA = processA(node->getChildTwo());

    if(node->getChildOne()->getData().tokenInstance == "+") {
        _out << "LOAD T" + tempVarPrevious + "\n";
        _out << "ADD T" + tempNumberA + "\n";
    }
    else if(node->getChildOne()->getData().tokenInstance == "/") {
        _out << "LOAD T" + tempVarPrevious + "\n";
        _out << "DIV T" + tempNumberA + "\n";
    }

    std::string tempVarNPrime = std::to_string(_varCntr);
    _out << "STORE T" + tempVarNPrime + "\n";
    _varIds.push("T" + tempVarNPrime);
    //std::cout << "In processNPrime, temp var is " << tempVarNPrime;
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
        return;
    }
    
    _out << "LOAD " + node->getChildOne()->getData().tokenInstance + "\n";
}

void codeGeneration::processLoop1(node::Node *node) { //<loop1> -> while [ <expr> <RO> <expr> ] <stat>
    //Produce a label to jump back to for testing
    std::string newLabel = "L" + std::to_string(_labelCntr);
    _varIds.push(newLabel);
    _labelCntr++;
    codeGeneration::produceLabel(newLabel);

    //Evaluate both expressions, placing them into temporary variables
    std::string exprTemp = codeGeneration::processExpr(node->getChildThree());
    std::string exprTemp2 = codeGeneration::processExpr(node->getChildFive());

    //Processing RO. TODO pull into reusable function later, if possible.
    //Don't think it will work quite the same for loop2 and if statements, but this works here
    std::string op = node->getChildFour()->getChildOne()->getData().tokenInstance;
    if(op == "<") { //If(expr1 < expr2) then (expr1 - expr2) < 0
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRPOS L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 > 0) Jump out
    }
    else if(op == ">") { //If(expr1 > expr2) then (expr1 - expr2) > 0
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n"; 
        _out << "BRNEG L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 < 0) Jump out
    }
    else if(op == "==") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRPOS L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 != 0) Jump out
        _out << "BRNEG L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 != 0) Jump out
    }
    /*else if(op == "...") { //If ((expr1 DIV 2 MULT 2 SUB expr1) == 0) && ((expr2 DIV 2 MULT 2 SUB expr2), Jump back in
        //Process if expr1 is even (equal to zero is even)
        _out << "LOAD T" + exprTemp + "\n";
        _out << "DIV 2\n";
        _out << "MULT 2\n";
        _out << "SUB T" + exprTemp + "\n";
        std::string newTemp = std::to_string(_varCntr);
        _varIds.push("T" + newTemp); //TODO I should really make a function for this
        _varCntr++;
        _out << "STORE T" + newTemp + "\n";

        //Process if expr2 is even (equal to zero is even)
        _out << "LOAD T" + exprTemp2 + "\n";
        _out << "DIV 2\n";
        _out << "MULT 2\n";
        _out << "SUB T" + exprTemp2 + "\n";
        
        //If they are both odd, then both values will be negative
        //If they are both even, then both values will be zero


    }*/
    else if(op == "=!=") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRZ L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 == 0) Jump OUT
    }

    processStat(node->getChildSeven());

    //Jump back in by default
    _out << "BR L" + std::to_string(_labelCntr - 1) + "\n";

    //Produce a label to jump out
    std::string newLabel2 = "L" + std::to_string(_labelCntr);
    _varIds.push(newLabel2);
    _labelCntr++;
    codeGeneration::produceLabel(newLabel2);
}

void codeGeneration::processLoop2(node::Node *node) { //<loop2> -> repeat <stat> until [ <expr> <RO> <expr> ]
    //Produce a label to jump back to
    std::string newLabel = "L" + std::to_string(_labelCntr);
    _varIds.push(newLabel);
    _labelCntr++;
    codeGeneration::produceLabel(newLabel);

    processStat(node->getChildTwo());

    //Evaluate both expressions, placing them into temporary variables
    std::string exprTemp = codeGeneration::processExpr(node->getChildFive());
    std::string exprTemp2 = codeGeneration::processExpr(node->getChildSeven());

    //Processing RO. TODO pull into reusable function later, if possible.
    //Don't think it will work quite the same for loop2 and if statements, but this works here
    std::string op = node->getChildSix()->getChildOne()->getData().tokenInstance;
    if(op == "<") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRPOS L" + std::to_string(_labelCntr - 1) + "\n"; //if(expr1 - expr2 > 0) Jump back in
    }
    else if(op == ">") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n"; 
        _out << "BRNEG L" + std::to_string(_labelCntr - 1) + "\n"; //if(expr1 - expr2 < 0) Jump back in
    }
    else if(op == "==") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRPOS L" + std::to_string(_labelCntr - 1) + "\n"; //if(expr1 - expr2 != 0) Jump in
        _out << "BRNEG L" + std::to_string(_labelCntr - 1) + "\n"; //if(expr1 - expr2 != 0) Jump in
    }
    /*else if(op == "...") { //If ((expr1 DIV 2 MULT 2 SUB expr1) == 0) && ((expr2 DIV 2 MULT 2 SUB expr2), Jump back in
        //Process if expr1 is even (equal to zero is even)
        _out << "LOAD T" + exprTemp + "\n";
        _out << "DIV 2\n";
        _out << "MULT 2\n";
        _out << "SUB T" + exprTemp + "\n";
        std::string newTemp = std::to_string(_varCntr);
        _varIds.push("T" + newTemp); //TODO I should really make a function for this
        _varCntr++;
        _out << "STORE T" + newTemp + "\n";

        //Process if expr2 is even (equal to zero is even)
        _out << "LOAD T" + exprTemp2 + "\n";
        _out << "DIV 2\n";
        _out << "MULT 2\n";
        _out << "SUB T" + exprTemp2 + "\n";
        
        //If they are both odd, then both values will be negative
        //If they are both even, then both values will be zero


    }*/
    else if(op == "=!=") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRZ L" + std::to_string(_labelCntr - 1) + "\n"; //if(expr1 - expr2 == 0) Jump IN
    }
}

/*void processRO(node::Node *node, std::string temp, std::string temp2) { //<RO> -> < | > | == | ... (three tokens here) | =!=
    std::string op = node->getChildOne()->getData().tokenInstance;
    if(op == "<") {
        _out << "BRNEG L" + _labelCntr;
    }
    else if(op == ">") {
        _out << "BRPOS L" + _labelCntr;
    }
    else if(op == "==") {
        _out << "BRZ L" + (_labelCntr - 1);
    }
    else if(op == "...") {

    }
    else if(op == "=!=") {

    }
    
}*/

//<stat> -> <in> ; | <out> ; | <block> | <if> ; | <loop1> ; | <loop2> ; | <assign> ; |
//<goto> ; | <label> ; | <pick> ;
void processStat(node::Node *node) {
    std::cout << "entering stat \n";
    node::Node *child = node->getChildOne();
    std::string token = child->getData().tokenInstance;

    std::cout<<"In stat. Token is " << token << std::endl;

    if(token == "in()") {
        codeGeneration::produceCin(child->getChildTwo()->getData());
    }
    else if(token == "out()") {
        std::string tempVarNum = codeGeneration::processExpr(child->getChildTwo());
        codeGeneration::produceCout(tempVarNum);
    }
    else if(token == "block()") {
        processBlock(child);
    }
    else if(token == "if()") {
        codeGeneration::processIf(child);
    }
    else if(token == "loop1()") {
        codeGeneration::processLoop1(child);
    }
    else if(token == "loop2()") {
        codeGeneration::processLoop2(child);
    }
    else if(token == "assign()") {
        processAssign(child);
    }
    else if(token == "goto()") {
        codeGeneration::produceJump(child->getChildTwo()->getData());
    }
    else if(token == "label()") {
        codeGeneration::produceLabel(child->getChildTwo()->getData().tokenInstance);
    }
    /*else if(token == "pick()") { //TODO
        tree::insert(pick(), root);
    }*/
}

void processBlock(node::Node *node) { //<block> -> { <vars> <stats> }
    node::Node *vars = node->getChildTwo()->getChildOne();
    if(vars) { //<vars> -> empty | create Identifier ; | create Identifier := Integer ; <vars>
        if(vars->getChildFour())
            codeGeneration::produceVars(vars->getChildTwo()->getData().tokenInstance, vars->getChildFour()->getData().tokenInstance);
        else 
            codeGeneration::produceVars(vars->getChildTwo()->getData().tokenInstance, std::to_string(-7));
    }
    codeGeneration::processStats(node->getChildThree());
}

void codeGeneration::processStats(node::Node *node) { //<stats> -> <stat> <mStat>
    std::cout << "Entering stats";
    processStat(node->getChildOne());
    processMStat(node->getChildTwo());
}

void processMStat(node::Node *node) { //<mStat> -> empty | <stat> <mStat>
    if(node == NULL || node->getChildOne() == NULL) return;
    processStat(node->getChildOne());
    processMStat(node->getChildTwo());
}

//Takes a subtree
void codeGeneration::processIf(node::Node *node) { //<if> -> if [ <expr> <RO> <expr> ] then <stat>
    //Evaluate both expressions, placing them into temporary variables
    std::string exprTemp = codeGeneration::processExpr(node->getChildThree());
    std::string exprTemp2 = codeGeneration::processExpr(node->getChildFive());

    std::string op = node->getChildFour()->getChildOne()->getData().tokenInstance;
    if(op == "<") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRPOS L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 > 0) Jump out
    }
    else if(op == ">") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n"; 
        _out << "BRNEG L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 < 0) Jump out
    }
    else if(op == "==") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRPOS L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 != 0) Jump out
        _out << "BRNEG L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 != 0) Jump out
    }
    else if(op == "=!=") {
        _out << "LOAD T" + exprTemp + "\n";
        _out << "SUB T" + exprTemp2 + "\n";
        _out << "BRZ L" + std::to_string(_labelCntr) + "\n"; //if(expr1 - expr2 == 0) Jump out
    }

    processStat(node->getChildEight());

    std::string newLabel = "L" + std::to_string(_labelCntr);
    _varIds.push(newLabel);
    _labelCntr++;
    codeGeneration::produceLabel(newLabel);
}

void processAssign(node::Node *node) { //<assign> -> set Identifier = <expr> | Identifier = <expr>
    std::cout << "In processAssign\n";
    std::string tempVar;
    if(node->getChildFour()) {
        tempVar = codeGeneration::processExpr(node->getChildFour());
        _out << "LOAD T" + tempVar + "\n";
        _out << "STORE " + node->getChildTwo()->getData().tokenInstance + "\n";
    }
    else {
        tempVar = codeGeneration::processExpr(node->getChildThree());
        _out << "LOAD T" + tempVar + "\n";
        std::cout << "trying to store to x" << std::endl;
        _out << "STORE " + node->getChildOne()->getData().tokenInstance + "\n";
    }
}

//Declares all stored identifiers
void codeGeneration::stopAndDeclareAllVars() {
    std::string var;
    std::string output;

    output = "STOP\n";

    while(!_varIds.empty()) {
        var = _varIds.front();
        output.append(var + " 0\n");
        _varIds.pop();
    }

    _out << output;
}
