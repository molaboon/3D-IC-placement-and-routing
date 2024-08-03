#include <stdio.h>
#include <vector>
#include <cmath>

#include "initial_placement.h"
#include "CG.h"
#include "readfile.h"

#define eta 0.1
#define total_layer 2
#define alpha 10
// #define h 0.00001

using namespace std;

/*
    check list

        score of x
        score of y
        score of z 
        create bins


*/

double scoreOfX( const vector <RawNet> rawNet, const double gamma)
{
    double score = 0.0;

    for (int net = 0; net < rawNet.size(); net++)
    {
        double numerator_1 = 0.0;

        double denominator_1 = 0.0;

        double numerator_2 = 0.0;

        double denominator_2 = 0.0;


        for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
        {

            double tmp = rawNet[net].Connection[instance]->x;

            numerator_1 += tmp * exp(tmp / gamma);
            
            denominator_1 += exp(tmp / gamma);

            numerator_2 += tmp * exp(-tmp/ gamma);
            
            denominator_2 += exp(-tmp/ gamma);
        }

        score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;

    }

    return score;

}

double scoreOfY( const vector <RawNet> rawNet, const double gamma)
{
    double score = 0.0;

    for (int net = 0; net < rawNet.size(); net++)
    {
        double numerator_1 = 0.0;
        
        double denominator_1 = 0.0;
        
        double numerator_2 = 0.0;

        double denominator_2 = 0.0;


        for (int instance = 0 ; instance < rawNet[net].Connection.size(); instance++)
        {

            double tmp = rawNet[net].Connection[instance]->y;

            numerator_1 += tmp * exp(tmp / gamma);
            
            denominator_1 += exp(tmp / gamma);

            numerator_2 += tmp * exp(-tmp/ gamma);
            
            denominator_2 += exp(-tmp/ gamma);
        }

        score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;
    }

    return score;
}

double bellShapeFunc( const double z, const double layer)
{     
    double distance = abs(z - layer);

    if (distance <= 0.5) 
        return 1.0 - 2.0 * (distance * distance);

    else if (distance > 0.5 && distance <= 1) 
        return 2.0 * ((distance - 1.0) * (distance - 1.0));
    
    else 
        return 0.0;
}

double RSum( const double z){
    
    double rsum = 0.0 ;
    
    // double eta = 0.1 ;

    for (int i = 0 ; i < total_layer; i++)
    {
        double l = (double) i;
        
        double tmp = bellShapeFunc(z, l);

        rsum += exp( tmp / eta);
    }
    return rsum;
}

double returnDensity(const double z, const double layer)
{
    double Rz, Bz, BzRSum;

    Rz = bellShapeFunc(z, layer);

    Bz = exp(Rz/ eta);

    BzRSum = RSum(z);

    Bz = Bz / BzRSum;

    return Bz;

}

double returnPsi(double z)
{
    double psi = 0;
    
    for(int i = 0; i < total_layer; i++)
    {
        double score ;

        score = (double)i * returnDensity(z, (double)i);

        psi += score;
    }

    return psi;
}

double TSVofNet( const vector <RawNet> rawNet, const double gamma)
{
    double score = 0.0;

    for (int net = 0; net < rawNet.size(); net++)
    {
        double numerator_1 = 0.0;
        
        double denominator_1 = 0.0;
        
        double numerator_2 = 0.0;

        double denominator_2 = 0.0;


        for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
        {

            double tmpPsi = returnPsi(rawNet[net].Connection[instance]->z);

            numerator_1 += tmpPsi * exp(tmpPsi / gamma);
            
            denominator_1 += exp(tmpPsi / gamma);

            numerator_2 += tmpPsi * exp(-tmpPsi/ gamma);
            
            denominator_2 += exp(-tmpPsi/ gamma);
        }

        score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;

    }

    return score;
}

