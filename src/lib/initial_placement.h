#include <vector>
#include "readfile.h"

using namespace std;


typedef struct grid_info
{
    float dieHeight;
    float dieWidth;

    float binWidth;
    float binHeight;

    float binXnum;
    float binYnum;

    int Numinstance;

    bool startStdCellPlacement;

}gridInfo;


void returnGridInfo(Die *die, gridInfo *binInfo, int Numinstance, vector <instance> &instances);

void firstPlacement(vector <instance> &instances, gridInfo binInfo, Die topDie);

void stdCellFirstPlacement( vector <instance> &instances, vector <instance> &macros, gridInfo binInfo, Die topDie) ;

float returnPenaltyWeight(vector <RawNet> &rawNet, const float gamma, vector <instance> &instances, gridInfo binInfo, float *densityMap);


