#include "codeGeneration.hpp"
#include "node.hpp"

#include <fstream>

#include <string>

namespace {
    static int LabelCntr=0; /* counting unique labels generated */
    static int VarCntr=0; /* counting unique temporaries generated */
    typedef enum {VAR, LABEL} nameType;
    static char Name[20]; /* for creation of unique names */
}

static char *newName(nameType what)
{ if (what==VAR) // creating new temporary
    sprintf(Name,"T%d",VarCntr++); /* generate a new label as T0, T1, etc */
  else // creating new Label
    sprintf(Name,"L%d",LabelCntr++); /* new lables as L0, L1, etc */
  return(Name);
}

// Recursive traversal
/*

TODO
    * Either add more tokenIds in the token class, or test for tokenInstance after tokenId.
        * Adding tokenids would lead to fewer if statements and simpler, more readable code,
          but would require a lot more editing of the codebase.

*/
static void codeGeneration::recGen(node::Node *node, std::string filename) { 
    std::ofstream out;
    out.open(filename, std::ios_base::app);

    std::string label, label2, argR; // local storage for temporary or label
    if (node==NULL)
    return; // no nodes

    // perform different actions based on the node label
    switch (node->getData().tokenId) { 
        case token::tokenIdList::readTok:      
            fprintf(out,"\tREAD\t%s\n",nodeP->str);
            break;
        case token:    
            recGen(stats->child1,out);           /* evaluate rhs */
            out << "\tSTORE\t" << nodeP->getData().tokenInstance
            fprintf(out,"\tSTORE\t%s\n",nodeP->tokenP.str);
            break;
        case ifNode:        
            recGen(nodeP->child3,out);              /* exprRight */
            argR = newName(VAR);
            recGen(nodeP->child1,out);              /* exprLeft */
            fprintf(out,"%SUB %s\n",argR);          /* ACC <- exprLeft - exprRight */
            label = newName(LABEL);
            if (codeP->child2->token == ==Tk) {     /* False is ACC != 0 */
            fprintf(out,"BRNEG %s\n",label);
            fprintf(out,"BRPOS %s\n",label);
            }
            recGen(nodeP->child3,out);              /* dependent statements */
            fprintf(out,"%s:\tNOOP\n",label);
            break;
// etc.
  }