#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "tree.h"
#include "strsort.h"
#include "general.h"


tree_t TreeCtor (const char *file_name) {

    tree_t err = {
        .root      = nullptr,
        .ischanged = false,

        .gdumpnum = 0,
        .guessnum = 0,
    };

    RET_ON_VAL(!file_name, ERR_INCRR_FILE, err);

    tree_t tree = {
        .root      = nullptr,
        .ischanged = false,

        .gdumpnum = 0,
        .guessnum = 0,
    };

    text descr= {
        .source      = nullptr,
        .content     = nullptr,
        .lines_count = 0,
    };

    GetText(file_name, &descr);
    DescrProc(&descr, &tree.root);

    FreeText(&descr);

    InitLog("GraphLog.html");

    return tree;
}

void TreeDtor (tree_t *tree) {

    if (!tree) return;
    
    RootDtor(tree->root);
    tree->gdumpnum = 0;
}

void RootDtor (node_t *root) {

    if (!root) return;
    
    RootDtor(root->left);
    RootDtor(root->right);

    NodeDtor(root);
}

node_t *NodeCtor (const char *value, size_t depth) {

    RET_ON_VAL(!value, ERR_INCRR_INPUT, nullptr);

    node_t *node = (node_t*) calloc(sizeof(node_t), 1);
    RET_ON_VAL(!node, ERR_NULL_PTR, nullptr);

    strcpy(node->value, value);

    node->prev  = nullptr;
    node->right = nullptr;
    node->left  = nullptr;

    node->depth = depth;

    return node;
}

void NodeDtor (node_t *node) {

    strcpy(node->value, "");

    node->prev  = nullptr;
    node->left  = nullptr;
    node->right = nullptr;

    node->depth = 0;

    free(node);
}

void InitLog (const char *file_name) {

    FILE *gdump = fopen(file_name, "w");
    RET_ON_VAL(!gdump, ERR_INCRR_FILE, );

    fprintf(gdump,  "<style>\n"

                    "::-webkit-scrollbar { width: 0;}\n"

                    "@font-face {\n"
                        "font-family: hoefler;\n"
                        "src: url(\"fonts/hoeflertext.otf\");\n"
                    "}\n"

                    "summary {"
                        "background-color: RGB(0, 0, 0);\n"
                        "border-radius: 15px;\n"
                        "font-weight: bold;\n"
                        "margin: .5em .5em 0;\n"
                        "padding: .5em;\n"

                        "font-family: hoefler;\n"
                        "text-align: center;\n"
                        "font-size: 15pt;\n"
                    "}\n"

                    "div {\n"
                        "text-align: center;\n"
                    "}\n"

                    ".dump {\n"
                        "color: rgb(5, 79, 79);\n"
                        "border: 3px solid rgb(5, 79, 79);\n"
                    "}\n"

                    ".character {\n"
                        "color: rgb(227, 79, 79);\n"
                        "border: 3px solid rgb(227, 79, 79);\n"
                    "}\n"

                    "body {\n"
                        "background: url(\"background.png\");\n"
                        "background-size: cover;\n"
                        "background-attachment: fixed;\n"
                    "}\n"

                    "</style>\n");

    fclose(gdump);
}

size_t DescrProc (text *descr, node_t **root, size_t line_num, size_t depth) {

    int isend = 0;
    sscanf(descr->content[line_num].start, " } %n", &isend);
    if(isend) return DescrProc(descr, root, line_num + 1, depth);

    int start = 0, end = 0, isleaf = 0;
    sscanf(descr->content[line_num].start, " { \"%n %*[^\"] \"%n } %n", &start, &end, &isleaf);
    descr->content[line_num].start[end-1] = '\0'; 
    char *value = descr->content[line_num].start + start;
    
    *root = NodeCtor(value, depth);

    if(!isleaf) {
        line_num = DescrProc(descr, &(*root)->left,  line_num + 1, depth + 1);
        line_num = DescrProc(descr, &(*root)->right, line_num + 1, depth + 1);

        (*root)->left->prev  = *root;
        (*root)->right->prev = *root;
    }

    return line_num;
}

void AddNode (node_t *node, const char *question, const char *newleafval) {

    RET_ON_VAL(!node || !question || !newleafval, ERR_INVAL_ARG, );

    node->left  = NodeCtor(newleafval, node->depth + 1);
    node->right = NodeCtor(node->value, node->depth + 1);

    strcpy(node->value, question);
}

