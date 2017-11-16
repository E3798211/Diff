#include "Tree.h"

// =================================================    Supporting functions

int SetNodes(FILE* output, Node* branch_root)
{
    //PrintVar(branch_root);
    if(branch_root == nullptr)      return OK;

    if(output == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Output file for DOT was not opened.   =====\n");
        SetColor(DEFAULT);

        return FILE_NOT_OPENED;
    }

    fprintf(output, "%d", branch_root);

    fprintf(output, BEGIN_DECLARATION);
    fprintf(output, LABELS);
    fprintf(output, NEXT_FIELD);

    fprintf(output, BEGIN_COLUMN);

    fprintf(output, "%d",  branch_root);
    fprintf(output, NEXT_FIELD);
    fprintf(output, "%lg", branch_root->data);
    fprintf(output, NEXT_FIELD);
    fprintf(output, "%d",  branch_root->data_type);
    fprintf(output, NEXT_FIELD);
    fprintf(output, "%d",  branch_root->left);
    fprintf(output, NEXT_FIELD);
    fprintf(output, "%d",  branch_root->right);
    fprintf(output, END_COLUMN);

    fprintf(output, END_DECLARATION);

    return OK;
}

int BuildConnections(FILE* output, Node* branch_root)
{
    if(branch_root == nullptr)      return OK;

    if(branch_root->left != nullptr){
        fprintf(output, "%d", branch_root);
        fprintf(output, TO);
        fprintf(output, "%d", branch_root->left);
        fprintf(output, LEFT_DIRECTION);

    }
    if(branch_root->right != nullptr){
        fprintf(output, "%d", branch_root);
        fprintf(output, TO);
        fprintf(output, "%d", branch_root->right);
        fprintf(output, RIGHT_DIRECTION);
    }

    return OK;
}


// =================================================    Create-function

Node* Tree::CreateNode(Node* parent_node, const bool is_right)
{
    EnterFunction();

    PrintVar(parent_node);
    PrintVar(is_right);

    SAFE {
    if(!NodeExists(root, parent_node)){
        SetColor(BLUE);
        USR_INFORM printf("Parent does not exist\n");
        SetColor(DEFAULT);

        PrintVar(parent_node);
        PrintVar(is_right);

        QuitFunction();
        return nullptr;
    }
    }


    Node* new_node = nullptr;
    try
    {
        new_node = new Node;
        n_nodes++;
    }
    catch(const std::bad_alloc &ex)
    {
        SetColor(RED);
        DEBUG printf("=====   Cannot create new node   =====\n");
        SetColor(DEFAULT);

        new_node = nullptr;

        PrintVar(new_node);
        PrintVar(n_nodes);

        QuitFunction();
        return nullptr;
    }

    new_node->parent   = parent_node;
    new_node->is_right = is_right;

    PrintVar(n_nodes);
    PrintVar(parent_node);
    PrintVar(is_right);

    SetColor(GREEN);
    DEBUG printf("New node created\n");
    SetColor(DEFAULT);

    QuitFunction();
    return new_node;
}


// =================================================    Private

bool Tree::NodeExists(const Node* branch_root, const Node* check_ptr)
{
    if(check_ptr == nullptr)                return false;

    if(branch_root == nullptr)              return false;
    if(branch_root == check_ptr)            return true;

    return (NodeExists(branch_root->left,  check_ptr) ||
            NodeExists(branch_root->right, check_ptr))?    true : false;
}

int Tree::PrintBranch(FILE* output, Node* branch_root, int (*print_type)(FILE* output, Node* node_to_print))
{
    print_type(output, branch_root);

    if(branch_root->left != nullptr)
        PrintBranch(output, branch_root->left, print_type);
    if(branch_root->right != nullptr)
        PrintBranch(output, branch_root->right, print_type);

    return OK;
}

// =================================================    Public


Tree::Tree()
{
    EnterFunction();

    try
    {
        root = new Node;
        n_nodes = 1;
    }
    catch(const std::bad_alloc &ex)
    {
        SetColor(RED);
        DEBUG printf("=====   Cannot create root node   =====\n");
        SetColor(DEFAULT);
    }

    root->is_right = true;

    PrintVar(root);
    PrintVar(n_nodes);

    QuitFunction();
}

Tree::~Tree()
{
    EnterFunction();
    if(alive){
        alive = false;

        DeleteBranch(root);
    }
    QuitFunction();
}

Node* Tree::GetRoot()
{
    return root;
}

int Tree::GetNNodes()
{
    return n_nodes;
}

