#include "readfile.h"




double scoreOfX( const vector <RawNet> rawNet, const double gamma);

double scoreOfY( const vector <RawNet> rawNet, const double gamma);

double TSVofNet( const vector <RawNet> rawNet, const double gamma);

double bellShapeFunc( const double z, const double layer);

double RSum( const double z);

double returnDensity(const double z, const double layer);

double returnPsi( const double z);

double scoreOfz( vector <RawNet> rawNets, int *firstLayer, int *secondLayer, vector<Instance> &instances, gridInfo binInfo);

int *createBins(gridInfo binInfo);

void penaltyInfoOfinstance( const Instance instance, const double density, const gridInfo binInfo, int *firstLayer, int *secondLayer);

void calculatePenaltyArea( double coordinate[], int *length, int *firstLayer, int *secondLayer, double density, int row, Instance instance, gridInfo binInfo);

double scoreOfPenalty(int *firstLayer, int *secondLayer, int binSize, gridInfo binInfo);

// double infaltionRatio(Instance instance, double routingOverflow);

// double infaltionRatio(Instance instance, double routingOverflow);

// double returnAlpha(int *CG);


