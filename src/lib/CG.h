#include "readfile.h"

float scoreOfX( const vector <RawNet> &rawNet, const float gamma, const bool isGra, instance graInstance, float originScore);

float scoreOfY( const vector <RawNet> &rawNet, const float gamma, const bool isGra, instance graInstance, float originScore);

float TSVofNet( vector <RawNet> &rawNet, bool isGra, instance graInstance, float originScore, float *densityMap);

float bellShapeFunc(const float z, const float layer);

float RSum(const float z);

float returnDensity(const float z, const float *densityMap);

float returnDensityInDensityMap(const float z, const float layer);

float returnPsi(const float z);

float scoreOfz( vector <RawNet> &rawNet, vector <instance> &instances, gridInfo binInfo, bool zisChanged, float *dnesityMap);

float *createBins(gridInfo binInfo);

void penaltyInfoOfinstance(const instance instance, const gridInfo binInfo, float *firstLayer, float *secondLayer, bool isGra, bool needMinus, float *graGrade, const int graVariable);

void calculatePenaltyArea(float coordinate[], int *length, float *firstLayer, float *secondLayer, int row, instance instance, gridInfo binInfo, bool needMinus, float *grade);

float scoreOfPenalty(float *firstLayer, float *secondLayer, gridInfo binInfo);

void gradientX(vector <RawNet> &rawNet, const float gamma, vector <instance> &instances, gridInfo binInfo, const float penaltyWeight, const float xScore, const float penaltyScore, const float *originFirstLayer, const float *originSecondLayer);

void gradientY(vector <RawNet> &rawNet, const float gamma, vector <instance> &instances, gridInfo binInfo, const float penaltyWeight, const float yScore, const float penaltyScore, const float *originFirstLayer, const float *originSecondLayer);

void gradientZ(vector <RawNet> &rawNet, const float gamma, vector <instance> &instances, gridInfo binInfo, const float penaltyWeight, const float yScore, const float penaltyScore, const float *originFirstLayer, const float *originSecondLayer, float *dnesityMap);

float returnTotalScore(vector<RawNet> &rawNet, const float gamma, const gridInfo binInfo, const float penaltyWeight, vector <instance> &instances, float *densityMap);

void CGandGraPreprocessing( vector <instance> &instances, float *nowGra, float *nowCG, float *lastGra, float *lastCG);

void conjugateGradient(float *nowGra, float *nowCG, float *lastCG, float *lastGra, int Numinstance, int iteration);

float returnAlpha(float nowCG[]);

void glodenSearch(instance &inst, gridInfo binInfo);

void newSolution(vector<instance> &instances, float *nowCG, grid_info binInfo);

void updateGra(vector <RawNet> &rawNet, float gamma, vector<instance> &instances, grid_info &binInfo, float *lastGra, float *nowGra, float *lastCG, float *nowCG,float &penaltyWeight, float *dnesityMap);

void returnDensityMap(float *densityMap);

void clacBktrk( vector <instance> &instances, float *lastGra, float *nowGra, int iter, float *optParam, 
                vector <RawNet> &rawNets, float gamma, grid_info &binInfo, float *lastCG, float *nowCG, 
                float &penaltyWeight, float *densityMap, float *curRefSoltion, float *newRefSolution);
            
float calcLipschitz(float *lastRefSolution, float *nowRefSolution, float *lastGra, float *nowGra, int numOfStdCell);

void refPosition(vector <instance> &instances);

void fillerPreprocess(vector <instance> &filler, gridInfo binInfo, Die topDie, Die btmDie);
