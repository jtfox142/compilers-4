#ifndef CODEGEN_H
#define CODEGEN_H

#include "node.hpp"

#include <deque>
#include <string>

namespace codeGeneration {

    void setOutputFile(std::string);
    void produceVars(std::string, std::string);
    void declareAllVars();
    void produceLabel(token::Token);
    void produceJump(token::Token);
    void produceCin(token::Token);
    void produceCout(std::string);
    std::string processExpr(node::Node*);


} //namespace codeGeneration

#endif  // CODEGEN_H