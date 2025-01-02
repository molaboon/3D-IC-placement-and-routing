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


void returnGridInfo(Die *die, gridInfo *binInfo, int Numinstance, vector <instance> &instances);

void firstPlacement(vector <instance> &instances, gridInfo binInfo, Die topDie);

void stdCellFirstPlacement( vector <instance> &instances, vector <instance> &macros, gridInfo binInfo, Die topDie) ;

double returnPenaltyWeight(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo);