double scoreOfz( vector <RawNet> rawNets, double *firstLayer, double *secondLayer, vector<Instance> &instances, gridInfo binInfo)
{
    double score = 0;
    double h = 0.05;

    //  claculate score of penalty

    for(int i = 0; i < instances.size(); i++)
    {
        double tmpD = returnDensity(instances[i].z, 0.0);

        instances[i].density = tmpD;

        penaltyInfoOfinstance(instances[i], instances[i].density, binInfo, firstLayer, secondLayer);
    }

    // calculate score of TSV

    for ( int i = 0; i < rawNets.size(); i++)
    {   
        score += TSVofNet(rawNets, h);
    }
    
    score += scoreOfPenalty(firstLayer, secondLayer, binInfo);

    return score;
}

double *createBins(gridInfo binInfo)
{
    double *bins;

    bins = (double *)calloc( binInfo.binXnum * binInfo.binYnum, sizeof(double) );

    return bins;
}

void penaltyInfoOfinstance( const Instance instance, const double density, const gridInfo binInfo, double *firstLayer, double *secondLayer)
{
    int row = 0;

    int leftXnum, rightXnum, topYnum, btmYnum;

    int inflate_leftXnum, inflate_rightXnum, inflate_topYnum, inflate_btmYnum;

    double coordinate[4] = { 0.0 };

    int inflateCoordinate[4] = { 0 };

    int length[4] = {0};

    int inflateLength[4] = {0};

    row = (int) binInfo.binXnum;

    double leftX = instance.x - (instance.width * 0.5);

    double rightX = instance.x + (instance.width * 0.5);

    double topY = instance.y - (instance.height * 0.5);

    double btmY = instance.y + (instance.height * 0.5);

    // double inflateLeftX = instance.x - (instance.inflateWidth * 0.5);
    // double inflateRightX = instance.x + (instance.inflateWidth * 0.5);
    // double inflateTopY = instance.y - (instance.inflateHeight * 0.5);
    // double inflateBtmY = instance.y + (instance.inflateHeight * 0.5);

    leftXnum = (int) (leftX / binInfo.binWidth);

    rightXnum = (int) (rightX / binInfo.binWidth) ; 

    topYnum = (int) (topY / binInfo.binHeight);

    btmYnum =  (int) (btmY / binInfo.binHeight);

    coordinate[0] = leftX;
    
    coordinate[1] = rightX;
    
    coordinate[2] = topY;
    
    coordinate[3] = btmY;

    length[0] = leftXnum;

    length[1] = rightXnum;

    length[2] = topYnum;

    length[3] = btmYnum;

    // printf("%d %d %d %d\n", length[0], length[1], length[2], length[3]);

    calculatePenaltyArea( coordinate, length, firstLayer, secondLayer, instance.density, row, instance, binInfo);

    // if_left_x_num = int( if_left_x // grid_info[2] )
    // if_right_x_num = int( if_right_x // grid_info[2])
    // if_top_y_num = int( if_top_y // grid_info[3] )
    // if_btm_y_num = int( if_btm_y // grid_info[3])     
    // if_coordinate = [if_left_x, if_right_x , if_top_y, if_btm_y]
    // if_length = [if_left_x_num, if_right_x_num, if_top_y_num, if_btm_y_num ]

}

