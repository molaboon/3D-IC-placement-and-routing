#include <vector>
#include "readfile.h"

using namespace std;


typedef struct grid_info
{
    double dieHeight;
    double dieWidth;

    double binWidth;
    double binHeight;

    double binXnum;
    double binYnum;

}gridInfo;


void returnGridInfo(Die &die, gridInfo &binInfo);

void calculatePenaltyWeight();

void firstPlacement(vector <Instance> &InstanceArray, gridInfo binInfo);