char *StartGuess (tree_t *tree) {
    
    RET_ON_VAL(!tree, ERR_INVAL_ARG, nullptr);

    tree->guessnum++;
    char *ans = GuessWho(tree->root);

    if (!ans) tree->ischanged = true;

    return ans;
}

char *GuessWho (node_t *node) {

    RET_ON_VAL(!node, ERR_INVAL_ARG, nullptr);

    char question[STRC] = {};
    sprintf(question, "Is your character %s?", node->value);

    if (AskQuestion(question)) {
        if (node->left) return GuessWho (node->left);
        else return ProcAns(true, node);
    }
    else {
        if (node->right) return GuessWho (node->right);
        else return ProcAns(false, node);
    }
}

int AskQuestion (const char *question) {

    printf("%s [Y/N]\n", question);

    int c = getchar();
    int isClear = ClearStdinBuf();
    while ((c != 'Y' && c != 'y' && c != 'N' && c != 'n') || isClear != 0) {
        printf("Incorrect value\n");
        printf("%s [Y/N]\n", question);

        c = getchar();
        isClear = ClearStdinBuf();
    }

    if (c == 'Y' || c =='y') {
        return 1;
    }

    return 0;
}

char *ProcAns (int isright, node_t *leaf) {

    RET_ON_VAL(!leaf, ERR_INVAL_ARG, nullptr);

    printf("Ahahah, I now that!\n");

    if (!isright) {
        char character[STRC] = {}, question[STRC] = {};

        printf("What character did you guess?\n");
        gets(character);

        printf("How does %s differ from %s?\n", character, leaf->value);
        gets(question);

        AddNode(leaf, question, character);

        return nullptr;
    }

    return leaf->value;
}

void SaveTree (node_t *node, FILE *output) {

    RET_ON_VAL(!node || !output, ERR_INVAL_ARG, );

    fprintf(output, "%*s%s\"", 4 * node->depth + 2, "{\"", node->value);

    if (node->left && node->right) {
        fprintf(output, "\n");

        SaveTree(node->left, output);
        SaveTree(node->right, output);

        fprintf(output, "%*s", 4 * node->depth, "");
    }

    fprintf(output, "}\n");
}

node_t *SearchNode (node_t *root, const char *value) {

    RET_ON_VAL(!root || !value, ERR_INVAL_ARG, nullptr);

    if (!stricmp(value, root->value)) {
        return root;
    }

    if (root->left && root->right) {
        node_t *left = SearchNode(root->left, value);

        if(left) {
            return left;
        }
        
        return SearchNode(root->right, value);
    }

    return nullptr;
}

void PrintNodeInfo (node_t *node) {

    RET_ON_VAL(!node, ERR_INVAL_ARG, );

    if (node->prev) {
        PrintNodeInfo(node->prev);

        if (node == node->prev->right) printf("Not ");
        printf("%s\n", node->prev->value);
    }
}

void PrintInfoTillNode (node_t *startnode, node_t *endnode) {

    RET_ON_VAL(!startnode || !endnode, ERR_INVAL_ARG, );

    if (startnode->prev && startnode != endnode) {
        PrintInfoTillNode(startnode->prev, endnode);

        if (startnode == startnode->prev->right) printf("Not ");
        printf("%s\n", startnode->prev->value);
    }
}

node_t *CommonNode (node_t *node1, node_t *node2) {

    RET_ON_VAL(!node1 || !node2, ERR_INVAL_ARG, nullptr);

    if (node1->depth == node2->depth) {
        if (node1 == node2) return node1;

        return CommonNode(node1->prev, node2->prev);
    }

    if (node1->depth > node2->depth) return CommonNode(node1->prev, node2);

    return CommonNode(node1, node2->prev);
}

void NodesSimilarities (node_t *root, const char *targetnode, const char *samplenode) {

    RET_ON_VAL(!root || !targetnode || !samplenode, ERR_INVAL_ARG, );

    node_t *startnode1 = SearchNode(root, targetnode), *startnode2 = SearchNode(root, samplenode);
    RET_ON_VAL(!startnode1 || !startnode2, ERR_INVAL_ARG, );

    node_t *comm_node = CommonNode(startnode1, startnode2);
    if (comm_node != root) PrintNodeInfo(comm_node);
    else printf("This nodes don't have any similarities\n");
}

