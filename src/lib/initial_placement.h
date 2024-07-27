#include <vector>
#include "readfile.h"

using namespace std;


typedef struct grid_info
{
    double dieHeight;
    double dieWidth;

    int binWidth;
    int binHeight;

    int binXnum;
    int binYnum;

}gridInfo;


void returnGridInfo(Die die, gridInfo &binInfo);

void calculatePenaltyWeight();

void firstPlacement(vector <Instance> &InstanceArray, gridInfo binInfo);

