#include "readfile.h"




double scoreOfX( const vector <RawNet> rawNet, const double gamma);

double scoreOfY( const vector <RawNet> rawNet, const double gamma);

double TSVofNet( const vector <RawNet> rawNet, const double gamma);

double bellShapeFunc( const double z, const double layer);

double RSum( const double z);

double returnBz(const double z, const double layer);

double returnPsi( const double z);

double scoreOfz( const vector <RawNet> rawNet, int *bin);

void penaltyInfoOfinstance( const Instance instance, const double density, const gridInfo binInfo);

void calculatePenaltyArea(int *coordinate, int *length, double *bins, double density, int row, Instance instance, gridInfo binInfo);

double scoreOfPenalty(int *bins, int binSize, gridInfo binInfo);

double infaltionRatio(Instance instance, double routingOverflow);

double infaltionRatio(Instance instance, double routingOverflow);

double returnAlpha(int *CG);


