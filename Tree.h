#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <stdio.h>
#include <assert.h>
#include <cstring>
#include "Errors.h"
#include "DotSyntax.h"

// =================================================

// =========================    Safe mode.
//
//  In safe mode programm checks if node exists, runs verificator, etc.
//  In case you are sure that your programm will work correct and you need to
//  speed it up, turn safe-mode off.
//

// Comment next line to turn safe-mode off.
#define _SAFE
#ifdef _SAFE
    #define SAFE
#else
    #define SAFE if(0)
#endif // _USR_INFORM

// =========================    Print information for user
#define _USR_INFORM
#ifdef _USR_INFORM
    #define USR_INFORM
#else
    #define USR_INFORM if(0)
#endif // _USR_INFORM

// =========================    Print information debug information
// Comment next line to turn debug off.
#define _DEBUG
#include "DebugLib.h"

// =================================================
typedef double data_t;

const int  LEFT = 0;
const int RIGHT = 1;
const int  STOP = -1;

struct Node {
    /// String with data
    data_t   data = 0;

    /// Pointer to the left node
    Node*    left = nullptr;

    /// Pointer to the right node
    Node*   right = nullptr;

    /// Pointer to the parent node
    Node*  parent = nullptr;

    /// Status if node is on the right of the parent or not
    bool is_right = false;

    /// Varriable responsible of how to understand data
    int data_type = 0;
};

class Tree {
private:
    /// Pointer to the first node
    Node* root  = nullptr;

    /// Nodes counter
    int n_nodes = 0;

    /// Alive or deleted
    bool alive = true;

// =========================    Supporting functions
    /// Prints nodes for DOT
    /**
        Function provides with go-round of the branch

        \param [in] output          FILE to write in
        \param [in] branch_root     Pointer to the root of the branch
        \param [in] print_type      Pointer to the function that prints nodes
    */
    int PrintBranch(FILE* output, Node* branch_root, int (*print_type)(FILE* output, Node* node_to_print));

public:
    /// Default constructor
    Tree();

    /// Destructor
    ~Tree();

    /// Checks node existence
    /**
        Retutns TRUE if node exists

        \param [in] branch_root     Pointer to te first element in branch we check
        \param [in] node            Pointer we want to check
    */
    bool NodeExists(const Node* branch_root, const Node* check_ptr);

    /// Gets root
    Node* GetRoot();

    /// Gets n_nodes
    int GetNNodes();

    /// Finds node
    /**
        Returns Pointer to the node if found, nullptr otherwise
        \param [in] branch_root     Pointer to the root of the branch
        \param [in] node_data       Data we are looking for
    */
    Node* FindNode(Node* branch_root, const data_t node_data);

    /// Gets status - dead or alive
    bool IsAlive();

    //int AddRight(Node* app_node, Node** new_ptr);

    /// Creates node.
    /**
        Returns nullptr in case of error.

        \param [in] parent          Pointer to the parent
        \param [in] is_right        Is this node is right or left
    */
    Node* CreateNode(Node* parent, const bool is_right = true);

    /// Set data
    /**
        Returns error code

        \param [in]  change_node    Ponter to the node you want to change
        \param [out] data           New data
        \param [in]  data_type      Type of the data
    */
    int SetData(Node* change_node, const data_t data, int data_type);

    /// Check if node is last or not
    /**
        Rerturns true if elem is last, false othewise.

        \warning Returns false even if node is not in the tree.

        \param [in] check_node      Pointer to the node you want to check
    */
    bool IsLast(const Node* check_node);

    /// Delete branch
    /**
        \param [in] branch_root     Pointer to the root node of the branch we want to delete
        \param [in] dbg_rec_depth   DEBUG VARRIABLE, PLEASE DO NOT USE IT
        \param [in] dbg_right       DEBUG VARRIABLE, PLEASE DO NOT USE IT

        \warning Deletes branch RECURSIVELY! Use only first parameter or you will not see correct debug info.
        Deletes root too.
    */
    int DeleteBranch(Node* branch_root, int dbg_rec_depth = 0, bool dbg_right = true);

    /// Calls grapic dump
    int CallGraph();


    // Verificator
    // Dump()       ? done?

};

#endif // TREE_H_INCLUDED
