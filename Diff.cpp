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

// Old PrintBranch()
/*
int Diff::PrintBranch(/*FILE* output, Node* root_node, char* current_var, int recursion_depth)
{
    EnterFunction();

    assert(output != nullptr);
    assert(root_node != nullptr);

    PrintVar(root_node);
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
                fprintf(output, "{\\%s(", exp );                                  \
            }

        if(0){}
        #include "UnOperations.h"
        #undef OP

        // =================================================

        if(root_node->left != nullptr)
            PrintBranch(/*output, root_node->left, current_var, recursion_depth + 1);

        fprintf(output, ")}");

    }else if (root_node->data_type == BIN_OPERATION){

// =================================
/*
        fprintf(output, "{(");

        if(root_node->left != nullptr)
            PrintBranch(/*output,*/ /* root_node->left, current_var, recursion_depth + 1);

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
            PrintBranch(/*output, *//* root_node->right, current_var, recursion_depth + 1);

        fprintf(output, ")}");

// =================================

        fprintf(output, "{(");

        if(root_node->left != nullptr)
            PrintBranch(/*output, root_node->left, current_var, recursion_depth + 1);

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
            PrintBranch(/*output, * root_node->right, current_var, recursion_depth + 1);

        fprintf(output, ")}");


    }else if (root_node->data_type == VARIABLE){
        // Varriables do not have arguments but they are still printed. Uncomment brackets in case of faults.

        //fprintf(output, "(");

        fprintf(output, "%s", current_var);

        if(root_node->left != nullptr)
            PrintBranch(/*output, *root_node->left, current_var, recursion_depth + 1);

        if(root_node->right != nullptr)
            PrintBranch(/*output, *root_node->right, current_var, recursion_depth + 1);

        //fprintf(output, ")");

    }else{
        // Here are constants and something I have forgot about if such things exist
        // Constants do not have arguments but they are still printed. Uncomment brackets in case of faults.

        //fprintf(output, "(");

        if(root_node->left != nullptr)
            PrintBranch(/*output,* root_node->left, current_var, recursion_depth + 1);

        fprintf(output, "{%lg}", root_node->data);

        if(root_node->right != nullptr)
            PrintBranch(/*output, *root_node->right, current_var, recursion_depth + 1);

        //fprintf(output, ")");
    }

    QuitFunction();

    return OK;
}*/




