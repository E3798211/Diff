#ifndef DIFF_H_INCLUDED
#define DIFF_H_INCLUDED

#include <ctype.h>
#include <cmath>

#include "Tree.h"

// =================================================

const char  DEFAULT_INPUT[] = "datadef.txt";
const char DEFAULT_OUTPUT[] = "outdef.txt";

// =================================================

class Diff {
//private:
public:
    /// Main tree
    Tree source;

    /// Destination tree
    Tree dest;

    /// Varriable
    char variable[100] = {};

    /// Status
    bool differentiated_successfully = true;

    /// Status
    bool optimized_successfully = true;

    /// Status
    /**
        Variable's value is 0 if tree is optimized, !0 otherwise
    */
    int diff_optimized = 0;

    // =============================================

    /// Creates align
    /**
        Returns amount of tabs

        \param [in] output      Output file
        \param [in] depth       Amount of tabs :)
    */
    int Align(FILE* output, int depth);

    /// Reads file
    /**
        Returns nullptr if something has gone wrong

        \param [in] input        Data file
    */
    char* FileRead(FILE* input);

    /// Identifies data type
    /**
        Returns error code

        \warning data and cur_var will be changed if spaces would be found in the beginning and the end

        \param [in] data            Data you want to identify
        \param [in] cur_var         Current variable
        \param [out] data_value     Data converted to double (if needed, 0 otherwise)
        \param [out] data_type      Type of data
    */
    int IdentifyData(char* data, char* cur_var, double* data_value, int* data_type);

    /// Appends node with data from file
    /**
        \param [in] data            String with data
        \param [in] app_node        Pointer to the node to be appended
        \param [in] place_in_data   Place in data where to look for data
        \param [in] current_var     Current variable
    */
    int AppendNode(char* data, int* place_in_data, Node* app_node, char* current_var);

    /// Loads data from database and creates tree
    /**
        \param [in] filename        Name of the data file
        \param [in] current_var     Current variable
    */
    int LoadData(const char* filename, char* current_var);

    /// Prints branch to the file recurcively to the file
    /**
        \param [in] output          Output file
        \param [in] root_node       Root of the branch
        \param [in] current_var     Current variable
        \param [in] recursion_depth Depth of the recursion
    */
    int PrintBranch(FILE* output, Node* root_node, char* current_var, int recursion_depth = 0);

    /// Loads data to the file
    /**
        \param [in] filename        Name of the file to be created
        \param [in] current_var     Current variable
    */
    int UnloadData(const char* filename, char* current_var);


    /// Differentiates complex functions
    /**
        \param [in] node_to_diff    Node to be differented
        \param [in] how_to_diff_complex     Pointer to the fuction to diff this node
    */
    //Node* ComplexFunc(Node* node_to_diff, Node* (Diff::*how_to_diff_complex)(Node* node_to_diff));
    Node* ComplexFunc(Diff& d, Node* node_to_diff, Node* (*how_to_diff_complex)(Node* node_to_diff));

    friend Node* Copy(Diff& d, Node* node_to_copy);

    friend Node* diffSIN(Diff& d, Node* node_to_diff);
    friend Node* diffCOS(Diff& d, Node* node_to_diff);
    friend Node* diffTAN(Diff& d, Node* node_to_diff);
    friend Node* diffLN (Diff& d, Node* node_to_diff);
    friend Node* diffEXP(Diff& d, Node* node_to_diff);

    friend Node* diffSUM(Diff& d, Node* node_to_diff);
    friend Node* diffSUB(Diff& d, Node* node_to_diff);
    friend Node* diffMUL(Diff& d, Node* node_to_diff);
    friend Node* diffDIV(Diff& d, Node* node_to_diff);
    friend Node* diffPOW(Diff& d, Node* node_to_diff);


public:
    /// Constructor
    /**
        \param [in] filename        Name of the file with data
        \param [in] current_var     Current variable
    */
    Diff(const char* filename, char* current_var);

    /// Default destructor
    ~Diff();

    /// Differentiates tree
    friend Node* Differetiate(Diff& d, Node* source_root_node);

    /// Optimizes tree
    /**
        Function deletes (*0), (+/- 0), (const + const) etc
    */
    int Optimize();

    int DeleteObvious (Node* branch_root);
    bool CountConstants(Node* branch_root);

    /// Takes diff
    int TakeDiff();
};


/// "Clears" data
/**
    Deletes spaces and turnes every letter to lower case

    \param [in, out] data           Data to be cleared
*/
char* ClearData(const char* data);







#endif // DIFF_H_INCLUDED
