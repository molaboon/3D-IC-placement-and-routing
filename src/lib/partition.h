#include "readfile.h"

void macroPartition( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie);

int returnCut(vector <RawNet> &netsOfMacros);

void macroGradient( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie, int totalIter, float *densityMap, vector <instance> fillers);

void macroLegalization(vector <instance> &macros, Die topDie, Die btmDie);

void updatePinsInMacroInfo( vector<instance> &macro, vector < vector<instance> > &pinsInMacros, vector <instance> &instances);

void finalUpdatePinsInMacro(vector<instance> &macro, vector < vector<instance> > &pinsInMacros, vector<instance> &instances);

void updateRotate(instance &macro);