int Diff::PrintBranch(Node* root_node, char* current_var, int recursion_depth, int prev_priority)
{
    EnterFunction();

    assert(output != nullptr);
    assert(root_node != nullptr);

    PrintVar(root_node);
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
                fprintf(output, "{\\%s(", exp );                                \
            }

        if(0){}
        #include "UnOperations.h"
        #undef OP

        // =================================================

        if(root_node->left != nullptr)
            PrintBranch(root_node->left, current_var, recursion_depth + 1);

        fprintf(output, ")}");

    }else if (root_node->data_type == BIN_OPERATION){

        int data = root_node->data;
        switch (data){
            case SUM_CODE :{
                if(prev_priority > PRI_FOURTH)
                    fprintf(output, "{\\left(");

                if(root_node->left != nullptr)
                    PrintBranch(root_node->left, current_var, recursion_depth + 1, PRI_FOURTH);

                fprintf(output, "%s", SUM);

                if(root_node->right!= nullptr)
                    PrintBranch(root_node->right, current_var, recursion_depth + 1, PRI_FOURTH);

                if(prev_priority > PRI_FOURTH)
                    fprintf(output, "\\right)}");
                break;
            }
            case SUB_CODE :{
                if(prev_priority > PRI_FOURTH)
                    fprintf(output, "{\\left(");

                if(root_node->left != nullptr)
                    PrintBranch(root_node->left, current_var, recursion_depth + 1, PRI_FOURTH);

                fprintf(output, "%s", SUB);

                if(root_node->right!= nullptr)
                    PrintBranch(root_node->right, current_var, recursion_depth + 1, PRI_FOURTH);

                if(prev_priority > PRI_FOURTH)
                    fprintf(output, "\\right)}");
                break;
            }
            case MUL_CODE :{
                if(prev_priority > PRI_THIRD)
                    fprintf(output, "{\\left(");

                if(root_node->left != nullptr)
                    PrintBranch(root_node->left, current_var, recursion_depth + 1, PRI_THIRD);

                fprintf(output, "\\cdot ");

                if(root_node->right!= nullptr)
                    PrintBranch(root_node->right, current_var, recursion_depth + 1, PRI_THIRD);

                if(prev_priority > PRI_THIRD)
                    fprintf(output, "\\right)}");
                break;
            }
            case DIV_CODE :{
                if(prev_priority > PRI_THIRD)
                    fprintf(output, "{\\left(");

                fprintf(output, "\\frac{");

                if(root_node->left != nullptr)
                    PrintBranch(root_node->left, current_var, recursion_depth + 1, PRI_THIRD);

                fprintf(output, "}{");

                if(root_node->right!= nullptr)
                    PrintBranch(root_node->right, current_var, recursion_depth + 1, PRI_THIRD);

                fprintf(output, "}");

                if(prev_priority > PRI_THIRD)
                    fprintf(output, "\\right)}");
                break;
            }
            case POW_CODE :{
                if(prev_priority == PRI_FIRST)
                    fprintf(output, "{");

                if(root_node->left != nullptr)
                    PrintBranch(root_node->left, current_var, recursion_depth + 1, PRI_FIRST);
                else fprintf(output, "");

                fprintf(output, "^{", POW);

                if(root_node->right!= nullptr)
                    PrintBranch(root_node->right, current_var, recursion_depth + 1, PRI_FIRST);

                fprintf(output, "}");

                if(prev_priority == PRI_FIRST)
                    fprintf(output, "}");
                break;
            }
        }

    }else if (root_node->data_type == VARIABLE){
        // Varriables do not have arguments but they are still printed. Uncomment brackets in case of faults.

        //fprintf(output, "(");

        fprintf(output, "%s", current_var);

        if(root_node->left != nullptr)
            PrintBranch(root_node->left, current_var, recursion_depth + 1);

        if(root_node->right != nullptr)
            PrintBranch(root_node->right, current_var, recursion_depth + 1);

        //fprintf(output, ")");

    }else{
        // Here are constants and something I have forgot about if such things exist
        // Constants do not have arguments but they are still printed. Uncomment brackets in case of faults.

        //fprintf(output, "(");

        if(root_node->left != nullptr)
            PrintBranch( root_node->left, current_var, recursion_depth + 1);

        fprintf(output, "{%lg}", root_node->data);

        if(root_node->right != nullptr)
            PrintBranch(root_node->right, current_var, recursion_depth + 1);

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
    PrintBranch(/*output,*/ dest.GetRoot(), current_var);
    fprintf(output, "\n$$\n\n\\end{document}\n");

    /*
    fprintf(output, "\\documentclass{article}\n\\begin{document}\n\n\\begin{eqnarray}\n\t");
    //PrintBranch(output, source.GetRoot(), current_var);
    PrintBranch(output, dest.GetRoot(), current_var);
    fprintf(output, "\n\\end{eqnarray}\n\n\\end{document}\n");
    */

    fclose(output);

    QuitFunction();
    return OK;
}

int Diff::SaySomething()
{
    EnterFunction();

    srand(seed++);

    fprintf(output, "%s ",  FIRST[rand() % N_FIRST].c_str());
    fprintf(output, "%s ",  SECOND[rand() % N_SECOND].c_str());
    fprintf(output, "%s\n", THIRD[rand() % N_THIRD].c_str());

    PrintVar(dest.GetRoot());
    PrintVar(dest.GetRoot()->data);
    PrintVar(dest.GetRoot()->data_type);
    PrintVar(dest.GetRoot()->left);
    PrintVar(dest.GetRoot()->right);

    QuitFunction();
    return OK;
}

