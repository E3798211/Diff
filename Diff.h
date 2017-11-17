#ifndef DIFF_H_INCLUDED
#define DIFF_H_INCLUDED

#include <ctype.h>

#include "Tree.h"

// =================================================

const char  DEFAULT_INPUT[] = "datadef.txt";

// =================================================

enum EQUATION_PARTS {
     VARIABLE = 1,
     CONSTANT = 2,
    OPERATION = 3,

    UNEXPECTED
};

enum OPERATION_CODES {
    #define OP( exp ) exp##_CODE ,

    #include "Operations.h"

    #undef OP
    MAX_CODE
};

const char  SUM[] = "+";
const char  SUB[] = "-";
const char MULT[] = "*";
const char  DIV[] = "/";

const char  SIN[] = "sin";
const char  COS[] = "cos";
const char  TAN[] = "tan";
const char CTAN[] = "ctan";

const char ASIN[] = "asin";
const char ACOS[] = "acos";
const char ATAN[] = "atan";
const char ACTN[] = "actn";

const char  POW[] = "^";
const char   LN[] = "ln";

// =================================================

class Diff {
//private:
public:
    /// Main tree
    Tree tree;

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

public:
    /// Constructor
    /**
        \param [in] filename        Name of the file with data
        \param [in] current_var     Current variable
    */
    Diff(const char* filename, char* current_var);

    /// Default destructor
    ~Diff();
};


/// "Clears" data
/**
    Deletes spaces and turnes every letter to lower case

    \param [in, out] data           Data to be cleared
*/
char* ClearData(const char* data);





#endif // DIFF_H_INCLUDED
