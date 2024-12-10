#include "readfile.h"

void macroPartition( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie);

int returnCut(vector <RawNet> &netsOfMacros);

void macroGradient( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie, int totalIter);

void macroLegalization(vector <instance> &macros, Die topDie, Die btmDie);

void macroRotation(vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie);
