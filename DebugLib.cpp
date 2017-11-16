#include "DebugLib.h"

bool IN = false;

void SetColor(const char* color)
{
    std::cout << color << "";
}

void Enter(const char* func_name)
{
    SetColor(YELLOW);
    std::cout << "\nEntered " << func_name << "():\n" << std::endl;
    SetColor(DEFAULT);
    IN = true;
}

void Quit (const char* func_name)
{
    SetColor(YELLOW);
    std::cout << "\nQuited " << func_name << "():" << std::endl;
    SetColor(DEFAULT);
    IN = false;
}
