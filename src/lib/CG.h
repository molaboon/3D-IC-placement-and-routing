#include "readfile.h"




double scoreOfX( vector <RawNet> rawNet, double gamma);

double scoreOfY( vector <RawNet> rawNet, double gamma);

double TSVofNet(vector <RawNet> rawNet, double gamma);

double bellShapeFunc(double z, double layer);

double RSum(double z);

double scoreOfDensity(double z, double layer);

double returnPsi(double z);

double scoreOfz( vector <RawNet> rawNet);

void penaltyInfoOfinstance(Instance instance, double density, gridInfo binInfo);

void calculatePenaltyArea(int *coordinate, int *length, double *bins, double density, int row, Instance instance, gridInfo binInfo);

double scoreOfPenalty(int *bins, int binSize, gridInfo binInfo);

