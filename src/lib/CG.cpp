#include <stdio.h>
#include <vector>
#include <cmath>
#include <string.h>
#include <iostream>

#include "initial_placement.h"
#include "CG.h"
#include "readfile.h"

#define eta 0.1
#define total_layer 2
#define alpha 10
#define h 0.00001
#define zChanged 1
#define znotChanged 0
#define Dimensions 3
#define spaceZweight 0.01

using namespace std;


double scoreOfX( const vector <RawNet> &rawNet, const double gamma, const bool isGra)
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
            double tmp = 0.0;

            if(isGra)
                tmp = rawNet[net].Connection[instance]->tmpX;
            else
                tmp = rawNet[net].Connection[instance]->x;

            numerator_1 += tmp * exp(tmp / gamma);
            denominator_1 += exp(tmp / gamma);
            numerator_2 += tmp * exp(-tmp/ gamma);
            denominator_2 += exp(-tmp/ gamma);
        }

        score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;
    }

    return score;

}

double scoreOfY( const vector <RawNet> &rawNet, const double gamma, const bool isGra)
{
    double score = 0.0;

    for (int net = 0; net < (int) rawNet.size(); net++)
    {
        double numerator_1 = 0.0;
        double denominator_1 = 0.0;
        double numerator_2 = 0.0;
        double denominator_2 = 0.0;

        for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
        {
            double tmp = 0.0;

            if(isGra)
                tmp = rawNet[net].Connection[instance]->tmpY;
            else
                tmp = rawNet[net].Connection[instance]->y;

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

double TSVofNet( vector <RawNet> &rawNet)
{
    double score = 0.0;
    int size = rawNet.size();

    for (int net = 0; net < size; net++)
    {
        double numerator_1 = 0.0;
        double denominator_1 = 0.0;
        double numerator_2 = 0.0;
        double denominator_2 = 0.0;

        for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
        {
            double tmpPsi = returnPsi(rawNet[net].Connection[instance]->z);

            numerator_1 += tmpPsi * exp(tmpPsi / 0.05);
            denominator_1 += exp(tmpPsi / 0.05);
            numerator_2 += tmpPsi * exp(-tmpPsi/ 0.05);
            denominator_2 += exp(-tmpPsi/ 0.05);
        }
        score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;
    }

    return score;
}

double scoreOfz( vector <RawNet> &rawNets, vector <instance> &instances, gridInfo binInfo, bool zisChanged)
{
    double score = 0.0;
    int size = instances.size();
    double *firstLayer = createBins(binInfo);
	double *secondLayer = createBins(binInfo);

    //  claculate score of penalty

    for(int i = 0; i < size; i++)
    {
        if(zisChanged)
        {
            double tmpD = returnDensity(instances[i].z, 0.0);

            instances[i].density = tmpD;
        }
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, false, false);
    }
    score = scoreOfPenalty(firstLayer, secondLayer, binInfo);

    free(firstLayer);
    free(secondLayer);
        
    return score;
}

double *createBins(gridInfo binInfo)
{
    double *bins;

    bins = (double *)calloc( binInfo.binXnum * binInfo.binYnum, sizeof( double ) );

    return bins;
}

void penaltyInfoOfinstance( const instance instance, const gridInfo binInfo, double *firstLayer, double *secondLayer, bool isGra, bool needMinus)
{
    int leftXnum, rightXnum, topYnum, btmYnum;
    int row = (int) binInfo.binXnum;

    double coordinate[4] = { 0.0 };
    int length[4] = {0};
    
    double leftX = 0, rightX = 0, topY = 0, btmY = 0;

    if(isGra)
    {
        leftX = instance.tmpX - (instance.width * 0.5);
        rightX = instance.tmpX + (instance.width * 0.5);
        topY = instance.tmpY - (instance.height * 0.5);
        btmY = instance.tmpY + (instance.height * 0.5);
    }
    else
    {
        leftX = instance.x - (instance.width * 0.5);
        rightX = instance.x + (instance.width * 0.5);
        topY = instance.y - (instance.height * 0.5);
        btmY = instance.y + (instance.height * 0.5);
    }
    
    leftXnum = (int) (leftX / binInfo.binWidth);
    rightXnum = (int) (rightX / binInfo.binWidth);
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

    if(needMinus)
        calculatePenaltyArea( coordinate, length, firstLayer, secondLayer, instance.density, row, instance, binInfo, needMinus);
    
    else
        calculatePenaltyArea( coordinate, length, firstLayer, secondLayer, instance.density, row, instance, binInfo, false);

    // if_left_x_num = int( if_left_x // grid_info[2] )
    // if_right_x_num = int( if_right_x // grid_info[2])
    // if_top_y_num = int( if_top_y // grid_info[3] )
    // if_btm_y_num = int( if_btm_y // grid_info[3])     
    // if_coordinate = [if_left_x, if_right_x , if_top_y, if_btm_y]
    // if_length = [if_left_x_num, if_right_x_num, if_top_y_num, if_btm_y_num ]
}

void calculatePenaltyArea( double coordinate[], int *length, double *firstLayer, double *secondLayer, double density, int row, instance instance, gridInfo binInfo, bool needMinus)
{   
    // double routing_overflow;

    for (int y = 0; y <= length[3] - length[2]; ++y) 
    {
        double y_length, x_length;
        if (length[3] == length[2]) 
            y_length = coordinate[3] - coordinate[2];

        else if (y == 0)  // top bin
            y_length = binInfo.binHeight - fmod(coordinate[2], binInfo.binHeight);
    
        else if (y == length[3] - length[2])  // btm bin
            y_length = fmod(coordinate[3], binInfo.binHeight);
    
        else
            y_length = binInfo.binHeight;

        for (int x = 0; x <= length[1] - length[0]; ++x) 
        {
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

            if(needMinus)
            {
                firstLayer[bin_index] -= y_length * x_length * density ;
                secondLayer[bin_index] -= y_length * x_length * (1.0 - density) ;
            }
            else
            {
                firstLayer[bin_index] += y_length * x_length * density ;
                secondLayer[bin_index] += y_length * x_length * (1.0 - density) ;
            }

            
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
    
    int binXnum = (int) binInfo.binXnum;
    int binYnum = (int) binInfo.binYnum;

    // if bin area > die area
    double overAreaOfX = 0.0;
    double overAreaOfY = 0.0;
    double overAreaOfXY = 0.0;

    if( binInfo.binXnum * binInfo.binWidth > binInfo.dieWidth )
        overAreaOfX = (binInfo.dieWidth - (binInfo.binXnum * binInfo.binWidth) ) * binInfo.binHeight;
    else
        overAreaOfX = area;
    
    if( binInfo.binYnum * binInfo.binHeight > binInfo.dieHeight )
        overAreaOfY = (binInfo.dieHeight - (binInfo.binYnum * binInfo.binHeight) ) * binInfo.binWidth;
    else
        overAreaOfY = area;
    
    if(binInfo.binYnum * binInfo.binHeight > binInfo.dieHeight && binInfo.binXnum * binInfo.binWidth > binInfo.dieWidth)
        overAreaOfXY = (binInfo.dieWidth - (binInfo.binXnum * binInfo.binWidth) ) * (binInfo.dieHeight - (binInfo.binYnum * binInfo.binHeight) );
    else 
        overAreaOfXY = area;

    for(int i = 0; i < binYnum; i++)
    {
        for(int j = 0; j < binXnum; j++)
        {
            int b = j + i*binXnum;
            
            if(j == binXnum - 1 && i == binYnum - 1)
                score += (firstLayer[b] - overAreaOfXY) * (firstLayer[b] - overAreaOfXY) + (secondLayer[b] - overAreaOfXY) * (secondLayer[b] - overAreaOfXY);

            else if(j == binXnum - 1)
                score +=  (firstLayer[b] - overAreaOfX) * (firstLayer[b] - overAreaOfX) + (secondLayer[b] - overAreaOfX) * (secondLayer[b] - overAreaOfX);
            
            else if (i == binYnum - 1)
                score +=  (firstLayer[b] - overAreaOfY) * (firstLayer[b] - overAreaOfY) + (secondLayer[b] - overAreaOfY) * (secondLayer[b] - overAreaOfY);

            else
                score +=  (firstLayer[b] - area) * (firstLayer[b] - area) + (secondLayer[b] - area) * (secondLayer[b] - area);                
        }
    }

    
    return score;    
}

void gradientX(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double xScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer)
{
    int size = instances.size();
    double *firstLayer = createBins(binInfo);
    double *secondLayer = createBins(binInfo);

    for(int i = 0; i < size; i++)
    {
        bool isGra, needMinus;
        double score = 0.0, score2 = 0.0;
        
        memcpy(firstLayer, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
        memcpy(secondLayer, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

        instances[i].tmpX = instances[i].x;
        instances[i].tmpX += h;
        
        score = scoreOfX(rawNet, gamma, isGra);

        // Dedecut the original block(needMinus = ture) first and the add the gra one(isGra = true).

        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = true);
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = false);

        score2 = scoreOfPenalty(firstLayer, secondLayer, binInfo);

        instances[i].gra_x =  (score - xScore)/h + ( penaltyWeight * ( score2 - penaltyScore ) ) / h;
    }
    free(firstLayer);
    free(secondLayer);
}

void gradientY(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer)
{
    int size = instances.size();
    
    double *firstLayer = createBins(binInfo);      
    double *secondLayer = createBins(binInfo);

    for(int i = 0; i < size; i++)
    {
        bool needMinus = false, isGra = false;
        double score = 0.0, score2 = 0.0;

        memcpy(firstLayer, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
        memcpy(secondLayer, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

        instances[i].tmpY = instances[i].y;
        instances[i].tmpY += h;

        score = scoreOfY(rawNet, gamma, true);

        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = true);
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = false);
            
        score2 = scoreOfPenalty(firstLayer, secondLayer, binInfo);

        instances[i].gra_y = ( (score - yScore) + penaltyWeight * (score2 - penaltyScore) ) / h;
    }
    
    free(firstLayer);
    free(secondLayer);
}

void gradientZ(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double zScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer)
{   
    double *fl = createBins(binInfo);
    double *sl = createBins(binInfo);

    for(int i = 0; i < instances.size(); i++)
    {
        double tmpz = instances[i].z;
        double tmpd = instances[i].density;
        double score , score2;
        bool needMinus = false;
        
        instances[i].z += h;
        instances[i].density = returnDensity(instances[i].z, 0.0);

        score = TSVofNet(rawNet);

        memcpy(fl, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
        memcpy(sl, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true);
        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, false);

        score2 = scoreOfPenalty(fl, sl, binInfo);

        instances[i].gra_z = (score - zScore) / h;
        instances[i].gra_d = penaltyWeight * ( score2 - penaltyScore) / h;

        instances[i].z = tmpz;
        instances[i].density = tmpd;
    }

    free(fl);
    free(sl);

}

double infaltionRatio(instance instance, double routingOverflow)
{   
    return 0.0;
}

double returnTotalScore(vector<RawNet> &rawNet, const double gamma, const gridInfo binInfo, const double penaltyWeight, vector <instance> &instances)
{
    double score_of_x, score_of_y, score_of_z, densityScore, totalScore, wireLength;

    score_of_x = scoreOfX(rawNet, gamma, false);

    score_of_y = scoreOfY(rawNet, gamma, false);

    densityScore = scoreOfz(rawNet, instances, binInfo, 1);

    score_of_z = TSVofNet(rawNet);

    totalScore = score_of_x + score_of_y + score_of_z * alpha + (densityScore) * penaltyWeight;

    wireLength = score_of_x + score_of_y;

    return totalScore;
}

void CGandGraPreprocessing( vector <instance> &instances, double *nowGra, double *nowCG, double *lastGra, double *lastCG)
{
    int size = instances.size();

    for(int i = 0; i < size; i++)
    {
        nowGra[i*3] = instances[i].gra_x;
        nowGra[i*3 + 1] = instances[i].gra_y;
        nowGra[i*3 + 2] = instances[i].gra_z + instances[i].gra_d;

        lastGra[i*3] = instances[i].gra_x;
        lastGra[i*3 + 1] = instances[i].gra_y;
        lastGra[i*3 + 2] = instances[i].gra_z + instances[i].gra_d;

        nowCG[i*3] = -instances[i].gra_x;
        nowCG[i*3 + 1] = -instances[i].gra_y;
        nowCG[i*3 + 2] = -(instances[i].gra_z + instances[i].gra_d);

        lastCG[i*3] = -instances[i].gra_x;
        lastCG[i*3 + 1] = -instances[i].gra_y;
        lastCG[i*3 + 2] = -(instances[i].gra_z + instances[i].gra_d);

        // printf("%lf, %lf, %lf, %lf, %lf, %lf\n", nowGra[i*3], nowGra[i*3 + 1], nowGra[i*3+2], nowCG[i*3], nowCG[i*3+1], nowCG[i*3+2]);
    }
}

void conjugateGradient(double *nowGra, double *nowCG, double *lastCG, double *lastGra, int Numinstance, int iteration)
{
    double beta = 0.0, norm = 0.0;

    for(int index = 0; index < Numinstance * Dimensions; index++)
    {
        norm += lastGra[index];
        beta += nowGra[index] * ( nowGra[index] - lastGra[index]);
    }

    norm = norm * norm;
    beta = beta/norm;

    for(int index = 0; index < Numinstance * Dimensions; index ++)
        nowCG[index] = (-nowGra[index]) + (beta * lastCG[index]);

}

double returnAlpha(double nowCG[])
{   
    double Alpha = 0.0, weight = 0.2;

    for(int i = 0; i < 3; i++)
        Alpha += nowCG[i] * nowCG[i];

    Alpha = sqrt(Alpha);
    Alpha = weight / Alpha;

    return Alpha;
}

void glodenSearch(instance &inst, const gridInfo binInfo)
{   
    if( inst.x + inst.width/2 > binInfo.dieWidth)
        inst.x = binInfo.dieWidth - inst.width/2 - 1.0 ;

    else if (inst.x - inst.width/2 < 0.0)
        inst.x = 0.0 + inst.width/2 + 1.0;

    if(inst.y + inst.height/2> binInfo.dieHeight)
        inst.y = binInfo.dieHeight - inst.height/2 - 1.0;

    else if (inst.y - inst.height/2 < 0.0)
        inst.y = 0.0 + inst.height/2 + 1.0;

    if(inst.z > 1.0)
        inst.z = 1.0;

    else if(inst.z < 0.0)
        inst.z = 0.0;    
}

double newSolution(vector <RawNet> &rawNets, vector<instance> &instances, double penaltyWeight, double gamma, double *nowCG, grid_info binInfo)
{
    double score = 0.0, wireLength = 0.0;
    for(int index = 0; index < binInfo.Numinstance; index++)
    {
        double tmp[Dimensions] = {0.0};
        double Alpha, spaceX, spaceY, spaceZ;

        tmp[0] = nowCG[index * Dimensions];
        tmp[1] = nowCG[index * Dimensions + 1];
        tmp[2] = nowCG[index * Dimensions + 2];

        Alpha = returnAlpha(tmp);

        spaceX = tmp[0] * Alpha * binInfo.binWidth;
        spaceY = tmp[1] * Alpha * binInfo.binHeight;
        spaceZ = tmp[2] * Alpha * 1;

        instances[index].x += spaceX;
        instances[index].y += spaceY;
        instances[index].z += spaceZ;

        glodenSearch(instances[index], binInfo);
    }

    score = returnTotalScore( rawNets, gamma, binInfo, penaltyWeight, instances);

    return score;

}

void updateGra(vector <RawNet> &rawNets, double gamma, vector<instance> &instances, grid_info &binInfo, double *lastGra, double *nowGra, double &penaltyWeight)
{
    double xScore, yScore, zScore, penaltyScore;
    double *originFirstLayer = createBins(binInfo);
    double *originSecondLayer = createBins(binInfo);
    double numInstances = instances.size();
    
    xScore = scoreOfX(rawNets, gamma, false);
    yScore = scoreOfY(rawNets, gamma, false);
    
    for(int j = 0; j < numInstances; j++)
        instances[j].density = returnDensity(instances[j].z, 0.0);
        
    zScore = TSVofNet(rawNets);
    
    for(int j = 0; j < numInstances; j++)
        penaltyInfoOfinstance(instances[j], binInfo, originFirstLayer, originSecondLayer, false, false);    
    
    penaltyScore = scoreOfPenalty(originFirstLayer, originSecondLayer, binInfo);

    gradientX(rawNets, gamma, instances, binInfo, penaltyWeight, xScore, penaltyScore, originFirstLayer, originSecondLayer);
    gradientY(rawNets, gamma, instances, binInfo, penaltyWeight, yScore, penaltyScore, originFirstLayer, originSecondLayer);
    gradientZ(rawNets, gamma, instances, binInfo, penaltyWeight, zScore, penaltyScore, originFirstLayer, originSecondLayer);

    memcpy( lastGra, nowGra, Dimensions * binInfo.Numinstance * sizeof(double) );

    for(int i = 0; i < binInfo.Numinstance; i++)
    {
        nowGra[i*3] = instances[i].gra_x;
        nowGra[i*3 + 1] = instances[i].gra_y;
        nowGra[i*3 + 2] = instances[i].gra_z + instances[i].gra_d;

    }

    free(originFirstLayer);
    free(originSecondLayer);
}
