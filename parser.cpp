#include "parser.hpp"
#include "token.hpp"
#include "scanner.hpp"
#include "node.hpp"
#include "tree.hpp"

#include <stdexcept>
#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>

namespace {

    static token::Token _lookahead;

}

//Prototypes
node::Node* program();
node::Node* func();
node::Node* block();
node::Node* vars();
node::Node* expr();
node::Node* N();
node::Node* NPrime();
node::Node* A();
node::Node* M();
node::Node* R();
node::Node* stats();
node::Node* mStat();
node::Node* stat();
node::Node* in();
node::Node* out();
node::Node* ifNonTerminal();
node::Node* pick();
node::Node* pickbody();
node::Node* loop1();
node::Node* loop2();
node::Node* assign();
node::Node* RO();
node::Node* label();
node::Node* gotoNonTerminal();

node::Node* parser::parse() {
    _lookahead = scanner::getNextToken();
    node::Node *root = program();
    
    return root;
}

//Fetches the next lookahead if the token instance matches the expected value. Exits with an error otherwise
void match(std::string terminalString, node::Node* root) {
    if(_lookahead.tokenInstance == terminalString) {
        node::Node* insertNode = new node::Node(_lookahead);
        tree::insert(insertNode, root);
        _lookahead = scanner::getNextToken();
    }
    else {
        std::cout << "ERROR: Token instance \"" << _lookahead.tokenInstance << "\" at line " << _lookahead.lineNumber 
            << " char " << _lookahead.charNumber << " did not match the expected value of \"" << terminalString << "\"" << std::endl;
        throw std::runtime_error("Mismatch Error");
    }
}

//Overloaded match function for checking tokenIds
void match(int tokenId, node::Node* root) {
    if(_lookahead.tokenId == tokenId) {
        node::Node* insertNode = new node::Node(_lookahead);
        tree::insert(insertNode, root);
        _lookahead = scanner::getNextToken();
    }
    else {
        std::string tokenIdArray[] = {"idTok", "keyTok", "opTok", "intTok", "EOFTok"};
        std::cout << "ERROR: The token instance \"" << _lookahead.tokenInstance << "\" of type " << tokenIdArray[_lookahead.tokenId] 
            << " at line " << _lookahead.lineNumber << " char " << _lookahead.charNumber 
            << " did not match the expected value of \"" << tokenIdArray[tokenId] << "\"" << std::endl;
        throw std::runtime_error("Mismatch Error");
    }
}

//Takes the string name of a nonterminal returns a vector containing the first set of that nonterminal
std::vector<std::string> first(std::string nonterminal) {
    std::vector<std::string> set;
    if(nonterminal == "stat") {
        set = {"cin", "cout", "{", "if", "while", "repeat", "set", "jump", "label", "pick"};
    }
    if(nonterminal == "block") {
        set = {"{"};
    }
    return set;
}

//pretty self explanitory.
bool setContainsLookahead(std::vector<std::string> set) {
    if(std::find(set.begin(), set.end(), _lookahead.tokenInstance) != set.end())
        return true;

    return false;
}

