#include "readfile.h"


typedef struct _terminal{
    int netID;
    int x;
    int y;

}terminal;

void cell2BestLayer( vector <instance> &instances, const Die topDie, const Die btmDie, vector <RawNet> &rawNets, Hybrid_terminal hbtTech);

void place2BestRow( vector <instance> &instances, const int numInstances, Die topDie, Die btmDie, vector <instance> &macros);

void writeFile(const vector <instance> &instances, const vector <RawNet> &rawNet, const int numInstances, const vector <terminal> &terminals);

void insertTerminal(const vector <instance> &instances, const vector <RawNet> &rawNet,  vector <terminal> &terminals, const Hybrid_terminal terminalTech, Die topDie);

void placeInst2BestX(const Die die, vector <vector<int>> &diePlacementState, vector <vector<int>> &dieMacroPlacementState, vector <instance> &instances, int *cellWidth);

void place2nearRow(const Die die, const Die theOtherDie,vector <vector<int>> &diePlacementState, vector <vector<int>> &theOtherDiePlacementState, vector <instance> &instances, int *dieCellWidth, int *theOtherDieCellWidth);

void writeVisualFile(const vector <instance> instances, int iteration, Die &topDie);

void macroPlace(vector <instance> &macros, Die topDie, Die btmDie);

void macroRotate(vector <instance> &macros, vector < vector<instance> > &pinsInMacros, vector <RawNet> &rawnets, vector <instance> stdCells);

void writeData(const float hpwl, const float hbt, const float penalty);

void wirteNodes(vector <instance> &instances, vector <instance> &macros);

void wirtePl(vector <instance> &instances, vector <instance> &macros, Die topDie);

void writeRow(vector <instance> &macros, Die topDie, Die btmDie);

void macroPlacement(vector <instance> &macros, vector<RawNet> &rawnets, Die topDie);

bool cooradinate(vector <instance> &macros, Die topDie, vector <RawNet> &rawnets, int rotation, int list[], int (&pinsGrade)[][4]);

int actualHPWL(vector<RawNet> &rawnet);

int decideRotation(int direction, int (&pinGrade)[][4], int index, int rotation);

void writeNet(vector <instance> &macros, vector < vector<instance> > &pinsInMacros, vector <RawNet> &rawnets, vector <instance> instances);