#include "Diff.h"


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
    while(data[beg] != '\0')        tolower(data[beg++]);

    PrintVar(clear_data_len);
    PrintVar(clear_data);

    QuitFunction();
    return clear_data;
}






int Diff::Align(FILE* output, int depth)
{
    assert(output != nullptr);
    for(int i = 0; i < depth; i++)
        fprintf(output, "\t");

    return depth;
}

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

int Diff::IdentifyData(char* data, char* cur_var)
{
    EnterFunction();

    assert(data != nullptr);
    assert(cur_var != nullptr);

    // =================================================

    char* clear_data = ClearData(data);
    char* clear_cur_var = ClearData(cur_var);

    // =================================================
    //
    // Macro-subst here
    //

    if(!strcmp(clear_data, clear_cur_var))          return VARIABLE;

    #define OP( exp )\
        else if (!strcmp( clear_data , exp ))       return OPERATION;
    if(0){}

    #include "Operations.h"

    #undef OP

    if(IsNum(clear_data))                           return CONSTANT;

    // If nothing fitted
    QuitFunction();
    return UNEXPECTED;
}

int Diff::AppendNode(char* data, int* place_in_data, Node* app_node)
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

    double node_num = atof(node_text);

    // HERE
    // Add data type

    tree.SetData(app_node, node_num, 0);

    bool left_added = false;
    while(data[(*place_in_data)] != ')'){
        if(data[(*place_in_data)] == '('){
            if(!left_added){
                // HERE
                app_node->left = tree.CreateNode(app_node, false);
                AppendNode(data, place_in_data, app_node->left);

                left_added = true;

            }else{
                // HERE
                app_node->right = tree.CreateNode(app_node, true);
                AppendNode(data, place_in_data, app_node->right);
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

int Diff::LoadData(const char* filename)
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
    AppendNode(data, &cur_pos_in_data, tree.GetRoot());

    fclose(database);

    QuitFunction();
    return OK;
}

// =================================================

    Diff::Diff(const char* filename)
{
    EnterFunction();

    LoadData(filename? filename : DEFAULT_INPUT);

    QuitFunction();
}

    Diff::~Diff()
{
    EnterFunction();
    QuitFunction();
}
