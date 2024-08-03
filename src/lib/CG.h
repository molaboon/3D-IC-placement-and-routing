#include "readfile.h"




double scoreOfX( const vector <RawNet> rawNet, const double gamma);

double scoreOfY( const vector <RawNet> rawNet, const double gamma);

double TSVofNet( const vector <RawNet> rawNet, const double gamma);

double bellShapeFunc( const double z, const double layer);

double RSum( const double z);

double returnDensity(const double z, const double layer);

double returnPsi( const double z);

double scoreOfz( vector <RawNet> rawNets, double *firstLayer, double *secondLayer, vector<Instance> &instances, gridInfo binInfo);

double *createBins(gridInfo binInfo);

void penaltyInfoOfinstance( const Instance instance, const double density, const gridInfo binInfo, double *firstLayer, double *secondLayer);

void calculatePenaltyArea( double coordinate[], int *length, double *firstLayer, double *secondLayer, double density, int row, Instance instance, gridInfo binInfo);

double scoreOfPenalty(double *firstLayer, double *secondLayer, gridInfo binInfo);

// double infaltionRatio(Instance instance, double routingOverflow);

// double infaltionRatio(Instance instance, double routingOverflow);

// double returnAlpha(int *CG);


