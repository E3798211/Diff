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
        \warning data and cur_var will be changed if spaces would be found in the beginning and the end

        \param [in] data            Data you want to identify
        \param [in] cur_var         Current varriable
    */
    int IdentifyData(char* data, char* cur_var);

    /// Appends node with data from file
    /**
        \param [in] data            String with data
        \param [in] app_node        Pointer to the node to be appended
        \param [in] place_in_data   Place in data where to look for data
    */
    int AppendNode(char* data, int* place_in_data, Node* app_node);

    /// Loads data from database and creates tree
    /**
        \param [in] filename        Name of the data file
    */
    int LoadData(const char* filename);

public:
    /// Constructor
    /**
        \param [in] filename        Name of the file with data
    */
    Diff(const char* filename);

    /// Default destructor
    ~Diff();
};


char* ClearData(const char* data);





#endif // DIFF_H_INCLUDED
