#include "readfile.h"


typedef struct _terminal{
    int netID;
    int x;
    int y;

}terminal;

void cell2BestLayer( vector <instance> &instances, const Die topDie, const Die btmDie);

void place2BestRow( vector <instance> &instances, const int numInstances, Die topDie, Die btmDie, vector <instance> &macros);

void writeFile(const vector <instance> instances, const vector <RawNet> rawNet, const int numInstances, const vector <terminal> terminals);

void insertTerminal(const vector <instance> &instances, vector <RawNet> &rawNet,  vector <terminal> &terminals, Hybrid_terminal terminalTech, Die topDie);

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet, vector <terminal> &terminals);

void placeInst2BestX(const Die die, vector <vector<int>> &diePlacementState, vector <vector<int>> &dieMacroPlacementState, vector <instance> &instances, int *cellWidth);

void place2nearRow(const Die die, const Die theOtherDie,vector <vector<int>> &diePlacementState, vector <vector<int>> &theOtherDiePlacementState, vector <instance> &instances, int *dieCellWidth, int *theOtherDieCellWidth);

void writeVisualFile(const vector <instance> instances, int iteration, Die &topDie);

void macroPlaceAndRotate(vector <instance> &macros, Die topDie, Die btmDie);