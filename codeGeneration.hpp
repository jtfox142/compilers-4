#ifndef CODEGEN_H
#define CODEGEN_H

#include "node.hpp"

#include <deque>
#include <string>

namespace codeGeneration {

    void recGen(node::Node *nodeP, std::string out);

} //namespace codeGeneration

#endif  // CODEGEN_H