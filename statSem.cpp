#include "parser.hpp"
#include "statSem.hpp"
#include "symbolTable.hpp"
#include "token.hpp"

//Prototypes
void processBlock(node::Node*);
void processVars(node::Node*, symbolTable::Scope*);
void processIdentifier(node::Node*, symbolTable::Scope*);
void traversePreorder(node::Node*, symbolTable::Scope*);

namespace {

    //Global for the purpose of ensuring that local identifiers don't have the same name as globals
    symbolTable::Scope *_globalSTV = new symbolTable::Scope();

}

void statSem::driver(node::Node *tree) {
    //<program> -> <vars> tape <func> <block> | <vars> tape <block>

    traversePreorder(tree, _globalSTV);

    std::cout << "Program successfully passed static semantic check." << std::endl;
}

//root, left, middle, right
void traversePreorder(node::Node *root, symbolTable::Scope *scope) {
    if(root == NULL) return;
    //std::cout << "Current Token: " << root->getData().tokenInstance << std::endl;

    if(root->getData().tokenInstance == "block()") {
        //std::cout << "Calling block()\n";
        //Create a new scope to operate in
        processBlock(root);
        return;
    }
    else if(root->getData().tokenInstance == "vars()") {
        //Add the identifiers to the stack
        //std::cout << "Calling vars()\n";
        processVars(root, scope);
        return;
    }
    else if(root->getData().tokenInstance == "func()") { //<func> -> func Identifier <block>
        //skip processing func identifier
        //std::cout << "Calling func()\n";
        traversePreorder(root->getChildThree(), scope);
        return;
    }
    else if(root->getData().tokenInstance == "goto()") { //<goto> -> jump Identifier
        //skip processing function identifier, because I'm only supposed to process vars
        //std::cout << "Skipping goto()\n";
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
void processBlock(node::Node *root) {
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