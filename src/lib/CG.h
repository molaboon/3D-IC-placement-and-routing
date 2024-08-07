#include "readfile.h"

double scoreOfX(const vector <RawNet> rawNet, const double gamma);

double scoreOfY(const vector <RawNet> rawNet, const double gamma);

double TSVofNet(const vector <RawNet> rawNet);

double bellShapeFunc(const double z, const double layer);

double RSum(const double z);

double returnDensity(const double z, const double layer);

double returnPsi(const double z);

double scoreOfz( vector <RawNet> rawNets, vector<Instance> &instances, gridInfo binInfo, int zisChanged);

double *createBins(gridInfo binInfo);

void penaltyInfoOfinstance(const Instance instance, const double density, const gridInfo binInfo, double *firstLayer, double *secondLayer);

void calculatePenaltyArea(double coordinate[], int *length, double *firstLayer, double *secondLayer, double density, int row, Instance instance, gridInfo binInfo);

double scoreOfPenalty(double *firstLayer, double *secondLayer, gridInfo binInfo);

void gradientX(vector <RawNet> rawNet, const double gamma, vector <Instance> &instances, gridInfo binInfo, const double penaltyWeight, const double xScore, const double penaltyScore);

void gradientY(vector <RawNet> rawNet, const double gamma, vector <Instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore);

void gradientZ(vector <RawNet> rawNet, const double gamma, vector <Instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore);

double returnTotalScore(vector<RawNet> rawNet, const double gamma, const gridInfo binInfo, const double penaltyWeight, vector <Instance> &instances);

void CGandGraPreprocessing( vector <Instance> instances, double *nowGra, double *nowCG, double *lastGra, double *lastCG);

double returnAlpha(double nowCG[]);

void newSolution(vector <RawNet> rawNets, vector<Instance> &instances, double penaltyWeight, double gamma, double *nowCG);

// double infaltionRatio(Instance instance, double routingOverflow);

// double infaltionRatio(Instance instance, double routingOverflow);


