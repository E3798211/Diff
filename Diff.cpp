#include "Diff.h"
//#include "Tree.h"


bool IsNum(const char* data)
{
    if(!strcmp(data, "0"))  return true;

    double data_value = atof(data);
    if(data_value != 0)     return true;

    return false;
}

char* ClearData(const char* data)
{
    EnterFunction();

    size_t data_len = strlen(data);

    int beg = 0;
    while(isspace(data[beg])){
        if(beg > data_len){
            SetColor(RED);
            DEBUG printf("=====   Probably data is empty   =====\n");
            SetColor(DEFAULT);

            PrintVar(data_len);
            PrintVar(beg);

            QuitFunction();
            return nullptr;
        }
        beg++;
    }

    int end = data_len;
    while(isspace(data[end - 1])){
        if(end < 0){
            SetColor(RED);
            DEBUG printf("=====   Probably data is empty   =====\n");
            SetColor(DEFAULT);

            PrintVar(data_len);
            PrintVar(end);

            QuitFunction();
            return nullptr;
        }
        end--;
    }

    PrintVar(beg);
    PrintVar(end);
    PrintVar(data);

    if(beg >= end){
        SetColor(RED);
        DEBUG printf("=====   beg >= end. What a terrible fault!   =====\n");
        SetColor(DEFAULT);

        PrintVar(beg);
        PrintVar(end);

        QuitFunction();
        return nullptr;
    }

    size_t clear_data_len = end - beg;
    char* clear_data = nullptr;
    try
    {
        clear_data = new char [clear_data_len];
    }
    catch(const std::bad_alloc& ex)
    {
        SetColor(RED);
        DEBUG printf("=====   Cannot allocate %d bytes for clear_data   =====\n", clear_data_len);
        SetColor(DEFAULT);

        PrintVar(clear_data_len);

        QuitFunction();
        return nullptr;
    }

    for(int i = 0; i < clear_data_len; i++)
        clear_data[i] = data[i + beg];
    clear_data[clear_data_len] = '\0';

    beg = 0;
    while(clear_data[beg] != '\0'){
        clear_data[beg] = tolower(clear_data[beg]);
        beg++;
    }

    PrintVar(clear_data_len);
    PrintVar(clear_data);

    QuitFunction();
    return clear_data;
}

int CallLatex(const char* filename)
{
    char cmd[1000] = "latex ";
    strcat(cmd, filename);
    system(cmd);

    strcpy(cmd, "xdg-open ");
    strcat(cmd, filename);
    int cmd_len = strlen(cmd);
    cmd[cmd_len - 3] = 'd';
    cmd[cmd_len - 2] = 'v';
    cmd[cmd_len - 1] = 'i';
    system(cmd);
}



// =================================================

/*
int Diff::Align(FILE* output, int depth)
{
    assert(output != nullptr);
    for(int i = 0; i < depth; i++)
        fprintf(output, "\t");

    return depth;
}
*/

char* Diff::FileRead(FILE* input)
{
    EnterFunction();
    PrintVar(input);

    assert(input != nullptr);

    if(fseek(input, 0, SEEK_END)){
        SetColor(RED);
        DEBUG printf("=====   Cannot set last position in file   =====\n");
        SetColor(DEFAULT);

        QuitFunction();
        return nullptr;
    }

    int file_size = ftell(input);
    PrintVar(file_size);

    if(file_size == EOF){
        SetColor(RED);
        DEBUG printf("=====   Cannot get pointer's position in   =====\n");
        SetColor(DEFAULT);

        QuitFunction();
        return nullptr;
    }

    char* data = nullptr;
    try{
        data = new char [file_size + 1];
    }
    catch(const std::bad_alloc& ex){
        SetColor(RED);
        DEBUG printf("=====   Cannot allocate %d bytes   =====\n", file_size + 1);
        SetColor(DEFAULT);

        QuitFunction();
        return nullptr;
    }

    rewind(input);
    fread(data, 1, file_size, input);
    data[file_size] = '\0';

    PrintVar(input);

    QuitFunction();
    return data;
}

