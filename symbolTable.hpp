#include "token.hpp"

#include <vector>

#ifndef symbolTableH
#define symbolTableH

/*

NOTES
    * It makes more sense to process everything with local scope rules, since global 

*/

namespace symbolTable {
    static std::vector<token::Token> stack;
    bool verify(token::Token);

    class Scope {
        public:
            Scope() {
                varCount = 0;
            }
            void push(token::Token);
            void pop();
            int find(token::Token);
            int getVarCount() {
                return varCount;
            }
        
        private:
            int varCount;
    };

}  // namespace symbolTable

#endif  // symbolTableH