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

void firstPlacement(vector <Instance> &instances, gridInfo binInfo);

void returnPenaltyWeight(vector <RawNet> rawNet, vector<Instance> instances, grid_info binInfo, double gamma);


