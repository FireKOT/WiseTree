#include <stdio.h>

#include "tree.h"
#include "strsort.h"
#include "config.h"


void InfRun (tree_t *tree);
void LogIfNeed (tree_t *tree, const char *name);
void SaveIfNeed (tree_t *tree);


int main () {

    tree_t WiseTree = TreeCtor("WiseData.txt");

    GDump(&WiseTree);

    InfRun(&WiseTree);

    TreeDtor(&WiseTree);

    return 0;
}


void InfRun (tree_t *tree) {

    RET_ON_VAL(!tree, ERR_INVAL_ARG, );

    do {
        char *ans = StartGuess(tree);

        LogIfNeed(tree, ans);
        SaveIfNeed(tree);

    } while (AskQuestion("Do you want to continue?"));
}

void LogIfNeed (tree_t *tree, const char *name) {

    RET_ON_VAL(!tree, ERR_INVAL_ARG, );

    if (name) {
        LogCharacter(tree, name);
    }
}

void SaveIfNeed (tree_t *tree) {

    RET_ON_VAL(!tree, ERR_INVAL_ARG, );

    if (tree->ischanged) {
        if (AskQuestion("Do you want to save changes?")) {
            FILE *output = fopen("WiseDataNew.txt", "w");

            SaveTree(tree->root, output);

            fclose(output);
        }

        tree->ischanged = false;
    }
}