//<program> -> <vars> tape <func> <block> | <vars> tape <block>
node::Node* program() {
    node::Node* root = new node::Node("program()");
    tree::insert(vars(), root);
    
    try {
        match("tape", root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"program\"" << std::endl;
        exit(1);
    }

    if(_lookahead.tokenInstance == "func") {
        tree::insert(func(), root);
    }

    tree::insert(block(), root);

    return root;
}

//<func> -> func Identifier <block>
node::Node* func() {
    node::Node* root = new node::Node("func()");
    try {
        match("func", root);
        match(token::tokenIdList::idTok, root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"func\"" << std::endl;
        exit(1);
    }

    tree::insert(block(), root);

    return root;
}

//<block> -> { <vars> <stats> }
node::Node* block() {
    node::Node* root = new node::Node("block()");

    try {
        match("{", root);
        tree::insert(vars(), root);
        tree::insert(stats(), root);
        match("}", root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"block\"" << std::endl;
        exit(1);
    }

    return root;
}

//<vars> -> empty | create Identifier ; | create Identifier := Integer ; <vars>
node::Node* vars() {
    node::Node* root = new node::Node("vars()");
    try {
        if(_lookahead.tokenInstance == "create") {
            match("create", root);
            match(token::idTok, root);
            if(_lookahead.tokenInstance == ":=") {
                match(":=", root);
                match(token::tokenIdList::intTok, root);
                match(";", root);
                tree::insert(vars(), root);
            }
            else {
                match(";", root);
            }
            return root;
        }
        else
            return root;
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"vars\"" << std::endl;
        exit(1);
    }
}

//<expr> -> <N> - <expr> | <N>
node::Node* expr() {
    node::Node* root = new node::Node("expr()");
    
    tree::insert(N(), root);

    if(_lookahead.tokenInstance == "-") {
        try {
            match("-", root);
        }
        catch(const std::exception& ex) {
            std::cerr << ex.what() << " inside of nonterminal \"expr\"" << std::endl;
            exit(1);
        }

        tree::insert(expr(), root);
    }
    
    return root;
}

//<N> -> <A> <NPrime>
node::Node* N() {
    node::Node* root = new node::Node("N()");

    tree::insert(A(), root);
    tree::insert(NPrime(), root);

    return root;
}

//<NPrime> -> empty | / <A> <NPrime> | + <A> <NPrime>
node::Node* NPrime() {
    node::Node* root = new node::Node("N'()");

    try {
        if(_lookahead.tokenInstance == "/") {
            match("/", root);
            tree::insert(A(), root);
            tree::insert(NPrime(), root);
            return root;
        }
        else if(_lookahead.tokenInstance == "+") {
            match("+", root);
            tree::insert(A(), root);
            tree::insert(NPrime(), root);
            return root;
        }
        else {
            return nullptr;
        }
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"NPrime\"" << std::endl;
        exit(1);
    }
}

//<A> -> <M> * <A> | <M>
node::Node* A() {
    node::Node* root = new node::Node("A()");

    tree::insert(M(), root);

    if(_lookahead.tokenInstance == "*") {
        try {
            match("*", root);
        }
        catch(const std::exception& ex) {
            std::cerr << ex.what() << " inside of nonterminal \"A\"" << std::endl;
            exit(1);
        }
       tree::insert(A(), root);
    }

    return root;
}

//<M> -> ^ <M> | <R>
node::Node* M() {
    node::Node* root = new node::Node("M()");

    if(_lookahead.tokenInstance == "^") {
        try {
            match("^", root);
        }
        catch(const std::exception& ex) {
            std::cerr << ex.what() << " inside of nonterminal \"M\"" << std::endl;
            exit(1);
        }
        tree::insert(M(), root);
        return root;
    }

    tree::insert(R(), root);
    return root;
}

//<R> -> ( <expr> ) | Identifier | Integer
node::Node* R() {
    node::Node* root = new node::Node("R()");

    try{
        if(_lookahead.tokenInstance == "(") {
            match("(", root);
            tree::insert(expr(), root);
            match(")", root);
            return root;
        }
        else if(_lookahead.tokenId == token::tokenIdList::idTok) {
            match(token::tokenIdList::idTok, root);
            return root;
        }
        else if(_lookahead.tokenId == token::tokenIdList::intTok) {
            match(token::tokenIdList::intTok, root);
            return root;
        }
        else {
            std::cerr << "Error in nonterminal R: symbol " << _lookahead.tokenInstance << "\" did not match allowed symbols."
                << "Allowed symbols are: \"(\", identifiers, and integers." << std::endl;
            exit(1);
        }
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"R\"" << std::endl;
        exit(1);
    }
}

//<stats> -> <stat> <mStat>
node::Node* stats() {
    node::Node* root = new node::Node("stats()");

    tree::insert(stat(), root);
    tree::insert(mStat(), root);
    
    return root;
}

//<mStat> -> empty | <stat> <mStat>
node::Node* mStat() {
    node::Node* root = new node::Node("mStat()");

    std::vector<std::string> firstSet;
    firstSet = first("stat");

    if(setContainsLookahead(firstSet)) {
        tree::insert(stat(), root);
        tree::insert(mStat(), root);
        return root;
    }
    else
        return NULL;
}

//<stat> -> <in> ; | <out> ; | <block> | <if> ; | <loop1> ; | <loop2> ; | <assign> ; |
//<goto> ; | <label> ; | <pick> ;
node::Node* stat() {
    node::Node* root = new node::Node("stat()");

    std::vector<std::string> firstSet;
    firstSet = first("block");

    //If the first set of block contains the lookahead token, then...
    if(setContainsLookahead(firstSet)) {
        tree::insert(block(), root);
        return root;
    }

    std::string token = _lookahead.tokenInstance;

    if(token == "cin") {
        tree::insert(in(), root);
    }
    else if(token == "cout") {
        tree::insert(out(), root);
    }
    else if(token == "if") {
        tree::insert(ifNonTerminal(), root);
    }
    else if(token == "while") {
        tree::insert(loop1(), root);
    }
    else if(token == "repeat") {
        tree::insert(loop2(), root);
    }
    else if(token == "set") {
        tree::insert(assign(), root);
    }
    else if(token == "jump") {
        tree::insert(gotoNonTerminal(), root);
    }
    else if(token == "label") {
        tree::insert(label(), root);
    }
    else if(token == "pick") {
        tree::insert(pick(), root);
    }
    
    match(";", root);
    return root;
}

//<in> -> cin Identifier
node::Node* in() {
    node::Node* root = new node::Node("in()");

    try {
        match("cin", root);
        match(token::idTok, root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"in\"" << std::endl;
        exit(1);
    }
    return root;
}

//<out> -> cout <expr>
node::Node* out() {
    node::Node* root = new node::Node("out()");

    try {
        match("cout", root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"out\"" << std::endl;
        exit(1);
    }
    tree::insert(expr(), root);
    return root;
}

//<if> -> if [ <expr> <RO> <expr> ] then <stat>
node::Node* ifNonTerminal() {
    node::Node* root = new node::Node("if()");

    try {
        match("if", root);
        match("[", root);
        tree::insert(expr(), root);
        tree::insert(RO(), root);
        tree::insert(expr(), root);
        match("]", root);
        match("then", root);
        tree::insert(stat(), root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"ifNonTerminal\"" << std::endl;
        exit(1);
    }
    return root;
}

//<pick> -> pick <expr> <pickbody>
node::Node* pick() {
    node::Node* root = new node::Node("pick()");

    try {
        match("pick", root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"pick\"" << std::endl;
        exit(1);
    }

    tree::insert(expr(), root);
    tree::insert(pickbody(), root);

    return root;
}

//<pickbody> -> <stat> : <stat>
node::Node* pickbody() {
    node::Node* root = new node::Node("pickbody()");

    stat();

    try {
        match(":", root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"pickbody\"" << std::endl;
        exit(1);
    }

    tree::insert(stat(), root);

    return root;
}

//<loop1> -> while [ <expr> <RO> <expr> ] <stat>
node::Node* loop1() {
    node::Node* root = new node::Node("loop1()");

    try {
        match("while", root);
        match("[", root);
        tree::insert(expr(), root);
        tree::insert(RO(), root);
        tree::insert(expr(), root);
        match("]", root);
        tree::insert(stat(), root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"loop1\"" << std::endl;
        exit(1);
    }
    return root;
}

//<loop2> -> repeat <stat> until [ <expr> <RO> <expr> ]
node::Node* loop2() {
    node::Node* root = new node::Node("loop2()");

    try{
        match("repeat", root);
        tree::insert(stat(), root);
        match("until", root);
        match("[", root);
        tree::insert(expr(), root);
        tree::insert(RO(), root);
        tree::insert(expr(), root);
        match("]", root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"loop2\"" << std::endl;
        exit(1);
    }

    return root;
}

//<assign> -> set Identifier = <expr> | Identifier = <expr>
node::Node* assign() {
    node::Node* root = new node::Node("assign()");

        try {
            if(_lookahead.tokenInstance == "set") {
                match("set", root);
            }
            match(token::idTok, root);
            match("=", root);
            tree::insert(expr(), root);
        }
        catch(const std::exception& ex) {
            std::cerr << ex.what() << " inside of nonterminal \"assign\"" << std::endl;
            exit(1);
        }   

        return root;
}

//<RO> -> < | > | == | ... (three tokens here) | =!=
node::Node* RO() {
    node::Node* root = new node::Node("RO()");

    try {
        if(_lookahead.tokenInstance == "<") {
            match("<", root);
            return root;
        }
        else if(_lookahead.tokenInstance == ">") {
            match(">", root);
            return root;
        }
        else if(_lookahead.tokenInstance == ".") {
            match(".", root);
            match(".", root);
            match(".", root);
            return root;
        }
        else if(_lookahead.tokenInstance == "==") {
            match("==", root);
            return root;
        }
        else if(_lookahead.tokenInstance == "=!=") {
            match("=!=", root);
            return root;
        }
        else {
            std::cerr << "Error in RO nonterminal: could not find matching symbol for \"" << _lookahead.tokenInstance << "\"" << std::endl;
            exit(1);
        }
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"RO\"" << std::endl;
        exit(1);
    }
}

//<label> -> label Identifier
node::Node* label() {
    node::Node* root = new node::Node("label()");

    try {
        match("label", root);
        match(token::idTok, root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"label\"" << std::endl;
        exit(1);
    }

    return root;
}

//<goto> -> jump Identifier
node::Node* gotoNonTerminal() {
    node::Node* root = new node::Node("goto()");

    try {
        match("jump", root);
        match(token::idTok, root);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what() << " inside of nonterminal \"gotoNonTerminal\"" << std::endl;
        exit(1);
    }

    return root;
}