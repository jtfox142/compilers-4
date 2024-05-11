#include "scanner.hpp"
#include "token.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>

namespace {

    static std::deque<std::string> _userInput;
    static std::string _fileName;
    static const std::vector<std::string> _keywords { 
        "start", "stop", "while", 
        "repeat", "until", "label", 
        "return", "cin", "cout", 
        "tape", "jump", "if", 
        "then", "pick", "create", 
        "set", "func" 
    };
    static int _lineNumber;
    static int _charNumber;
    static int _commentLine;
    static std::ifstream _inputStream;
    static std::stringstream _stringLine;

} //namespace

bool isKeyword(std::string tokenString) {
    bool found = (std::find(_keywords.begin(), _keywords.end(), tokenString) != _keywords.end());
    return found;
}

bool isValidIdTok(std::string tokenString) {
    //Test if the idTok contains special characters
    for(std::string::iterator it = tokenString.begin(); it != tokenString.end(); ++it) {
        if(!isalnum(*it) && (*it) != '_') {
            return false;
        }
    }

    return true;
}

bool isValidIntTok(std::string tokenString) {
    //Test if the intTok contains non-numbers
    for(std::string::iterator it = tokenString.begin(); it != tokenString.end(); ++it) {
        if(!isdigit(*it)) {
            return false;
        }
    }

    return true;
}

std::string filter(std::string tokenString) {
    //Filter out comments
    std::string comment = "//";
    int charPos = tokenString.find(comment);
    if(charPos != -1) {
        tokenString.erase(charPos);
        _commentLine = _lineNumber;
    }
    if(tokenString.length() == 0 && !_stringLine.eof())
        tokenString = "--comment";

    return tokenString;
}

//Fetches the next line of input
void getNextLine() {
    std::string readLine;
    if(!_inputStream.is_open()) {
        std::cerr << "Input stream closed itself in scanner\n";
        exit(1);   
    }
    getline(_inputStream, readLine);
    _stringLine.str({});
    _stringLine.clear();
    _stringLine << readLine;
    _lineNumber++;
    _charNumber = 1;
}

//
void scanner::startStream(std::string fileName) {
    //std::cout << "Starting filestream\n";
    _inputStream.open(fileName);
    if(!_inputStream.is_open()) 
        throw std::runtime_error("ERROR: could not open file. Possible bad file name.");
    
    //Initialize values
    getNextLine();
    _lineNumber = 1;
    _charNumber = 1;
    _commentLine = -1;
}

//Grabs the next token. Tests the first char, identifies the most likely token type,
//then checks for errors or reserved words
token::Token scanner::getNextToken() {
    //std::cout << "Getting next token\n";

    if(_stringLine.eof()) {
        //std::cout << "Reached ss eof.\n";
        getNextLine();
    }

    std::string tokenString;
    _stringLine >> tokenString;
    //std::cout << "Token string is " << tokenString <<std::endl;

    std::string filteredString = filter(tokenString);
    //std::cout << "Filtered string is: " << filteredString << std::endl;

    token::Token token;
    token.tokenInstance = filteredString;

    if(filteredString == "") {
        if(_inputStream.eof()) {
            //std::cout << "EOF detected\n";
            token::Token eof(token::tokenIdList::EOFTok, "EOF", _lineNumber, 0);
            return eof;
        }
        else {
            //TODO Recursively call getNextToken?
            token = scanner::getNextToken(); //Successfully passed filter
            filteredString = token.tokenInstance;
        }
    }

    //--comment is only returned if the ENTIRE LINE is a comment: str.length == 0 after erasing everything past //
    if(filteredString == "--comment") {
        //std::cout << "Comment detected.\n";
        getNextLine();
        token = scanner::getNextToken(); //Successfully passed filter
        filteredString = token.tokenInstance;
    }

    char beginningChar = filteredString.at(0);
        
    //If the beginning char is a letter, then it is an identifier token
    if(isalpha(beginningChar)) {
        //Check for reserved words
        if(isKeyword(filteredString)) {
            if(filteredString == "start")
                token.tokenId = token::tokenIdList::startTok;
            else if(filteredString == "stop") {
                token.tokenId = token::tokenIdList::stopTok;
            }
            else if(filteredString == "while") {
                token.tokenId = token::tokenIdList::whileTok;
            }
            else if(filteredString == "repeat") {
                token.tokenId = token::tokenIdList::repeatTok;
            }
            else if(filteredString == "until") {
                token.tokenId = token::tokenIdList::untilTok;
            }
            else if(filteredString == "label") {
                token.tokenId = token::tokenIdList::labelTok;
            }
            else if(filteredString == "return") {
                token.tokenId = token::tokenIdList::returnTok;
            }
            else if(filteredString == "cin") {
                token.tokenId = token::tokenIdList::readTok;
            }
            else if(filteredString == "cout") {
                token.tokenId = token::tokenIdList::writeTok;
            }
            else if(filteredString == "tape") {
                token.tokenId = token::tokenIdList::tapeTok;
            }
            else if(filteredString == "jump") {
                token.tokenId = token::tokenIdList::jumpTok;
            }
            else if(filteredString == "if") {
                token.tokenId = token::tokenIdList::ifTok;
            }
            else if(filteredString == "then") {
                token.tokenId = token::tokenIdList::thenTok;
            }
            else if(filteredString == "pick") {
                token.tokenId = token::tokenIdList::pickTok;
            }
            else if(filteredString == "create") {
                token.tokenId = token::tokenIdList::createTok;
            }
            else if(filteredString == "set") {
                token.tokenId = token::tokenIdList::assignTok;
            }
            else if(filteredString == "func") {
                token.tokenId = token::tokenIdList::funcTok;
            }
        }
        else {
            //Checks the word for illegal characters
            if(isValidIdTok(filteredString)) 
                token.tokenId = token::tokenIdList::idTok;
            else {
                std::cerr << "ERROR: idTok " << filteredString << " contains an illegal character.";
                exit(1);
            }
        }
    }
    else if(isdigit(beginningChar)) {
        if(isValidIntTok(filteredString)) {
            token.tokenId = token::tokenIdList::intTok;
        }
        else {
            std::cerr << "ERROR: intTok " << filteredString << " contains an illegal character.";
            exit(1);
        }
    }
    else {
        token.tokenId = token::tokenIdList::opTok;
    }

    token.lineNumber = _lineNumber;
    token.charNumber = _charNumber;

    _charNumber += filteredString.length();

    //std::cout << std::endl;
    return token;
}