// =================================================

    Diff::Diff(const char* filename, char* current_var)
{
    EnterFunction();

    strcpy(variable, current_var);

    LoadData(filename? filename : DEFAULT_INPUT, current_var);


    // FOR LAB GEN
    output = fopen("outdef.txt", "w");
    if(output == nullptr){
        SetColor(RED);
        DEBUG printf("=====   Cannot open file %s   =====\n", filename);
        SetColor(DEFAULT);
    }

    fprintf(output, "\\documentclass{article}\n\n\\usepackage[utf8x]{inputenc}\n\n\\usepackage[russian]{babel}\n\n\
    \\usepackage{geometry}\n\\geometry{left=2cm}\n\\geometry{right=1.5cm}\n\\begin{document}\
    \
    В эфире ваша любимая рубрика \"Матан и мазохизм\". Сегодня давайте возьмем производную от этого чудесного выражения:\n\n");
    fprintf(output, "$$\n");
    PrintBranch(source.GetRoot(), variable);
    fprintf(output, "\n$$\n\\newpage\nЧто ж, приступим! Будем есть слона по кусочкам.\n");

    srand(time(0));
    seed = rand();

    // FOR LAB

    QuitFunction();
}

    Diff::~Diff()
{
    EnterFunction();

    if(differentiated_successfully){
        //UnloadData(DEFAULT_OUTPUT, variable);

        //PrintBranch(dest.GetRoot(), "x");
        fprintf(output, "\n\\vspace{1cm}\nНа этом, думаю, можно закончить. Можете присылать ваши пожелания, все они будут проигнорированы\n\
        \n\\end{document}\n\\end\n");
        fclose(output);

        dest.CallGraph();
        //source.CallGraph();
        CallLatex(DEFAULT_OUTPUT);
    }else{
        SetColor(BLUE);
        printf("Differentiating failed\n");
        SetColor(DEFAULT);

        dest.CallGraph();
        CallLatex(DEFAULT_OUTPUT);
        fprintf(output, "\n\\vspace{1cm}\nНа этом, думаю, можно закончить. Можете присылать ваши пожелания, все они будут проигнорированы\n\
        \n\\end{document}\n\\end\n");
        fclose(output);
    }



    QuitFunction();
}


int Diff::TakeDiff()
{
    dest.root = Differetiate(*this, source.GetRoot());

    Optimize();
    //a.dest.CallGraph();

    return OK;
}

// =================================================    BRAIN

#define dL *Differetiate(d, node_to_diff->left)
#define dR *Differetiate(d, node_to_diff->right)
#define L  *Copy(d, node_to_diff->left)
#define R  *Copy(d, node_to_diff->right)

#define NEW( name )                                             \
    CreateNode();                                               \
    if( name == nullptr){                                       \
        SetColor(RED);                                          \
        DEBUG printf("=====   Node was not created   =====\n"); \
        SetColor(DEFAULT);                                      \
                                                                \
        return nullptr;                                         \
    }

bool Diff::ContainsVariables(Node* branch_root)
{
    EnterFunction();

    if(branch_root == nullptr){
        QuitFunction();
        return false;
    }

    bool  left_contains = ContainsVariables(branch_root->left);
    bool right_contains = ContainsVariables(branch_root->right);

    if(left_contains || right_contains || branch_root->data_type == VARIABLE){
        QuitFunction();
        return true;
    }

    QuitFunction();
    return false;
}

Node* ComplexFunc(Diff& d, Node* node_to_diff, Node* (*how_to_diff_complex)(Diff& d, Node* node_to_diff))
{
    return (*how_to_diff_complex(d, node_to_diff)) * (*Differetiate(d, node_to_diff->left));
}



Node* diffSIN(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* cos = NEW(cos);

    cos->data_type  = UN_OPERATION;
    cos->data       = COS_CODE;
    cos->left       = &L;

    d.SaySomething();
    fprintf(d.output, "$$\n\t");
    d.PrintBranch(node_to_diff, "x");
    fprintf(d.output, "~\\Rightarrow~");
    d.PrintBranch(cos, "x");
    fprintf(d.output, "\n$$\n");

    QuitFunction();
    return cos;
}