bool Tree::IsAlive()
{
    return alive;
}

Node* Tree::FindNode(Node* branch_root, const data_t node_data)
{
    EnterFunction();

    SAFE {
    if(branch_root == nullptr){
        QuitFunction();
        return nullptr;
    }
    }

    if(branch_root->data == node_data){
        QuitFunction();
        return branch_root;
    }

    Node* check_node = nullptr;
    if(branch_root->left != nullptr)
        check_node = FindNode(branch_root->left, node_data);

    // If found
    if(check_node != nullptr){
        SetColor(GREEN);
        DEBUG printf("Element \"%d\" found. p = %p\n", check_node->data, check_node);
        SetColor(DEFAULT);

        QuitFunction();
        return check_node;
    }

    if(branch_root->right != nullptr)
        check_node = FindNode(branch_root->right, node_data);

    QuitFunction();
    return check_node;
}

int Tree::SetData(Node* change_node, const data_t data, int data_type)
{
    EnterFunction();

    PrintVar(change_node);
    PrintVar(data);

    assert(change_node != nullptr);

    SAFE {

    // Verificator

    if(!NodeExists(root, change_node)){
        SetColor(BLUE);
        USR_INFORM printf("Node does not exist\n");
        SetColor(DEFAULT);

        PrintVar(change_node);
        PrintVar(data);

        QuitFunction();
        return NODE_DOES_NOT_EXIST;
    }
    }

    // =============================

    change_node->data      = data;
    change_node->data_type = data_type;

    // =============================

    PrintVar(change_node);
    PrintVar(data);

    QuitFunction();
    return OK;
}

bool Tree::IsLast(const Node* check_node)
{
    EnterFunction();

    SAFE {

    // Verificator

    if(!NodeExists(root, check_node)){
        SetColor(BLUE);
        USR_INFORM printf("Node does not exist\n");
        SetColor(DEFAULT);

        QuitFunction();
        return false;
    }
    }

    if (check_node->left == nullptr && check_node->right == nullptr){
        SetColor(GREEN);
        DEBUG printf("Given pointer is last\n");
        SetColor(DEFAULT);

        QuitFunction();
        return true;
    }else{
        SetColor(GREEN);
        DEBUG printf("Given pointer is not last\n");
        SetColor(DEFAULT);

        QuitFunction();
        return false;
    }
}

int Tree::DeleteBranch(Node* branch_root, int rec_depth, bool right)
{
    assert(branch_root != nullptr);

    SetColor(MAGENTA);
    DEBUG printf("\tIn:   recursion depth = %d, right = %s\n", rec_depth, right? "true" : "false");
    SetColor(DEFAULT);

    PrintVar(branch_root);
    PrintVar(branch_root->left);
    PrintVar(branch_root->right);

    SAFE {
    if(!NodeExists(root, branch_root)){
        SetColor(BLUE);
        USR_INFORM printf("Node does not exist\n");
        SetColor(DEFAULT);

        return NODE_DOES_NOT_EXIST;
    }
    }

    if((branch_root)->left != nullptr)
        DeleteBranch(branch_root->left,    rec_depth + 1, false);
    if((branch_root)->right != nullptr)
        DeleteBranch((branch_root)->right, rec_depth + 1, true);

    if(branch_root != root){
        if(branch_root->is_right)
            branch_root->parent->right = nullptr;
        else
            branch_root->parent->left  = nullptr;
    }
    else{
        root = nullptr;
    }

    delete branch_root;
    n_nodes--;

    SetColor(MAGENTA);
    DEBUG printf("\tQuit: recursion depth = %d, right = %s\n", rec_depth, right? "true" : "false");
    SetColor(DEFAULT);
    return OK;
}

int Tree::CallGraph()
{
    FILE* output = fopen(DOT_FILENAME_DEFAULT, "w");
    if(output == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Output file for DOT was not opened.   =====\n");
        SetColor(DEFAULT);

        return FILE_NOT_OPENED;
    }

    fprintf(output, BEGIN);
    fprintf(output, SET_RECORD_SHAPE);
    PrintBranch(output, root, SetNodes);
    PrintBranch(output, root, BuildConnections);
    fprintf(output, END);

    fclose(output);

    char command[1000];
    strcpy(command, DOT1);
    strcat(command, DOT_FILENAME_DEFAULT);
    strcat(command, DOT2);
    strcat(command, IMG_FILENAME_DEFAULT);

    system(command);

    strcpy(command, OPEN);
    strcat(command, IMG_FILENAME_DEFAULT);

    return system(command);
}

