#include <stdio.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <iostream>

#include "lib/readfile.h"
#include "lib/initial_placement.h"
#include "lib/CG.h"
#include "lib/coarsening.h"
#include "lib/partition.h"
#include "lib/legalization.h"

// double start_time;

using namespace std;
using std::vector;

int main(int argc, char *argv[]){
	
	// start_time = omp_get_wtime();
	// srand(time(NULL));
	// char *outputName = *(argv + 2);
	
	char *inputName = *(argv + 1);
	FILE *input = fopen(inputName, "r");
	assert(input);

	Die top_die, bottom_die;												//store the die information
	Hybrid_terminal terminal;												//store the size of the hybrid bond terminal connect between two dies
	// int NumTerminal;														//cut size calculated by shmetis
	int NumTechnologies;													//TA and TB
	vector <Tech_menu> TechMenu;											//The detail of the library of the standardcell by different technology
	
	int numInstances;														//How many instances need to be placeed in the two dies
	vector <instance> instances;										    //The standard cell with its library
	
	int NumNets;															//How many nets connect betweem Instances
	vector <RawNet> rawnet;													//The rawnet data store in input
 	// vector <Net> NetArray;													//NetArray store how many cell connects to this net and a list of cell which connects to this net
	gridInfo binInfo;														// bin infomation(bin w/h, num of bin)

	vector <instance> macros;
	vector <instance*> netsOfMacros;
 
	/*	read data	*/
	readTechnologyInfo(input, &NumTechnologies, TechMenu);	
	readDieInfo(input, &top_die, &bottom_die);
	readHybridTerminalInfo(input, &terminal);
	readInstanceInfo(input, &numInstances, instances, &NumTechnologies, TechMenu, macros);
	readNetInfo(input, &NumNets, rawnet, instances, macros, netsOfMacros);
	returnGridInfo(top_die, binInfo, numInstances);

	/*	macro partition and placement	*/
	
	// macroPartition( macros, netsOfMacros);


	/*	coarsening */
	// cout << "h" << endl;

	// coarsen(rawnet, instances);

	/*	first placement and CG preprocessing	*/
	
	double gamma, penaltyWeight, totalScore = 0.0, newScore = 0.0;
	double wireLength, newWireLength;
	double lastCG[numInstances * 3 ] = {0.0};
	double nowCG[numInstances * 3 ] = {0.0};
	double lastGra[numInstances * 3 ] = {0.0};
	double nowGra[numInstances * 3 ] = {0.0};

	firstPlacement(instances, binInfo);
	gamma = 0.05 * binInfo.dieWidth;

	penaltyWeight = returnPenaltyWeight(rawnet, gamma, instances, binInfo);

	totalScore = returnTotalScore(rawnet, gamma, binInfo, penaltyWeight, instances);

	CGandGraPreprocessing(instances, nowGra, nowCG, lastGra, lastCG);

	/*	std. cell Coarsening	*/
	// cout << "here" <<endl;

	// coarsen(rawnet, instances);

	/*	Refinement(CG)	*/

	int totalIter = numInstances;

	for(int i = 0; i < totalIter; i++)
	{
		for(int j = 0; j < 30; j++)
		{
			conjugateGradient(nowGra, nowCG, lastCG, lastGra, numInstances, i);

			newScore = newSolution(rawnet, instances, penaltyWeight, gamma, nowCG, binInfo);

			updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, penaltyWeight);

			cout << newScore << endl;

			if( newScore < totalScore)
				totalScore = newScore;

			else
			{
				// for(int k = 0; k < numInstances; k++)
				// {
				// 	cout << instances[k].instIndex << ": "<< instances[k].x <<" " << instances[k].y << " " << instances[k].z << endl;
				// }
				cout << "next iter\n\n";
		
				break;
			}
				
		}
		penaltyWeight *=2;
	}


	cell2BestLayer(instances, numInstances, top_die, bottom_die);
	place2BestRow(instances, numInstances, top_die, bottom_die);
	// writeFile()
	return 0;
}

// printNetInfo(NumNets, rawnet);
// printInstanceInfo(numInstances, instances);
// printTechnologyInfo(NumTechnologies, TechMenu);
// printDieInfo(top_die, bottom_die);
// printHybridTerminalInfo(terminal);
// printInstanceInfo(numInstances, instances);