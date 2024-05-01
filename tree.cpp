#include "tree.hpp"

#include <iostream>
#include <iomanip>

//root, left, middle, right
void tree::printPreorder(node::Node *root, int level) {
    if(root == NULL) return;

    for(int count = 0; count < level; count++) {
        std::cout << "  ";
    }

    std::cout << root->getData().tokenInstance << std::endl;
    //file << std::setw(level * 2) << root->getData().at(0) << ":" << root->getData() << std::endl;

    printPreorder(root->getChildOne(), level+1);
    printPreorder(root->getChildTwo(), level+1);
    printPreorder(root->getChildThree(), level+1);
    printPreorder(root->getChildFour(), level+1);
    printPreorder(root->getChildFive(), level+1);
    printPreorder(root->getChildSix(), level+1);
    printPreorder(root->getChildSeven(), level+1);
    printPreorder(root->getChildEight(), level+1);
}

//Inefficient, but it works. Would love to optimize later
void tree::insert(node::Node* token, node::Node* root) {
    //std::cout << "Inserting " << word << " into the tree." << std::endl;
    if(token == NULL)
        return;

    if(root->getChildNum() == 1) {
        root->setChildOne(token);
        //std::cout << word << " has been inserted on the left." << std::endl;
    }
    else if(root->getChildNum() == 2) {
        root->setChildTwo(token);
        //std::cout << word << " has been inserted in the middle." << std::endl;
    }
    else if(root->getChildNum() == 3) {
        root->setChildThree(token);
        //std::cout << word << " has been inserted on the right." << std::endl;
    }
    else if(root->getChildNum() == 4) {
        root->setChildFour(token);
        //std::cout << word << " has been inserted on the right." << std::endl;
    }
    else if(root->getChildNum() == 5) {
        root->setChildFive(token);
        //std::cout << word << " has been inserted in the middle." << std::endl;
    }
    else if(root->getChildNum() == 6) {
        root->setChildSix(token);
        //std::cout << word << " has been inserted on the right." << std::endl;
    }
    else if(root->getChildNum() == 7) {
        root->setChildSeven(token);
        //std::cout << word << " has been inserted on the right." << std::endl;
    }
    else if(root->getChildNum() == 8) {
        root->setChildEight(token);
    }
    
}