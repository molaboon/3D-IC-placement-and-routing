#include "readfile.h"

void cell2BestLayer( vector <instance> &instances, const int numInstances, const Die topDie, const Die btmDie);

void place2BestRow( vector <instance> &instances, const int numInstances, const Die topDie, const Die btmDie);

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet);

void writeFile(const vector <instance> instances, char *outputFile, const vector <RawNet> rawNet);