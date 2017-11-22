#ifndef DIFF_H_INCLUDED
#define DIFF_H_INCLUDED

#include <ctype.h>
#include <cmath>
#include <ctime>
#include <cstdlib>

#include <string>

#include "Tree.h"

// =================================================

const char  DEFAULT_INPUT[] = "datadef.txt";
const char DEFAULT_OUTPUT[] = "outdef.txt";

// =================================================

const std::string  FIRST[] = {"Таким образом, ", "И так, ", "Далее, ", "Предвкушая скорую концовку, ", "Надеясь на благополучный исход, ",
                              "Закинемся таблетками, ", "Потеряв всякую надежду, ", "Уповая на удачу, ", "Исступленно пялясь в выражение, ",
                              "Двигаясь дальше, "};
const std::string SECOND[] = {"нетрудо видеть, что ", "очевидно, что ", "продолжим, ", "забыв обо всем, ", "удерживаясь от сквернословия, ",
                              "не принимая во внимание нашу никчемность, "};
const std::string  THIRD[] = {"приведем выражение к виду ", "получим ", "придем к этому ", "потратив всего час жизни ",
                              "наши страдания еще не кончились "};

const int  N_FIRST = 10;
const int N_SECOND = 6;
const int  N_THIRD = 5;
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

    /// Output file
    FILE* output;

    /// Random seed
    int seed = 0;

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
    //int PrintBranch(/*FILE* output, */Node* root_node, char* current_var, int recursion_depth = 0);
    int PrintBranch(Node* root_node, char* current_var, int recursion_depth = 0, int prev_priority = 0);

    /// Loads data to the file
    /**
        \param [in] filename        Name of the file to be created
        \param [in] current_var     Current variable
    */
    int UnloadData(const char* filename, char* current_var);

    /// Prints random words in output file
    int SaySomething();


    /// Differentiates complex functions
    /**
        \param [in] node_to_diff    Node to be differented
        \param [in] how_to_diff_complex     Pointer to the fuction to diff this node
    */
    //Node* ComplexFunc(Node* node_to_diff, Node* (Diff::*how_to_diff_complex)(Node* node_to_diff));
    Node* ComplexFunc(Diff& d, Node* node_to_diff, Node* (*how_to_diff_complex)(Node* node_to_diff));

    /// Checks if branch contains variables
    /**
        \param [in] branch_root     Pointer to the root of the branch to be checked
    */
    bool ContainsVariables(Node* branch_root);

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

    /// Sets node to zero
    /**
        Changes node's type to CONSTANT, sets data to zero and deletes left and right

        \param [out] node           Node to be set to zero
    */
    int SetToZero(Node* node);

    /// Deletes useless left branch
    /**
        \param [out] node           Node to be simplified
    */
    int UselessLeftBranch(Node* node);

    /// Deletes useless right branch
    /**
        \param [out] node           Node to be simplified
    */
    int UselessRightBranch(Node* node);

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
