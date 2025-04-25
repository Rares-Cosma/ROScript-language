#include "interpreter.h"

void interpret(vector<ASTNode*> AST) {
    for (ASTNode* node : AST) {
        node->get(); // Call the get method to print the AST nodes
    }
}
