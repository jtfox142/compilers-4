#ifndef TREE_H
#define TREE_H

#include "node.hpp"

#include <deque>
#include <string>

namespace tree {

    void printPreorder(node::Node*, int);
    void insert(node::Node*, node::Node*);

} //namespace tree

#endif  // TREE_H