int Diff::IdentifyData(char* data, char* cur_var, double* data_value, int* data_type)
{
    EnterFunction();

    assert(data != nullptr);
    assert(cur_var != nullptr);
    assert(data_value != nullptr);
    assert(data_type != nullptr);

    // =================================================

    char* clear_data = ClearData(data);
    char* clear_cur_var = ClearData(cur_var);

    // =================================================

    if(clear_data == nullptr || clear_cur_var == nullptr){
        SetColor(BLUE);
        DEBUG printf("=====   Data is wrong   =====\n");
        SetColor(DEFAULT);

        *data_value = 0;
        *data_type  = UNEXPECTED;

        return UNIDENTIFIED_DATA;
    }

    if(!strcmp(clear_data, clear_cur_var))          //return VARIABLE;
    {
        *data_value = 0;
        *data_type  = VARIABLE;

        PrintVar(*data_value);
        PrintVar(*data_type);

        return OK;
    }

    // =================================================    FIRST MACRO-SUBST

    #define OP( exp )\
        else if (!strcmp( clear_data , exp ))                               \
        {    \
            *data_value = exp##_CODE;                                       \
            *data_type  = UN_OPERATION;                                     \
                                                                            \
            return OK;                                                      \
        }

    if(0){}
    #include "UnOperations.h"
    #undef OP

    // =================================================    SECOND MACRO-SUBST

    #define OP( exp )\
        else if (!strcmp( clear_data , exp ))                               \
        {    \
            *data_value = exp##_CODE;                                       \
            *data_type  = BIN_OPERATION;                                     \
                                                                            \
            return OK;                                                      \
        }

    if(0){}
    #include "BinOperations.h"
    #undef OP

    // =================================================

    if(IsNum(clear_data))
    {
        *data_value = atof(clear_data);
        *data_type  = CONSTANT;

        PrintVar(*data_value);
        PrintVar(*data_type);

        return OK;
    }

    // If nothing fitted
    QuitFunction();
    return UNIDENTIFIED_DATA;
}

int Diff::AppendNode(char* data, int* place_in_data, Node* app_node, char* current_var)
{
    EnterFunction();

    while(data[(*place_in_data)] != '(')    (*place_in_data)++;
    while(data[(*place_in_data)] != '"')    (*place_in_data)++;

    int node_data_beg = ++(*place_in_data);
    int node_data_end = node_data_beg;
    while(data[node_data_end] != '"')       node_data_end++;

    PrintVar(node_data_end);
    PrintVar(node_data_end - node_data_beg);

    char* node_text = nullptr;
    try
    {
        node_text = new char [node_data_end - node_data_beg + 1];
    }
    catch(const std::bad_alloc& ex)
    {
        SetColor(RED);
        DEBUG printf("=====   Cannot allocate %d bytes for new node   =====\n", node_data_end - node_data_beg + 1);
        SetColor(DEFAULT);

        QuitFunction();
        return NODE_NOT_CREATED;
    }

    int i = 0;
    while((*place_in_data) != node_data_end)
        node_text[i++] = data[(*place_in_data)++];

    node_text[node_data_end - node_data_beg] = '\0';
    PrintVar(node_text);
    PrintVar(*place_in_data);

    // HERE
    // Add data type

    double data_value = 0;
    int    data_type  = 0;
    if(IdentifyData(node_text, current_var, &data_value, &data_type) == UNIDENTIFIED_DATA){
        SetColor(BLUE);
        DEBUG printf("Invalid argument \"%s\"\n", node_text);
        SetColor(DEFAULT);

        return UNIDENTIFIED_DATA;
    }

    source.SetData(app_node, data_value, data_type);

    bool left_added = false;
    while(data[(*place_in_data)] != ')'){
        if(data[(*place_in_data)] == '('){
            if(!left_added){
                // HERE
                app_node->left = CreateNode(app_node, false);
                AppendNode(data, place_in_data, app_node->left, current_var);

                left_added = true;

            }else{
                // HERE
                app_node->right = CreateNode(app_node, true);
                AppendNode(data, place_in_data, app_node->right, current_var);
            }
        }
        (*place_in_data)++;
    }

    PrintVar(*place_in_data);

    delete [] node_text;
    node_text = nullptr;

    QuitFunction();
    return OK;
}

