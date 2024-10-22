#include "readfile.h"


typedef struct _terminal{
    int netID;
    int x;
    int y;

}terminal;

void cell2BestLayer( vector <instance> &instances, const int numInstances, const Die topDie, const Die btmDie);

void place2BestRow( vector <instance> &instances, const int numInstances, Die topDie, Die btmDie, vector <instance> &macros);

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet);

void writeFile(const vector <instance> instances, char *outputFile, const vector <RawNet> rawNet, const int numInstances, const vector <terminal> terminals);

void insertTerminal(const vector <instance> instances, const vector <RawNet> rawNet,  vector <terminal> &terminals, Hybrid_terminal terminalTech, Die topDie);
