#ifndef CODEGEN_H
#define CODEGEN_H

#include "node.hpp"

#include <deque>
#include <string>

namespace codeGeneration {

    void setOutputFile(std::string);
    void produceVars(token::Token, std::string);
    void declareAllVars();
    void produceLabel(token::Token);
    void produceJump(token::Token);
    void produceCin(token::Token);
    void produceCout(std::string);
    std::string processExpr(token::Token);


} //namespace codeGeneration

#endif  // CODEGEN_H