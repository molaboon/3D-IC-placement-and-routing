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

double returnPenaltyWeight(vector <RawNet> rawNet, const double gamma, vector <Instance> &instances, gridInfo binInfo);


