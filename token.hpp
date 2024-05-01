#ifndef tokenH
#define tokenH

#include <iostream>
#include <string>
#include <map>

namespace token {

    enum tokenIdList { idTok, keyTok, opTok, intTok, EOFTok};

    class Token {

        public:
            inline Token() {
                tokenId = -1;
                tokenInstance = "";
                lineNumber = -1;
                charNumber = -1;
            };

            inline Token(int id, std::string instance, int lineNum, int charNum) {
                tokenId = id;
                tokenInstance = instance;
                lineNumber = lineNum;
                charNumber = charNum;
            }
            int tokenId;
            std::string tokenInstance;
            int lineNumber, charNumber;
    };

}

#endif //tokenH