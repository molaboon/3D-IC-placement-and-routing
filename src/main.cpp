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


using namespace std;
using std::vector;

#define dimention 3

int main(int argc, char *argv[]){
	
	// start_time = omp_get_wtime();
	// srand(time(NULL));

	clock_t startTime, endTime;

	startTime = clock();
	
	char *inputName = *(argv + 1);
	// char *outputName = *(argv + 2);
	// char *visualFile = *(argv + 3);

	FILE *input = fopen(inputName, "r");
	assert(input);

	Die top_die, bottom_die;												//store the die information
	Hybrid_terminal terminalTech;												//store the size of the hybrid bond terminal connect between two dies
	// int NumTerminal;														//cut size calculated by shmetis
	int NumTechnologies;													//TA and TB
	vector <Tech_menu> TechMenu;											//The detail of the library of the standardcell by different technology
	
	int numInstances;														//How many instances need to be placeed in the two dies
	vector <instance> instances;							    //The standard cell with its library
	
	int NumNets;															//How many nets connect betweem Instances
	vector <RawNet> rawnet;													//The rawnet data store in input
 	// vector <Net> NetArray;													//NetArray store how many cell connects to this net and a list of cell which connects to this net
	gridInfo binInfo;														// bin infomation(bin w/h, num of bin)

	vector <instance> macros;
	vector <instance*> netsOfMacros;
	vector <terminal> terminals;
 
	/*	read data	*/
	readTechnologyInfo(input, &NumTechnologies, TechMenu);	
	readDieInfo(input, &top_die, &bottom_die);
	readHybridTerminalInfo(input, &terminalTech);
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

	startTime = clock();

	int totalIter = 1;

	for(int i = 0; i < totalIter; i++)
	{
		for(int j = 0; j < 1; j++)
		{
			conjugateGradient(nowGra, nowCG, lastCG, lastGra, numInstances, i);

			newScore = newSolution(rawnet, instances, penaltyWeight, gamma, nowCG, binInfo);

			updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, penaltyWeight);

			if( newScore < totalScore)
				totalScore = newScore;

			else
			{
				
				cout << "next iter\n\n";
		
				break;
			}	
		}
		penaltyWeight *= 2;
	}

	endTime = clock();

	cout <<"Time " << endTime - startTime <<endl;

	

	// cell2BestLayer(instances, numInstances, top_die, bottom_die);
	// place2BestRow(instances, numInstances, top_die, bottom_die, macros);
	// insertTerminal(instances, rawnet, terminals, terminalTech, top_die);
	// writeFile(instances, outputName, rawnet, numInstances, terminals);

	return 0;
}

// printNetInfo(NumNets, rawnet);
// printInstanceInfo(numInstances, instances);
// printTechnologyInfo(NumTechnologies, TechMenu);
// printDieInfo(top_die, bottom_die);
// printHybridTerminalInfo(terminal);
// printInstanceInfo(numInstances, instances);