void calculatePenaltyArea( double coordinate[], int *length, double *firstLayer, double *secondLayer, double density, int row, Instance instance, gridInfo binInfo)
{
   
    double routing_overflow;

    // grid_info = [die width, die height, grid width, grid height] 

    for (int y = 0; y <= length[3] - length[2]; ++y) {

        double y_length, x_length;
        
        if (length[3] == length[2]) 
            y_length = coordinate[3] - coordinate[2];

        else if (y == 0)  // top bin
            y_length = binInfo.binHeight - fmod(coordinate[2], binInfo.binHeight);
    
        else if (y == length[3] - length[2])  // btm bin
            y_length = fmod(coordinate[3], binInfo.binHeight);
    
        else
            y_length = binInfo.binHeight;

        for (int x = 0; x <= length[1] - length[0]; ++x) {
    
            if (length[1] == length[0]) {
    
                x_length = coordinate[1] - coordinate[0];
    
            } else if (x == 0) { // left bin
    
                x_length = binInfo.binWidth - fmod(coordinate[0], binInfo.binWidth);
    
            } else if (x == length[1] - length[0]) { // right bin
    
                x_length = fmod(coordinate[1], binInfo.binWidth);
    
            } else { // other bins
    
                x_length = binInfo.binWidth;
            }

            int bin_index = (length[0] + x) + (row * (length[2] + y));

            // printf("length0:%d x:%d row:%d y:%d length:%d yl:%lf xl:%lf\n", length[0], x,  row, y, length[2], y_length, x_length);
            firstLayer[bin_index] += y_length * x_length * density ;

            secondLayer[bin_index] += y_length * x_length * (1.0 - density) ;
            
            // routing_overflow = ((x_length + y_length) / 2.0) * y_length * x_length;
        }
    }

    // instance.inflate_ratio = inflation_ratio(instance, routing_overflow / (instance.width * instance.height));
    // instance.inflate_ratio = 1;
}

double scoreOfPenalty(double *firstLayer, double *secondLayer, gridInfo binInfo)
{
    double score = 0.0;
    double area = binInfo.binWidth * binInfo.binHeight;
    int binSize = (int) binInfo.binXnum * binInfo.binYnum;

    for(int bin = 0; bin < binSize; bin++)
        score +=  (firstLayer[bin] - area) * (firstLayer[bin] - area) + (secondLayer[bin] - area) * (secondLayer[bin] - area);
     
    return score;    
}

double gradientX(vector <RawNet> rawNet, const double gamma, vector <Instance> &instances, gridInfo binInfo, const double penaltyWeight, const double xScore, const double penaltyScore, const double densitySocre)
{
    double h = 0.00001;

    for(int i = 0; i < instances.size(); i++)
    {
        double *firstLayer = createBins(binInfo);

        double *secondLayer = createBins(binInfo);

        double tmpx = instances[i].x;

        double score2;

        instances[i].x += h;

        score2 = scoreOfX(rawNet, gamma);

        for(int j = 0; j < instances.size(); j++)
            penaltyInfoOfinstance(instances[j], instances[j].density, binInfo, firstLayer, secondLayer);

        score2 += scoreOfPenalty(firstLayer, secondLayer, binInfo);

        instances[i].gra_x = ( (score2 - xScore) + penaltyWeight * (densitySocre - score2 ) ) / h;

        instances[i].x = tmpx;

        free(firstLayer);
        free(secondLayer);
    }

}

double gradientY(vector <RawNet> rawNet, const double gamma, vector <Instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore, const double densitySocre)
{
    double h = 0.00001;

    for(int i = 0; i < instances.size(); i++)
    {
        double *firstLayer = createBins(binInfo);

        double *secondLayer = createBins(binInfo);

        double tmpy = instances[i].y;

        double score2;

        instances[i].y += h;

        score2 = scoreOfY(rawNet, gamma);

        for(int j = 0; j < instances.size(); j++)
            penaltyInfoOfinstance(instances[j], instances[j].density, binInfo, firstLayer, secondLayer);

        score2 += scoreOfPenalty(firstLayer, secondLayer, binInfo);

        instances[i].gra_y = ( (score2 - yScore) + penaltyWeight * (densitySocre - score2 ) ) / h;

        instances[i].y = tmpy;

        free(firstLayer);
        free(secondLayer);
    }

}