Node* diffCOS(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* sin = NEW(sin);
    Node* multiplier = NEW(multiplier);

    sin->data_type          = UN_OPERATION;
    sin->data               = SIN_CODE;
    sin->left               = &L;

    multiplier->data_type   = CONSTANT;
    multiplier->data        = -1;

    Node* new_node = (*sin) * (*multiplier);

    d.SaySomething();
    fprintf(d.output, "$$\n\t");
    d.PrintBranch(node_to_diff, "x");
    fprintf(d.output, "~\\Rightarrow~");
    d.PrintBranch(new_node, "x");
    fprintf(d.output, "\n$$\n");

    QuitFunction();

    return new_node;
}

Node* diffTAN(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* cos = NEW(cos);
    Node* minus_1 = NEW(minus_1);
    Node* const_2 = NEW(const_2);

    cos->data_type          = UN_OPERATION;
    cos->data               = SIN_CODE;
    cos->left               = &L;

    minus_1->data_type   = CONSTANT;
    minus_1->data        = -1;

    const_2->data_type   = CONSTANT;
    const_2->data        = 2;


    Node* cos_2 = (*cos) ^ (*const_2);

    Node* new_node = (*cos_2) ^ (*minus_1);

    d.SaySomething();
    fprintf(d.output, "$$\n\t");
    d.PrintBranch(node_to_diff, "x");
    fprintf(d.output, "~\\Rightarrow~");
    d.PrintBranch(new_node, "x");
    fprintf(d.output, "\n$$\n");

    QuitFunction();

    return new_node;
}

Node* diffLN (Diff& d, Node* node_to_diff)
{
    Node* const_1 = NEW(const_1);
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

    if(!d.ContainsVariables(node_to_diff->right)){
        //Node* new_pow   = NEW(new_pow);
        Node* pow_mult  = NEW(pow_mult);
        Node* const_1  =  NEW(const_1);

        /*
        new_pow->data_type  = CONSTANT;
        new_pow->data       = node_to_diff->right->data - 1;
        */

        const_1->data_type  = CONSTANT;
        const_1->data       = 1;

        Node* new_pow = *(Copy(d, node_to_diff->right)) - *(const_1);

        pow_mult->data_type = CONSTANT;
        pow_mult->data      = node_to_diff->right->data;

        QuitFunction();
        return *((*pow_mult) * *(L ^ (*new_pow))) * (dL);

    }else{
        // =====================


        printf("\n\n\n\n\n\t\t\t\t\tSUKA\n\n\n\n\n");

        Node* new_ln        = NEW(new_ln);

        new_ln->data_type   = UN_OPERATION;
        new_ln->data        = LN_CODE;

        new_ln->left        = Copy(d, node_to_diff->left);

        Node* lnf_g = (*new_ln) * R;

        QuitFunction();
        return (*Copy(d, node_to_diff)) * (*(Differetiate(d, lnf_g)));
    }
}




