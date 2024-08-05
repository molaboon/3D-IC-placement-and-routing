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


	// int arr[] = {0, 1, 2};
	// int arr_s = sizeof(arr) / sizeof(arr[0]);

	// pr( arr, arr_s);
	//read file part
	readTechnologyInfo(input, &NumTechnologies, TechMenu);	

	readDieInfo(input, &top_die, &bottom_die);

	readHybridTerminalInfo(input, &terminal);

	readInstanceInfo(input, &NumInstances, instances, &NumTechnologies, TechMenu);

	readNetInfo(input, &NumNets, rawnet, instances);

	returnGridInfo(top_die, binInfo);

	firstPlacement(instances, binInfo);

	// printNetInfo(NumNets, rawnet);
	// printInstanceInfo(NumInstances, instances);

	// double *lastCG = (double *)calloc( NumInstances, sizeof(double) );
	// double *nowCG = (double *)calloc( NumInstances, sizeof(double) );
	// double *lastGra = (double *)calloc( NumInstances, sizeof(double) );
	// double *nowGra = (double *)calloc( NumInstances, sizeof(double) );
	
	double gamma = 0.5 * binInfo.binWidth;

	double penaltyweight = returnPenaltyWeight(rawnet, gamma, instances, binInfo);

	printf("%lf \n", penaltyweight);
	// printf("%lf %lf %lf\n", ans, ansy, ansz);

	// printTechnologyInfo(NumTechnologies, TechMenu);
	// printDieInfo(top_die, bottom_die);
	// printHybridTerminalInfo(terminal);
	// printInstanceInfo(NumInstances, instances);


	return 0;
}
