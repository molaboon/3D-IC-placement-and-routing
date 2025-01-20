#include "readfile.h"

double scoreOfX( const vector <RawNet> &rawNet, const double gamma, const bool isGra, instance graInstance, double originScore);

double scoreOfY( const vector <RawNet> &rawNet, const double gamma, const bool isGra, instance graInstance, double originScore);

double TSVofNet( vector <RawNet> &rawNet, bool isGra, instance graInstance, double originScore);

double bellShapeFunc(const double z, const double layer);

double RSum(const double z);

double returnDensity(const double z, const double *densityMap);

double returnDensityInDensityMap(const double z, const double layer);

double returnPsi(const double z);

double scoreOfz( vector <RawNet> &rawNet, vector <instance> &instances, gridInfo binInfo, bool zisChanged, double *dnesityMap);

double *createBins(gridInfo binInfo);

void penaltyInfoOfinstance(const instance instance, const gridInfo binInfo, double *firstLayer, double *secondLayer, bool isGra, bool needMinus, double *graGrade);

void calculatePenaltyArea(double coordinate[], int *length, double *firstLayer, double *secondLayer, int row, instance instance, gridInfo binInfo, bool needMinus, double *grade);

double scoreOfPenalty(double *firstLayer, double *secondLayer, gridInfo binInfo);

void gradientX(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double xScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer);

void gradientY(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer);

void gradientZ(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer, double *dnesityMap);

double returnTotalScore(vector<RawNet> &rawNet, const double gamma, const gridInfo binInfo, const double penaltyWeight, vector <instance> &instances, double *densityMap);

void CGandGraPreprocessing( vector <instance> &instances, double *nowGra, double *nowCG, double *lastGra, double *lastCG);

void conjugateGradient(double *nowGra, double *nowCG, double *lastCG, double *lastGra, int Numinstance, int iteration);

double returnAlpha(double nowCG[]);

void glodenSearch(instance &inst, gridInfo binInfo);

void newSolution(vector<instance> &instances, double *nowCG, grid_info binInfo);

void updateGra(vector <RawNet> &rawNet, double gamma, vector<instance> &instances, grid_info &binInfo, double *lastGra, double *nowGra, double *lastCG, double *nowCG,double &penaltyWeight, double *dnesityMap);

void returnDensityMap(double *densityMap);
// double infaltionRatio(instance instance, double routingOverflow);
double calcLipschitz(double *lastRefSolution, double *nowRefSolution, double *lastGra, double *nowGra, int numOfStdCell);

