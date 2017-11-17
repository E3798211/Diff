#include <iostream>
#include <stdio.h>

#include "Diff.h"
#include "DebugLib.h"

int main()
{
/*
    Tree a;
    a.SetData(a.GetRoot(), 555, 111);
    a.GetRoot()->left  = a.CreateNode(a.GetRoot(), false);
    a.GetRoot()->right = a.CreateNode(a.GetRoot(), true);

    a.SetData(a.GetRoot()->left, 111, 123);
    a.SetData(a.GetRoot()->right, 222, 321);

    //a.CallGraph();
*/

    Diff a(DEFAULT_INPUT, "x");
    //double value = 0;
    //int type = 0;
    //int x = a.IdentifyData(" 345.5 ", "y", &value, &type);
    //std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << value << "\n" << type  << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << std::endl;

    a.tree.CallGraph();
}
