#include "readfile.h"

void macroPartition( vector <instance> &macros, vector <instance*> &netsOfMacros);

int returnBestMacroLayer(double topDieArea, double btmDieArea, int cut, vector <instance> &macros, vector <instance*> &netsOfMacros, int index);