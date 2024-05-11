#ifndef CODEGEN_H
#define CODEGEN_H

#include "node.hpp"

#include <deque>
#include <string>

namespace codeGeneration {

    void setOutputFile(std::string);
    void produceVars(std::string, std::string);
    void stopAndDeclareAllVars();
    void produceLabel(std::string);
    void produceJump(token::Token);
    void produceCin(token::Token);
    void produceCout(std::string);
    std::string processExpr(node::Node*);
    void processLoop1(node::Node*);
    void processLoop2(node::Node*);


} //namespace codeGeneration

#endif  // CODEGEN_H