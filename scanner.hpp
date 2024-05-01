#ifndef scannerH
#define scannerH

#include "token.hpp"

#include <string>
#include <vector>
#include <deque>

namespace scanner {

    token::Token getNextToken();
    void startStream(std::string);

}  // namespace scanner

#endif  // scannerH