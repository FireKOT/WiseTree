#include <stdio.h>

#include "tree.h"
#include "strsort.h"
#include "config.h"


int main () {

    tree_t WiseTree = TreeCtor("WiseData.txt");

    GDump(&WiseTree);

    TreeDtor(&WiseTree);

    return 0;
}



