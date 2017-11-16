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

    Diff a(DEFAULT_INPUT);
    int x = a.IdentifyData("+", "y");
    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << x << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << std::endl;

    //a.tree.CallGraph();
}
