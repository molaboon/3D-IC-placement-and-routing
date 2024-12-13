#include <stdio.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <map>

#include "lib/readfile.h"
#include "lib/initial_placement.h"
#include "lib/CG.h"
#include "lib/coarsening.h"
#include "lib/partition.h"
#include "lib/legalization.h"

using namespace std;
using std::vector;

#define dimention 3
#define macroPart 1
#define stdCellPart 0

int main(int argc, char *argv[]){
	
	// start_time = omp_get_wtime();
	// srand(time(NULL));

	double startTime, endTime;
	
	char *inputName = *(argv + 1);
	char *outputName = *(argv + 2);
	char *visualFile = *(argv + 3);
	int totalIter = atoi(*(argv + 4));

	FILE *input = fopen(inputName, "r");
	assert(input);

	Die top_die, bottom_die;												//store the die information
	Hybrid_terminal terminalTech;												//store the size of the hybrid bond terminal connect between two dies
	
	int NumTechnologies;													//TA and TB
	vector <Tech_menu> *techMenuPtr = new vector<Tech_menu>;				//The detail of the library of the standardcell by different technology

	int numStdCells;														//How many instances need to be placeed in the two dies
	vector <instance> instances;							    //The standard cell with its library
	
	int NumNets;															//How many nets connect betweem Instances
	vector <RawNet> rawnet;													//The rawnet data store in input
 	gridInfo binInfo;														// bin infomation(bin w/h, num of bin)

	vector <instance> macros;
	vector <instance> stdCells;
	vector <RawNet> netsOfMacros;
	vector <int> numStdCellConncetMacro;
	vector <terminal> terminals;
	vector < vector<instance> > pinsInMacros;

	map <double, double> densityMap;
 
	/*	read data	*/
	readTechnologyInfo(input, &NumTechnologies, techMenuPtr);	
	readDieInfo(input, &top_die, &bottom_die);
	readHybridTerminalInfo(input, &terminalTech);
	readInstanceInfo(input, &numStdCells, instances, &NumTechnologies, techMenuPtr, macros, stdCells, pinsInMacros);
	delete techMenuPtr;
	
	// create a vector called "pinsInMacro". 
	// Because the macro are fixed after placement of macro, the "pinsInMacros" is stored the pin info of each macro
	readNetInfo(input, &NumNets, rawnet, instances, macros, netsOfMacros, numStdCellConncetMacro, pinsInMacros);
	returnGridInfo(&top_die, &binInfo, numStdCells);

	/*	macro gradient and placement	*/
	
	if(macroPart)
	{
		macroGradient( macros, netsOfMacros, top_die, totalIter);
		macroLegalization(macros, top_die, bottom_die);
		updatePinsInMacroInfo( macros, pinsInMacros);
		// macroPartition( macros, netsOfMacros, top_die);
	}

	/*	coarsening */

	// coarsen(rawnet, instances);

	/*	first placement and CG preprocessing	*/
	
	if (stdCellPart){

		double gamma, penaltyWeight, totalScore = 0.0, newScore = 0.0;
		double wireLength, newWireLength;

		double *lastCG = new double[ numStdCells * 3 ]{0.0};
		double *nowCG = new double[ numStdCells * 3 ]{0.0};
		double *lastGra = new double[ numStdCells * 3 ]{0.0};
		double *nowGra = new double[ numStdCells * 3 ]{0.0};

		firstPlacement(instances, binInfo, top_die);

		gamma = 0.05 * binInfo.dieWidth;
		penaltyWeight = returnPenaltyWeight(rawnet, gamma, instances, binInfo);

		totalScore = returnTotalScore(rawnet, gamma, binInfo, penaltyWeight, instances);

		CGandGraPreprocessing(instances, nowGra, nowCG, lastGra, lastCG);

		/*	std. cell Coarsening	*/
		// cout << "here" <<endl;

		// coarsen(rawnet, instances);

		/*	Refinement(CG)	*/

		startTime = clock();

		int totalIter = numStdCells ;

		for(int i = 0; i < totalIter; i++)
		{
			for(int j = 0; j < 40; j++)
			{
				conjugateGradient(nowGra, nowCG, lastCG, lastGra, numStdCells, i);

				newScore = newSolution(rawnet, instances, penaltyWeight, gamma, nowCG, binInfo);

				updatePinsInMacroInfo( macros, pinsInMacros);
				
				updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, penaltyWeight);

				double g = clock();
				printf("Time iter: %fs\n", (g - startTime) / (double) CLOCKS_PER_SEC );

				if( newScore < totalScore)
					totalScore = newScore;

				else
				{
					// cout << "next iter\n\n";
					break;
				}
				
			}
			// penaltyWeight *= 2;
		}

		endTime = clock();

		printf("Time: %fs\n", (endTime - startTime) / (double) CLOCKS_PER_SEC );

	}

	if(true)
	{	
		// firstPlacement(instances, binInfo, top_die);
		// cell2BestLayer(macros, top_die, bottom_die);
		// place2BestRow(instances, numStdCells, top_die, bottom_die, macros);
		// insertTerminal(instances, rawnet, terminals, terminalTech, top_die);
		writeVisualFile(macros, visualFile, top_die);
		// writeFile(instances, outputName, rawnet, numStdCells, terminals);
	}

	return 0;
}

// printNetInfo(NumNets, rawnet);
// printInstanceInfo(numStdCells, instances);
// printTechnologyInfo(NumTechnologies, TechMenu);
// printDieInfo(top_die, bottom_die);
// printHybridTerminalInfo(terminal);
// printInstanceInfo(numStdCells, instances);