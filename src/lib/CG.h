#include "readfile.h"

double scoreOfX(const vector <RawNet> &rawNet, const double gamma, const bool isGra);

double scoreOfY(const vector <RawNet> &rawNet, const double gamma, const bool isGra);

double TSVofNet( vector <RawNet> &rawNet);

double bellShapeFunc(const double z, const double layer);

double RSum(const double z);

double returnDensity(const double z, const double layer);

double returnPsi(const double z);

double scoreOfz( vector <RawNet> &rawNet, vector <instance> &instances, gridInfo binInfo, bool zisChanged);

double *createBins(gridInfo binInfo);

void penaltyInfoOfinstance(const instance instance, const gridInfo binInfo, double *firstLayer, double *secondLayer, bool isGra, bool needMinus);

void calculatePenaltyArea(double coordinate[], int *length, double *firstLayer, double *secondLayer, double density, int row, instance instance, gridInfo binInfo, bool needMinus);

double scoreOfPenalty(double *firstLayer, double *secondLayer, gridInfo binInfo);

void gradientX(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double xScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer);

void gradientY(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer);

void gradientZ(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer);

double returnTotalScore(vector<RawNet> &rawNet, const double gamma, const gridInfo binInfo, const double penaltyWeight, vector <instance> &instances);

void CGandGraPreprocessing( vector <instance> &instances, double *nowGra, double *nowCG, double *lastGra, double *lastCG);

void conjugateGradient(double *nowGra, double *nowCG, double *lastCG, double *lastGra, int Numinstance, int iteration);

double returnAlpha(double nowCG[]);

void glodenSearch(instance &inst, gridInfo binInfo);

double newSolution(vector <RawNet> &rawNet, vector<instance> &instances, double penaltyWeight, double gamma, double *nowCG, grid_info binInfo);

void updateGra(vector <RawNet> &rawNet, double gamma, vector<instance> &instances, grid_info &binInfo, double *lastGra, double *nowGra, double &penaltyWeight);

// double infaltionRatio(instance instance, double routingOverflow);


