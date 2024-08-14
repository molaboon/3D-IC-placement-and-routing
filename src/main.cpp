#include <stdio.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "lib/readfile.h"
#include "lib/initial_placement.h"
#include "lib/CG.h"

// double start_time;

using namespace std;
using std::vector;

int main(int argc, char *argv[]){
	
	// start_time = omp_get_wtime();
	
	// srand(time(NULL));
	
	char *inputName = *(argv + 1);
	char *outputName = *(argv + 2);
	FILE *input = fopen(inputName, "r");
	assert(input);

	Die top_die, bottom_die;												//store the die information
	Hybrid_terminal terminal;												//store the size of the hybrid bond terminal connect between two dies
	// int NumTerminal;														//cut size calculated by shmetis
	int NumTechnologies;													//TA and TB
	vector <Tech_menu> TechMenu;											//The detail of the library of the standardcell by different technology
	int NumInstances;														//How many instances need to be placeed in the two dies
	vector <Instance> instances;										//The standard cell with its library
	int NumNets;															//How many nets connect betweem Instances
	vector <RawNet> rawnet;													//The rawnet data store in input
	vector <int> PartitionResult;											//Using hmetis to do 2-way partition to divide the Instances into to die
	vector <Net> NetArray;													//NetArray store how many cell connects to this net and a list of cell which connects to this net
	gridInfo binInfo;														// bin infomation(bin w/h, num of bin)

	/*			data preprocessing			 */

	readTechnologyInfo(input, &NumTechnologies, TechMenu);	
	readDieInfo(input, &top_die, &bottom_die);
	readHybridTerminalInfo(input, &terminal);
	readInstanceInfo(input, &NumInstances, instances, &NumTechnologies, TechMenu);
	readNetInfo(input, &NumNets, rawnet, instances);
	returnGridInfo(top_die, binInfo, NumInstances);

	/* first placement and CG preprocessing */
	
	double gamma, penaltyWeight, totalScore = 0.0, newScore = 0.0;
	double lastCG[NumInstances * 3 ] = {0.0};
	double nowCG[NumInstances * 3 ] = {0.0};
	double lastGra[NumInstances * 3 ] = {0.0};
	double nowGra[NumInstances * 3 ] = {0.0};
	
	firstPlacement(instances, binInfo);
	gamma = 0.05 * binInfo.dieWidth;

	penaltyWeight = returnPenaltyWeight(rawnet, gamma, instances, binInfo);

	totalScore = returnTotalScore(rawnet, gamma, binInfo, penaltyWeight, instances);

	CGandGraPreprocessing(instances, nowGra, nowCG, lastGra, lastCG);

	/*				Coarsening					*/

	/*				Refinement(CG)				*/

	int totalIter = NumInstances;

	for(int i = 0; i < totalIter; i++)
	{
		for(int j = 0; j < 30; j++)
		{
			conjugateGradient(nowGra, nowCG, lastCG, lastGra, NumInstances, i);

			newScore = newSolution(rawnet, instances, penaltyWeight, gamma, nowCG, binInfo);

			updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, penaltyWeight);

		}
		penaltyWeight *=2;
	}



	
	return 0;
}

// printNetInfo(NumNets, rawnet);
// printInstanceInfo(NumInstances, instances);
// printTechnologyInfo(NumTechnologies, TechMenu);
// printDieInfo(top_die, bottom_die);
// printHybridTerminalInfo(terminal);
// printInstanceInfo(NumInstances, instances);