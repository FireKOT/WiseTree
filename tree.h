#ifndef _WISETREE_H
#define _WISETREE_H


#include <stdio.h>

#include "strsort.h"


#define GDump(tree) TreeGraphDump(tree, __FILE__, __PRETTY_FUNCTION__, __LINE__);


const size_t STRC = 1000;


struct node_t {

    char value[STRC];

    node_t *prev;
    node_t *right;
    node_t *left;

    size_t depth;
};

struct tree_t {

    node_t *root;
    int ischanged;

    size_t gdumpnum;
    size_t guessnum;
};


tree_t  TreeCtor  (const char *file_name);
void    TreeDtor  (tree_t *tree);
void    RootDtor  (node_t *root);
node_t *NodeCtor  (const char *value, size_t depth = 0);
void    NodeDtor  (node_t *node);
void    InitLog   (const char *file_name);
size_t  DescrProc (text *descr, node_t **root, size_t line_num = 0, size_t depth = 0);
void    AddNode   (node_t *node, const char *question, const char *newleafval);

char   *StartGuess        (tree_t *tree);
char   *GuessWho          (node_t *node);
int     AskQuestion       (const char *question);
char   *ProcAns           (int isright, node_t *leaf);
void    SaveTree          (node_t *node, FILE *output);
node_t *SearchNode        (node_t *root, const char *value);
void    PrintNodeInfo     (node_t *node);
void    PrintInfoTillNode (node_t *startnode, node_t *endnode);
node_t *CommonNode        (node_t *node1, node_t *node2);
void    NodesSimilarities (node_t *root, const char *targetnode, const char *samplenode);
void    NodesDifferences  (node_t *root, const char *targetnode, const char *samplenode);
void    AnalysNodes       (node_t *root, const char *targetnode, const char *samplenode);

void TreeGraphDump  (tree_t *tree, const char *file, const char *funk, int linenum);
int  MakeGNodes     (node_t *node, FILE *output, size_t num = 0);
int  MakeChain      (node_t *node, FILE *output, size_t num = 0);
void PrintGNodeCode (size_t num, const char *shape, const char *color, const char *label, FILE *output);
void PrintChainCode (size_t from, size_t where, const char *style, const char *color, size_t weight, FILE *output);
void LogCharacter   (tree_t *tree, const char *name);


#endif