int Diff::LoadData(const char* filename, char* current_var)
{
    EnterFunction();

    assert(filename != nullptr);

    FILE* database = fopen(filename, "r");
    if(database == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Cannot open file %s   =====\n", filename);
        SetColor(DEFAULT);

        return FILE_NOT_OPENED;
    }

    char* data = FileRead(database);
    PrintVar(data);

    int cur_pos_in_data = 0;        // Counter for AppendNode()
    AppendNode(data, &cur_pos_in_data, source.GetRoot(), current_var);

    fclose(database);

    QuitFunction();
    return OK;
}

int Diff::PrintBranch(FILE* output, Node* root_node, char* current_var, int recursion_depth)
{
    EnterFunction();

    assert(output != nullptr);
    assert(root_node != nullptr);

    PrintVar(root_node->data);
    PrintVar(root_node->data_type);

    // Checking if we printing operation, constant or variable:
    // Un-ops should be printed another way

    if(root_node->data_type == UN_OPERATION){
        PrintVar(root_node->data_type);
        // =================================================    UN-OPS MACRO-SUBST

        #define OP( exp )\
            else if ( exp##_CODE == root_node->data )                           \
            {                                                                   \
                fprintf(output, "{%s(", exp );                                  \
            }

        if(0){}
        #include "UnOperations.h"
        #undef OP

        // =================================================

        if(root_node->left != nullptr)
            PrintBranch(output, root_node->left, current_var, recursion_depth + 1);

        fprintf(output, ")}");

    }else if (root_node->data_type == BIN_OPERATION){

        fprintf(output, "{(");

        if(root_node->left != nullptr)
            PrintBranch(output, root_node->left, current_var, recursion_depth + 1);

        // =================================================    BIN-OPS MACRO-SUBST

        #define OP( exp )\
            else if ( exp##_CODE == root_node->data )                           \
            {                                                                   \
                fprintf(output, "%s", exp );                                    \
            }

        if(0){}
        #include "BinOperations.h"
        #undef OP

        // =================================================

        if(root_node->right != nullptr)
            PrintBranch(output, root_node->right, current_var, recursion_depth + 1);

        fprintf(output, ")}");

    }else if (root_node->data_type == VARIABLE){
        // Varriables do not have arguments but they are still printed. Uncomment brackets in case of faults.

        //fprintf(output, "(");

        fprintf(output, "%s", current_var);

        if(root_node->left != nullptr)
            PrintBranch(output, root_node->left, current_var, recursion_depth + 1);

        if(root_node->right != nullptr)
            PrintBranch(output, root_node->right, current_var, recursion_depth + 1);

        //fprintf(output, ")");

    }else{
        // Here are constants and something I have forgot about if such things exist
        // Constants do not have arguments but they are still printed. Uncomment brackets in case of faults.

        //fprintf(output, "(");

        if(root_node->left != nullptr)
            PrintBranch(output, root_node->left, current_var, recursion_depth + 1);

        fprintf(output, "{%lg}", root_node->data);

        if(root_node->right != nullptr)
            PrintBranch(output, root_node->right, current_var, recursion_depth + 1);

        //fprintf(output, ")");
    }

    QuitFunction();

    return OK;
}

int Diff::UnloadData(const char* filename, char* current_var)
{
    EnterFunction();

    assert(filename != nullptr);

    FILE* output = fopen(filename, "w");
    if(output == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Cannot open file %s   =====\n", filename);
        SetColor(DEFAULT);

        return FILE_NOT_OPENED;
    }

    fprintf(output, "\\documentclass{article}\n\\begin{document}\n\n$$\n\t");
    //PrintBranch(output, source.GetRoot(), current_var);
    PrintBranch(output, dest.GetRoot(), current_var);
    fprintf(output, "\n$$\n\n\\end{document}\n");

    fclose(output);

    QuitFunction();
    return OK;
}


// =================================================

    Diff::Diff(const char* filename, char* current_var)
{
    EnterFunction();

    strcpy(variable, current_var);

    LoadData(filename? filename : DEFAULT_INPUT, current_var);

    QuitFunction();
}

    Diff::~Diff()
{
    EnterFunction();

    /*
    if(differentiated_successfully){
        UnloadData(DEFAULT_OUTPUT, variable);

        source.CallGraph();
        CallLatex(DEFAULT_OUTPUT);
    }else{
        SetColor(BLUE);
        printf("Differentiating failed\n");
        SetColor(DEFAULT);
    }
    */

    UnloadData(DEFAULT_OUTPUT, variable);

    dest.CallGraph();
    //source.CallGraph();
    CallLatex(DEFAULT_OUTPUT);

    QuitFunction();
}

// =================================================    BRAIN

#define dL *Differetiate(d, node_to_diff->left)
#define dR *Differetiate(d, node_to_diff->right)
#define L  *Copy(d, node_to_diff->left)
#define R  *Copy(d, node_to_diff->right)

Node* diffSIN(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* cos = CreateNode();
    if(cos == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }

    cos->data_type  = UN_OPERATION;
    cos->data       = COS_CODE;
    cos->left       = &L;

    QuitFunction();
    return cos;
}

Node* diffCOS(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* sin = CreateNode();
    if(sin == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }
    Node* multiplier = CreateNode();
    if(multiplier == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }

    sin->data_type          = UN_OPERATION;
    sin->data               = SIN_CODE;
    sin->left               = &L;

    multiplier->data_type   = CONSTANT;
    multiplier->data        = -1;

    QuitFunction();
    return (*sin) * (*multiplier);
}

Node* diffTAN(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* cos = CreateNode();
    if(cos == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }
    Node* minus_1 = CreateNode();
    if(minus_1 == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }
    Node* const_2 = CreateNode();
    if(const_2 == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }

    cos->data_type          = UN_OPERATION;
    cos->data               = SIN_CODE;
    cos->left               = &L;

    minus_1->data_type   = CONSTANT;
    minus_1->data        = -1;

    const_2->data_type   = CONSTANT;
    const_2->data        = 2;


    Node* cos_2 = (*cos) ^ (*const_2);

    QuitFunction();
    return (*cos_2) ^ (*minus_1);
}

Node* diffLN (Diff& d, Node* node_to_diff)
{
    Node* const_1 = CreateNode();
    if(const_1 == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }
    const_1->data_type = CONSTANT;
    const_1->data      = 1;

    return (*const_1) / L;
}

Node* diffEXP(Diff& d, Node* node_to_diff)
{
    return Copy(d, node_to_diff);
}




Node* diffSUM(Diff& d, Node* node_to_diff)
{
    return dL + dR;
}

Node* diffSUB(Diff& d, Node* node_to_diff)
{
    return dL - dR;
}

Node* diffMUL(Diff& d, Node* node_to_diff)
{
    return *(dL * R) + *(dR * L);
}

Node* diffDIV(Diff& d, Node* node_to_diff)
{
    Node* sub = *(dL * R) - *(dR * L);

    return (*sub) / (*(R * R));
}

Node* diffPOW(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* new_pow = CreateNode();
    if(new_pow == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }
    Node* pow_mult = CreateNode();
    if(pow_mult == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created   =====\n");
        SetColor(DEFAULT);

        return nullptr;
    }

    new_pow->data_type  = CONSTANT;
    new_pow->data       = node_to_diff->right->data - 1;

    pow_mult->data_type = CONSTANT;
    pow_mult->data      = node_to_diff->right->data;

    QuitFunction();
    return *((*pow_mult) * *(L ^ (*new_pow))) * (dL);
}







Node* ComplexFunc(Diff& d, Node* node_to_diff, Node* (*how_to_diff_complex)(Diff& d, Node* node_to_diff))
{
    return (*how_to_diff_complex(d, node_to_diff)) * (*Differetiate(d, node_to_diff->left));
}

Node* Copy(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* new_node = CreateNode();
    if(new_node == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Node was not created    =====\n");
        SetColor(DEFAULT);

        QuitFunction();
        return nullptr;
    }

    new_node->data_type = node_to_diff->data_type;
    new_node->data      = node_to_diff->data;
    if(node_to_diff->left  != nullptr)
        new_node->left  = &L;
    if(node_to_diff->right != nullptr)
        new_node->right = &R;

    QuitFunction();
    return new_node;
}



Node* Differetiate(Diff& d, Node* source_root_node)
{
    EnterFunction();

    assert(source_root_node != nullptr);

    // =====================================

    switch (source_root_node->data_type)
    {
        case UN_OPERATION:
        {
            // Check first if next is variable or func
            if(d.source.IsLast(source_root_node)){
                SetColor(BLUE);
                printf("Operation without parameters cannot be processed.\n");
                SetColor(DEFAULT);

                d.differentiated_successfully = false;

                QuitFunction();
                return nullptr;
            }else{
                QuitFunction();


                #define OP( exp )                                               \
                    else if (exp##_CODE == source_root_node->data){             \
                        QuitFunction();                                         \
                        return ComplexFunc(d, source_root_node, diff##exp);     \
                    }
                if(0){}
                #include "UnOperations.h"
                #undef OP( exp )
            }
            break;
        }
        case BIN_OPERATION:
        {
            // Check first if next is variable or func
            if(d.source.IsLast(source_root_node)){
                SetColor(BLUE);
                printf("Operation without parameters cannot be processed.\n");
                SetColor(DEFAULT);

                d.differentiated_successfully = false;

                QuitFunction();
                return nullptr;
            }else{
                QuitFunction();


                #define OP( exp )                                               \
                    else if (exp##_CODE == source_root_node->data){             \
                        QuitFunction();                                         \
                        return diff##exp(d, source_root_node);                  \
                    }
                if(0){}
                #include "BinOperations.h"
                #undef OP( exp )
            }
            break;
        }
        case VARIABLE:
        {
            // When we see variable with arguments, cry
            if(!d.source.IsLast(source_root_node)){
                SetColor(BLUE);
                printf("Unexpected argument after variable.\n");
                SetColor(DEFAULT);

                d.differentiated_successfully = false;

                QuitFunction();
                return nullptr;
            }else{
                Node* new_node = CreateNode();
                new_node->data_type = CONSTANT;
                new_node->data      = 1;


                d.differentiated_successfully = true;

                QuitFunction();
                return new_node;
            }
            break;
        }
        case CONSTANT:
        {
            // When we see constant with arguments, cry
            if(!d.source.IsLast(source_root_node)){
                SetColor(BLUE);
                printf("Unexpected argument after constant.\n");
                SetColor(DEFAULT);

                d.differentiated_successfully = false;

                QuitFunction();
                return nullptr;
            }else{
                Node* new_node = CreateNode();
                new_node->data_type = CONSTANT;
                new_node->data      = 0;


                d.differentiated_successfully = true;

                QuitFunction();
                return new_node;
            }
            break;
        }
        default:
        {
            SetColor(BLUE);
            printf("Strange root detected... data type = %d\n", source_root_node->data_type);
            SetColor(DEFAULT);

            d.differentiated_successfully = false;
        }
    }

    //d.dest.CallGraph();
}


