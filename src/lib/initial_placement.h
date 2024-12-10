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

    int Numinstance;

    bool startStdCellPlacement;

}gridInfo;


void returnGridInfo(Die *die, gridInfo *binInfo, int Numinstance);

void firstPlacement(vector <instance> &instances, gridInfo binInfo, Die topDie);

double returnPenaltyWeight(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo);


