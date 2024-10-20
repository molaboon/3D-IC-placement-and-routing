#include "readfile.h"


typedef struct _terminal{
    int netID;
    int x;
    int y;

}terminal;

void cell2BestLayer( vector <instance> &instances, const int numInstances, const Die topDie, const Die btmDie);

void place2BestRow( vector <instance> &instances, const int numInstances, const Die topDie, const Die btmDie);

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet);

void writeFile(const vector <instance> instances, char *outputFile, const vector <RawNet> rawNet, const int numInstances);

void insertTerminal(const vector <instance> instances, const vector <RawNet> rawNet,  vector <terminal> &terminals, Hybrid_terminal terminalTech, Die topDie);