void NodesDifferences (node_t *root, const char *targetnode, const char *samplenode) {

    RET_ON_VAL(!root || !targetnode || !samplenode, ERR_INVAL_ARG, );

    node_t *startnode1 = SearchNode(root, targetnode), *startnode2 = SearchNode(root, samplenode);
    RET_ON_VAL(!startnode1 || !startnode2, ERR_INVAL_ARG, );

    node_t *comm_node = CommonNode(startnode1, startnode2);
    if (startnode1 != startnode2) PrintInfoTillNode(startnode1, comm_node);
    else printf("This nodes don't have any differences\n");
}

void AnalysNodes (node_t *root, const char *targetnode, const char *samplenode) {

    RET_ON_VAL(!root || !targetnode || !samplenode, ERR_INVAL_ARG, );

    node_t *tnodeptr = SearchNode(root, targetnode), *snodeptr = SearchNode(root, samplenode);

    printf("%s and %s both:\n", tnodeptr->value, snodeptr->value);
    NodesSimilarities(root, targetnode, samplenode);
    printf("\n");

    printf("%s unlike %s:\n", tnodeptr->value, snodeptr->value);
    NodesDifferences(root, targetnode, samplenode);
}

void TreeGraphDump (tree_t *tree, const char *file, const char *funk, int linenum) {

    char file_name[STRC] = {};
    sprintf(file_name, "gdump\\GraphImageCode#%d.txt", tree->gdumpnum);
    FILE *gdump = fopen(file_name, "w");
    RET_ON_VAL(!gdump, ERR_ACC_DENi, );

    fprintf(gdump, "digraph G{\n");
    fprintf(gdump, "bgcolor = \"transparent\"\n");
    fprintf(gdump, "size = \"15.85\"\n");
    fprintf(gdump, "\n");


    MakeGNodes(tree->root, gdump);
    fprintf(gdump, "\n");
    MakeChain(tree->root, gdump);
    fprintf(gdump, "}\n");

    fclose(gdump);


    FILE *html = fopen("GraphLog.html", "a");

    char cmd[STRC] = {}, img_name[STRC] = {};
    sprintf(img_name, "gdump\\GraphImage%d.svg", tree->gdumpnum);
    sprintf(cmd, "dot %s -T svg -o %s", file_name, img_name);
    system(cmd);

    fprintf(html, "<details>\n");
    fprintf(html, "<summary class = \"dump\">Dump #%d In file %s in function %s in line %d</summary>\n",tree->gdumpnum+1, file, funk, linenum);
    fprintf(html, "<div> <img src = %s> </div>\n", img_name);
    fprintf(html, "</details>\n\n");

    fclose(html);

    tree->gdumpnum++;
}

int MakeGNodes (node_t *node, FILE *output, size_t num) {

    if (!node) {
        return 0;
    }

    if (!node->left && !node->right) {
        PrintGNodeCode (num, "box", "#00FF7F", node->value, output);
        return num; 
    }
    PrintGNodeCode (num, "box", "#3FC391", node->value, output);

    num = MakeGNodes(node->left, output, num + 1);
    num = MakeGNodes(node->right, output, num + 1);

    return num;
}

int MakeChain (node_t *node, FILE *output, size_t num) {

    if (!node) {
        return 0;
    }

    if (!node->left && !node->right) {
        return num; 
    }

    size_t from = num;

    PrintChainCode (from, num + 1, "bold", "#7FFF00", 1, output);
    num = MakeChain(node->left, output, num + 1);

    PrintChainCode (from, num + 1, "bold", "#FF4500", 1, output);
    num = MakeChain(node->right, output, num + 1);

    return num;
}

void PrintGNodeCode (size_t num, const char *shape, const char *color, const char *label, FILE *output) {

    fprintf(output, "%d [shape = %s, style = \"filled, rounded\", fillcolor = \"%s\", label = \"%s\"];\n", num, shape, color, label);
}

void PrintChainCode (size_t from, size_t where, const char *style, const char *color, size_t weight, FILE *output) {

    fprintf(output, "%d -> %d [style = %s, color = \"%s\", weight = %d];\n", from, where, style, color, weight);
}

void LogCharacter (tree_t *tree, const char *name) {

    FILE *html = fopen("GraphLog.html", "a");

    fprintf(html, "<details>\n");
    fprintf(html, "<summary class = \"character\">Guess #%d %s</summary>", tree->guessnum, name);
    fprintf(html, "<div> <img src = \"imgs\\%s.png\"> </div> \n", name);
    fprintf(html, "</details>\n\n");

    fclose(html);
}