double gradientZ(vector <RawNet> rawNet, const double gamma, vector <Instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore, const double densitySocre)
{
    double h = 0.00001;

    for(int i = 0; i < instances.size(); i++)
    {
        double *firstLayer = createBins(binInfo);

        double *secondLayer = createBins(binInfo);

        double tmpz = instances[i].z;

        double tmpD = instances[i].density;

        double score2;

        instances[i].z += h;

        score2 = scoreOfz(rawNet, firstLayer, secondLayer, instances, binInfo);

        for(int j = 0; j < instances.size(); j++)

            penaltyInfoOfinstance(instances[j], instances[j].density, binInfo, firstLayer, secondLayer);

        score2 += scoreOfPenalty(firstLayer, secondLayer, binInfo);

        instances[i].gra_z = ( (score2 - yScore) + penaltyWeight * (densitySocre - score2 ) ) / h;

        instances[i].z = tmpz;

        instances[i].density = tmpD;

        free(firstLayer);
        free(secondLayer);
    }

}
// def gradient_of_z(nets, gamma, instances):
//     h = 1e-5
//     score1 = calculate_num_TSV(nets, gamma)
//     for  instance in instances:
//         tmpz = dc(instance.z)
//         instance.z += h
//         score2 = calculate_num_TSV(nets, gamma)
//         instance.gradientz += (score2 - score1) / h
//         instance.z = dc(tmpz)

// def gradient_of_density(instances, score_of_density, die_width, die_height, grid_size, penalty_weight):
//     h = 1e-5
//     for instance in instances:
//         tmpz = dc(instance.z)
//         bin = new_bin(grid_size)
//         instance.z += h
//         for ins in instances:
//             density = calculate_score_of_Density(ins.z, 0) # no inflate
//             calculate_penalty(ins, density, bin, grid_size)
//         score2 = penlaty_score(bin, grid_size)

//         instance.gradientd += ( penalty_weight * (score2 - score_of_density) ) / h
//         instance.z = dc(tmpz)
/*
double infaltionRatio(Instance instance, double routingOverflow)
{   
    return 0.0;
}

double returnAlpha(int *CG)
{
    return 0.0;
}

double returnTotalScore(vector<RawNet> rawNet, double gamma, int *bins, gridInfo binInfo, double penaltyWeight)
{
    double score_of_x, score_of_y, score_of_z, densityScore, totalScore;

    score_of_x = scoreOfX(rawNet, gamma);

    score_of_y = scoreOfY(rawNet, gamma);

    // score_of_z = scoreOfz(rawNet, bins);

    int binSize = sizeof(bins) / sizeof(bins[1]);

    densityScore = scoreOfPenalty(bins, binSize, binInfo);

    totalScore = score_of_x + score_of_y + score_of_z * alpha + densityScore * penaltyWeight;

    return totalScore;
}

void CGandGraPreprocessing( vector <Instance> instance, int *tmpGra, int *tmpCG)
{
    // for(int i = 0; i < instance.size(); i++)
    // {
    //     tmpGra[ 3 * i] = instance[i].gra_x;

    //     tmpGra[ 3 * i + 1] = instance[i].gra_y;

    //     tmpGra[ 3 * i + 2] = instance[i].gra_z;

    //     tmpCG[ 3 * i] = -instance[i].gra_x;

    //     tmpCG[ 3 * i + 1] = -instance[i].gra_y;

    //     tmpCG[ 3 * i + 2] = -instance[i].gra_z;
    // }

    // for cell in cells:
        
    //     tmp_gra = []
        
    //     tmp_cg = []
        
    //     tmp_gra.append ( cell.gradientx )
                  
    //     tmp_gra.append ( cell.gradienty )

    //     tmp_gra.append ( cell.gradientz + cell.gradientd )
       
    //     tmp_cg.append ( -cell.gradientx )

    //     tmp_cg.append ( -cell.gradienty )

    //     tmp_cg.append ( -(cell.gradientz + cell.gradientd) )
        
    //     gradient.append(tmp_gra)
        
    //     cg.append(tmp_cg)
        

}

*/