Node* Copy(Diff& d, Node* node_to_diff)
{
    EnterFunction();

    Node* new_node = NEW(new_node);

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

    if(!d.differentiated_successfully)  return nullptr;

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
                        Node* new_node = ComplexFunc(d, source_root_node, diff##exp);   \
                        d.SaySomething();                                       \
                        fprintf(d.output, "$$\n\t");                            \
                        d.PrintBranch(source_root_node, "x");                   \
                        fprintf(d.output, "~\\Rightarrow~");                     \
                        d.PrintBranch(new_node, "x");                           \
                        fprintf(d.output, "\n$$\n");                            \
                                                                                \
                        QuitFunction();                                         \
                        return  new_node;                                       \
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
                        Node* new_node = diff##exp(d, source_root_node);        \
                        d.SaySomething();                                       \
                        fprintf(d.output, "$$\n\t");                            \
                        d.PrintBranch(source_root_node, "x");                   \
                        fprintf(d.output, "~\\Rightarrow~");                     \
                        d.PrintBranch(new_node, "x");                           \
                        fprintf(d.output, "\n$$\n");                            \
                                                                                \
                        QuitFunction();                                         \
                        return new_node;                                        \
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

                d.SaySomething();
                fprintf(d.output, "$$\n\t");
                d.PrintBranch(source_root_node, "x");
                fprintf(d.output, "~\\Rightarrow~");
                d.PrintBranch(new_node, "x");
                fprintf(d.output, "\n$$\n");

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

                d.SaySomething();
                fprintf(d.output, "$$\n\t");
                d.PrintBranch(source_root_node, "x");
                fprintf(d.output, "~\\Rightarrow~");
                d.PrintBranch(new_node, "x");
                fprintf(d.output, "\n$$\n");

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


// =================================================    OPTIMIZATION

#define NodeIsConst (branch_root->data_type == CONSTANT)
#define NodeIsVar   (branch_root->data_type == VARIABLE)
#define NodeIsBinOp (branch_root->data_type == BIN_OPERATION)
#define NodeIsUnOp  (branch_root->data_type == UN_OPERATION)

int Diff::SetToZero(Node* node)
{
    EnterFunction();

    node->data_type = CONSTANT;
    node->data = 0;

    if(node->left != nullptr)
        DeleteBranch(node->left);
    node->left  = nullptr;

    if(node->right != nullptr)
        DeleteBranch(node->right);
    node->right = nullptr;

    QuitFunction();
    return OK;
}

int Diff::UselessLeftBranch(Node* node)
{
    EnterFunction();

    node->data_type = node->right->data_type;
    node->data      = node->right->data;

    Node* tmp_left  = node->right->left;
    Node* tmp_right = node->right->right;

    DeleteBranch(node->left);
    delete node->right;

    node->left  = tmp_left;
    node->right = tmp_right;

    QuitFunction();
    return OK;
}

int Diff::UselessRightBranch(Node* node)
{
    EnterFunction();

    node->data_type = node->left->data_type;
    node->data      = node->left->data;

    Node* tmp_left  = node->left->left;
    Node* tmp_right = node->left->right;

    DeleteBranch(node->right);
    delete node->left;

    node->left  = tmp_left;
    node->right = tmp_right;

    QuitFunction();
    return OK;
}



int Diff::DeleteObvious (Node* branch_root)
{
    EnterFunction();

    if(branch_root == nullptr){
        QuitFunction();
        return OK;
    }

    DeleteObvious(branch_root->left);
    DeleteObvious(branch_root->right);

    // =====================================

    if(NodeIsBinOp){

        //diff_optimized++;

        int data = branch_root->data;
        PrintVar(data);

        switch (data)
        {
            MARK
            case SUM_CODE:
            {
                      if(branch_root->left ->data_type == CONSTANT){
                    if(branch_root->left ->data == 0){
                        UselessLeftBranch (branch_root);
                        diff_optimized++;
                    }
                }else if(branch_root->right->data_type == CONSTANT){
                    if(branch_root->right->data == 0){
                        UselessRightBranch(branch_root);
                        diff_optimized++;
                    }
                }
                break;
            }
            case SUB_CODE:
            {
                if(branch_root->right->data_type == CONSTANT){
                    if(branch_root->right->data == 0){
                        UselessRightBranch(branch_root);
                        diff_optimized++;
                    }
                }
                break;
            }
            case MUL_CODE:
            {
                if(branch_root->left ->data_type == CONSTANT){
                          if(branch_root->left ->data == 0){
                        SetToZero(branch_root);
                        diff_optimized++;
                    }else if(branch_root->left ->data == 1){
                        UselessLeftBranch(branch_root);
                        diff_optimized++;
                    }
                }else if(branch_root->right->data_type == CONSTANT){
                          if(branch_root->right->data == 0){
                        SetToZero(branch_root);
                        diff_optimized++;
                    }else if(branch_root->right->data == 1){
                        UselessRightBranch(branch_root);
                        diff_optimized++;
                    }
                }
                break;
            }
            case DIV_CODE:
            {
                if(branch_root->right->data_type == CONSTANT){
                    if(branch_root->right->data == 1){
                        UselessRightBranch(branch_root);
                        diff_optimized++;
                    }
                }
                break;
            }
            case POW_CODE:
            {
                if(branch_root->right->data_type == CONSTANT){
                    if(branch_root->right->data == 1){
                        UselessRightBranch(branch_root);
                        diff_optimized++;
                    }
                }
                break;
            }
        }

        QuitFunction();
        return OK;
    }else{
        QuitFunction();
        return OK;
    }
}

bool Diff::CountConstants(Node* branch_root)
{
    EnterFunction();

    if(!optimized_successfully) return false;

    if(NodeIsConst)             return true;
    if(NodeIsVar  )             return false;
    if(NodeIsUnOp )             return false;

    // =====================================

    bool  left_is_countable = CountConstants(branch_root->left);
    bool right_is_countable = CountConstants(branch_root->right);

    PrintVar(left_is_countable);
    PrintVar(right_is_countable);
    PrintVar(NodeIsBinOp);
    PrintVar(branch_root->data_type);

    DEBUG printf("==================== br_root = %d\n", branch_root);

    if(left_is_countable && right_is_countable && NodeIsBinOp){

        double  left_data = 0;
        double right_data = 0;

        if(branch_root->left  != nullptr)    left_data  = branch_root->left ->data;
        if(branch_root->right != nullptr)    right_data = branch_root->right->data;


        double sum = 0;
        int data_type = branch_root->data;
        switch (data_type)
            {
            case SUM_CODE:
                {
                    sum = left_data + right_data;
                    PrintVar(sum);
                    break;
                }
            case SUB_CODE:
                {
                    sum = left_data - right_data;
                    PrintVar(sum);
                    break;
                }
            case MUL_CODE:
                {
                    sum = left_data * right_data;
                    PrintVar(sum);
                    break;
                }
            case DIV_CODE:
                {
                    sum = left_data / right_data;
                    PrintVar(sum);
                    break;
                }
            case POW_CODE:
                {
                    sum = pow(left_data, right_data);
                    PrintVar(sum);
                    break;
                }
            default:
                {
                    SetColor(RED);
                    printf("=====   Unexpected problem   =====\n");
                    SetColor(DEFAULT);

                    QuitFunction();
                    return false;
                }
            }

        branch_root->data      = sum;
        branch_root->data_type = CONSTANT;

        delete branch_root->left;
        branch_root->left  = nullptr;

        delete branch_root->right;
        branch_root->right = nullptr;

        diff_optimized++;

        QuitFunction();
        return true;
    }else{
        QuitFunction();
        return false;
    }
}





int Diff::Optimize()
{
    EnterFunction();

    fprintf(output, "\n\\vspace{1cm}А теперь сделаем несколько абсолютно очевидных преобразований:\n\n\n\\noindent ");

    do{
        SaySomething();
        fprintf(output, "$$\n\t");
        PrintBranch(dest.GetRoot(), "x");
        fprintf(output, "\n$$\n");

        diff_optimized = 0;

        DeleteObvious (dest.GetRoot());
        CountConstants(dest.GetRoot());

        // optimized_successfully is changed inside DeleteObvious() and CountConstants()
        if(!optimized_successfully){
            SetColor(RED);
            printf("=====   Could not optimize derivative. Errors occured.   =====\n");
            SetColor(DEFAULT);

            QuitFunction();
            return OPTIMIZATION_FAILED;
        }


        PrintVar(diff_optimized);
        SetColor(MAGENTA);
        DEBUG printf("Loop in optimization\n");
        SetColor(DEFAULT);
    }while(diff_optimized != 0);

    QuitFunction();
    return OK;
}





