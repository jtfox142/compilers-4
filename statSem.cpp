#include "parser.hpp"
#include "statSem.hpp"
#include "symbolTable.hpp"
#include "token.hpp"
#include "codeGeneration.hpp"

#include <fstream>

//Prototypes
void processBlockScope(node::Node*);
void processVars(node::Node*, symbolTable::Scope*);
void processIdentifier(node::Node*, symbolTable::Scope*);
void traversePreorder(node::Node*, symbolTable::Scope*);

namespace {

    //Global for the purpose of ensuring that local identifiers don't have the same name as globals
    symbolTable::Scope *_globalSTV = new symbolTable::Scope();

}

void statSem::driver(node::Node *tree, std::string filename) {
    //<program> -> <vars> tape <func> <block> | <vars> tape <block>

    codeGeneration::setOutputFile(filename);
    traversePreorder(tree, _globalSTV);
    codeGeneration::stopAndDeclareAllVars(); //Declare all vars at the end of the program

    std::cout << filename << std::endl;
}

//root, left, middle, right
void traversePreorder(node::Node *root, symbolTable::Scope *scope) {
    if(root == NULL) return;
    //std::cout << "Current Token: " << root->getData().tokenInstance << std::endl;

    if(root->getData().tokenInstance == "block()") {
        //std::cout << "Calling block()\n";
        //Create a new scope to operate in
        processBlockScope(root);
        //codeGeneration::processBlock(root);
        return;
    }
    else if(root->getData().tokenInstance == "vars()") {
        //TODO Bug: The only vars that will be loaded onto the stack are declared before tape or right after the first block starts.
        //No vars produced as a child of <stats> will be processed by statsem
        
        //Add the identifiers to the stack
        processVars(root, scope);
        return;
    }
    else if(root->getData().tokenInstance == "func()") { //<func> -> func Identifier <block>
        /*

        What I need to do is 
            * Upon declaration of func, create an object that holds a func identifier as well as the output for the asm.
            * If goto(f1) is called, search for the object with f1 identifier and send the corresponding output to _out
    
        
        traversePreorder(root->getChildThree(), scope);*/
        return;
    }
    else if(root->getData().tokenInstance == "stats()") {
        codeGeneration::processStats(root); //TODO Bug: if stats() returns a block(), it will not be processed in the static semantic check.
        return;
    }
    else if(root->getData().tokenId == token::idTok) {
        //std::cout << "Line " << root->getData().lineNumber << ": calling processIdentifier() for " << root->getData().tokenInstance << std::endl;
        processIdentifier(root, scope);
    }

    //std::cout << "Traversing\n";

    traversePreorder(root->getChildOne(), scope);
    traversePreorder(root->getChildTwo(), scope);
    traversePreorder(root->getChildThree(), scope);
    traversePreorder(root->getChildFour(), scope);
    traversePreorder(root->getChildFive(), scope);
    traversePreorder(root->getChildSix(), scope);
    traversePreorder(root->getChildSeven(), scope);
    traversePreorder(root->getChildEight(), scope);
}

//Starts a local scope for new identifiers within the block
void processBlockScope(node::Node *root) {
    symbolTable::Scope *scope = new symbolTable::Scope();
    //std::cout << "Entering a new block scope.\n";

    //<block> -> { <vars> <stats> }
    //Traverse the block until the block is empty
    traversePreorder(root->getChildOne(), scope);
    traversePreorder(root->getChildTwo(), scope);
    traversePreorder(root->getChildThree(), scope);
    traversePreorder(root->getChildFour(), scope);

    //Pop the local vars off the stack before leaving the block
    while(scope->getVarCount() > 0) {
        scope->pop();
    }
    //std::cout << "Exiting block scope.\n";
}


void processVars(node::Node *root, symbolTable::Scope *local) {
    //<vars> -> empty | create Identifier ; | create Identifier := Integer ; <vars>
    if(root == NULL || root->getChildOne() == NULL) return;

    //std::cout<< "Identifier: " << root->getChildTwo()->getData().tokenInstance << std::endl;
    //If vars() subtree is not empty, push the identifier onto the stack
    if(root->getChildOne()->getData().tokenInstance == "create") {
        token::Token token = root->getChildTwo()->getData();

        //Ensure the identifier hasn't been used before
        if(local->find(token) == -1 && symbolTable::verify(token) == false) {
            //std::cout << "Pushing token " << token.tokenInstance << " onto the stack.\n";
            local->push(token);

            //Generate code for vars
            std::string assignValue = "-7"; //Default integer value
            if(root->getChildThree()->getData().tokenInstance == ":=") { //If the var is assigned a value, fetch that value
                assignValue = root->getChildFour()->getData().tokenInstance;
            }
            codeGeneration::produceVars(token.tokenInstance, assignValue); 

            //std::cout << "Token pushed successfully\n";
        }
        else {
            std::cerr << "ERROR Line " << token.lineNumber << ": Identifier " << token.tokenInstance << " is declared multiple times.\n";
            exit(1);
        }
    }

    //If vars() is called again in the subtree, load that onto the stack as well
    if(root->getChildSix()) {
        processVars(root->getChildSix(), local);
    }
}

//Checks if a var exists in local scope, then in global scope. If not, error
void processIdentifier(node::Node *root, symbolTable::Scope *local) {
    if(local->find(root->getData()) == -1) {
        bool inGlobal = symbolTable::verify(root->getData());
        if(!inGlobal) {
            std::cerr << "ERROR Line " << root->getData().lineNumber << ": Variable " << root->getData().tokenInstance 
                << " is not declared in this scope." << std::endl;
            exit(1);
        }
    }
    /*else
        std::cout << "Identifier " << root->getData().tokenInstance << " is valid in this scope.\n";*/

}