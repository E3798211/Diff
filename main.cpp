#include <iostream>
#include <stdio.h>

#include "Diff.h"
#include "DebugLib.h"

int main()
{
    Diff a(DEFAULT_INPUT, "x");

    a.dest.root = Differetiate(a, a.source.GetRoot());

    a.dest.CallGraph();
}
