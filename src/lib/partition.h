#include "readfile.h"

void macroPartition( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie);

int returnCut(vector <RawNet> &netsOfMacros);

void macroGradient( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie);

void macroLegalization(vector <instance> &macros);

void macroRotation(vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie);
