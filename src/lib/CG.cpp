#include <stdio.h>
#include <vector>
#include <cmath>
#include <string.h>
#include <iostream>
#include <omp.h>

#include "initial_placement.h"
#include "CG.h"
#include "readfile.h"
#include "legalization.h"

#define eta 0.1
#define total_layer 2
#define alpha 10
#define h 1
#define zChanged 1
#define znotChanged 0
#define Dimensions 3
#define spaceZweight 0.01
#define cellMoveSize 200
#define onlyPenalty 1
#define graVaribaleX 0
#define graVaribaleY 1
#define graVaribaleZ 2
#define needFillers 0

using namespace std;


float scoreOfX( const vector <RawNet> &rawNet, const float gamma, const bool isGra, instance graInstance, float originScore)
{
    float score = 0.0;

    if(isGra)
    {
        for(int net = 0; net < graInstance.numNetConnection; net++)
        {
            float n_1 = 0.0, d_1 = 0.0, n_2 = 0.0, d_2 = 0.0;
            float mn_1 = 0.0, md_1 = 0.0, mn_2 = 0.0, md_2 = 0.0;
            int nowNet = 0;

            for (int ins = 0 ; ins < rawNet[ graInstance.connectedNet[net] ].numPins; ins++)
            {
                float tmpGra = 0.0, tmpOri = 0.0;
                
                tmpGra = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->tmpX;
                tmpOri = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->x;

                n_1 += tmpOri * exp(tmpOri / gamma);
                d_1 += exp(tmpOri / gamma);
                n_2 += tmpOri * exp(-tmpOri/ gamma);
                d_2 += exp(-tmpOri/ gamma);

                mn_1 += tmpGra * exp(tmpGra / gamma);
                md_1 += exp(tmpGra / gamma);    
                mn_2 += tmpGra * exp(-tmpGra/ gamma);
                md_2 += exp(-tmpGra/ gamma);
            }
            score += ( mn_1/md_1 - mn_2/md_2 ) - ( n_1/d_1 - n_2/d_2 ); 
        }
    }
    else{

        for (int net = 0; net < rawNet.size(); net++)
        {
            float numerator_1 = 0.0;
            float denominator_1 = 0.0;
            float numerator_2 = 0.0;
            float denominator_2 = 0.0;

            for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
            {
                float tmp = 0.0;

                tmp = rawNet[net].Connection[instance]->x;

                numerator_1 += tmp * exp(tmp / gamma);
                denominator_1 += exp(tmp / gamma);
                numerator_2 += tmp * exp(-tmp/ gamma);
                denominator_2 += exp(-tmp/ gamma);
            }
            score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;
        }
    }

    return score;
}

float scoreOfY( const vector <RawNet> &rawNet, const float gamma, const bool isGra, instance graInstance, float originScore)
{
    float score = 0.0;

    if(isGra)
    {
        for(int net = 0; net < graInstance.numNetConnection; net++)
        {
            float n_1 = 0.0, d_1 = 0.0, n_2 = 0.0, d_2 = 0.0;
            float mn_1 = 0.0, md_1 = 0.0, mn_2 = 0.0, md_2 = 0.0;

            for (int ins = 0 ; ins < rawNet[ graInstance.connectedNet[net] ].numPins; ins++)
            {
                float tmpGra = 0.0, tmpOri = 0.0;

                tmpGra = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->tmpY;
                tmpOri = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->y;

                n_1 += tmpOri * exp(tmpOri / gamma);
                d_1 += exp(tmpOri / gamma);
                n_2 += tmpOri * exp(-tmpOri/ gamma);
                d_2 += exp(-tmpOri/ gamma);

                mn_1 += tmpGra * exp(tmpGra / gamma);
                md_1 += exp(tmpGra / gamma);
                mn_2 += tmpGra * exp(-tmpGra/ gamma);
                md_2 += exp(-tmpGra/ gamma);
            }
            score += ( mn_1/md_1 - mn_2/md_2 ) - ( n_1/d_1 - n_2/d_2 ); 
        }
    }
    else
    {
        for (int net = 0; net < (int) rawNet.size(); net++)
        {
            float numerator_1 = 0.0;
            float denominator_1 = 0.0;
            float numerator_2 = 0.0;
            float denominator_2 = 0.0;

            for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
            {
                float tmp = 0.0;

                tmp = rawNet[net].Connection[instance]->y;

                numerator_1 += tmp * exp(tmp / gamma);
                denominator_1 += exp(tmp / gamma);
                numerator_2 += tmp * exp(-tmp/ gamma);
                denominator_2 += exp(-tmp/ gamma);
            }
            score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;
        }
    }
    return score;
}

float bellShapeFunc( const float z, const float layer)
{     
    float distance = abs(z - layer);

    if (distance <= 0.5) 
        return 1.0 - 2.0 * (distance * distance);

    else if (distance > 0.5 && distance <= 1) 
        return 2.0 * ((distance - 1.0) * (distance - 1.0));
    
    else 
        return 0.0;
}

float RSum( const float z){
    
    float rsum = 0.0 ;
    
    // float eta = 0.1 ;

    for (int i = 0 ; i < total_layer; i++)
    {
        float l = (float) i;
        float tmp = bellShapeFunc(z, l);

        rsum += exp( tmp / eta);
    }
    return rsum;
}

float returnDensity(const float z, const float *densityMap)
{
    int index = int(z/0.1f);

    return densityMap[index];
}

float returnDensityInDensityMap(const float z, const float layer)
{
    float Rz, Bz, BzRSum;

    Rz = bellShapeFunc(z, layer);
    Bz = exp(Rz/ eta);
    BzRSum = RSum(z);

    Bz = Bz / BzRSum;
    return Bz;
}

float returnPsi(float z)
{
    float psi = 0;
    
    for(int i = 0; i < total_layer; i++)
    {
        float score ;

        score = (float)i * returnDensityInDensityMap(z, (float)i);
        psi += score;
    }

    return psi;
}

float TSVofNet( vector <RawNet> &rawNet, bool isGra, instance graInstance, float originScore, float *densityMap)
{
    float score = 0.0;
    int size = rawNet.size();

    if(isGra)
    {
        for(int net = 0; net < graInstance.numNetConnection; net++)
        {
            float n_1 = 0.0, d_1 = 0.0, n_2 = 0.0, d_2 = 0.0;
            float mn_1 = 0.0, md_1 = 0.0, mn_2 = 0.0, md_2 = 0.0;
            float weight = 1.0f;
            float mul = 0;

            // if(rawNet[net].numPins == 2)
            //     weight = 1;
            // else if(rawNet[net].numPins == 3)
            //     weight = 2000;
            // else if(rawNet[net].numPins == 4)
            //     weight = 2500;
            // else
            //     weight = 3000;
 
            for (int ins = 0 ; ins < rawNet[ graInstance.connectedNet[net] ].numPins; ins++)
            {
                float tmpGra = 0.0, tmpOri = 0.0, tmpGraPsi = 0.0, tmpPsi = 0.0;
                
                tmpGra = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->tmpD;
                tmpOri = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->density;

                tmpPsi= 1.0 - tmpOri;
                tmpGraPsi = 1.0 - tmpGra;

                if(tmpPsi < 0.5)
                    tmpPsi = 0.00001;
                else
                    tmpPsi = 0.99999;

                if(tmpGraPsi < 0.5)
                    tmpGraPsi = 0.00001;
                else
                    tmpGraPsi = 0.9999;
                
                n_1 += tmpPsi * exp(tmpPsi / 0.05);
                d_1 += exp(tmpPsi / 0.05);
                n_2 += tmpPsi * exp(-tmpPsi/ 0.05);
                d_2 += exp(-tmpPsi/ 0.05);

                mn_1 += tmpGraPsi * exp(tmpGraPsi / 0.05);
                md_1 += exp(tmpGraPsi / 0.05);
                mn_2 += tmpGraPsi * exp(-tmpGraPsi/ 0.05);
                md_2 += exp(-tmpGraPsi/ 0.05);
            }
            score += (( mn_1/md_1 - mn_2/md_2 ) - ( n_1/d_1 - n_2/d_2 )); 
        }

        score = score; 
    }
    else
    {
        for (int net = 0; net < size; net++)
        {
            float numerator_1 = 0.0;
            float denominator_1 = 0.0;
            float numerator_2 = 0.0;
            float denominator_2 = 0.0;
            float weight = 1.0f;

            // if(rawNet[net].numPins == 2)
            //     weight = 10;
            // else if(rawNet[net].numPins == 3)
            //     weight = 1000;
            // else if(rawNet[net].numPins == 4)
            //     weight = 1500;
            // else
            //     weight = 2000;
            

            for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
            {
                float tmpPsi = rawNet[net].Connection[instance]->z;

                tmpPsi = 1.0 - returnDensity(tmpPsi, densityMap);

                if(tmpPsi < 0.5)
                    tmpPsi = 0.00001;
                else
                    tmpPsi = 0.99999;
               
                numerator_1 += tmpPsi * exp(tmpPsi / 0.05);
                denominator_1 += exp(tmpPsi / 0.05);
                numerator_2 += tmpPsi * exp(-tmpPsi/ 0.05);
                denominator_2 += exp(-tmpPsi/ 0.05);
            }
            score += (numerator_1/denominator_1 - numerator_2 / denominator_2) ;
        }
    }
    
    return score;
}

float scoreOfz( vector <RawNet> &rawNets, vector <instance> &instances, gridInfo binInfo, bool zisChanged, float *densityMap, vector <instance> &fillers)
{
    float score = 0.0;
    int size = instances.size();
    int numFiller = fillers.size();
    float *firstLayer = createBins(binInfo);
	float *secondLayer = createBins(binInfo);

    //  claculate score of penalty

    for(int i = 0; i < size; i++)
    {
        if(zisChanged)
        {
            float tmpD = returnDensity(instances[i].z, densityMap);

            instances[i].density = tmpD;
            instances[i].tmpD = instances[i].density;
        }
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, false, false, 0, 0);
    }

    if (needFillers)
    {
        for(int i = 0; i < numFiller; i++)
            penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, false, false, 0, 0);
    }
    
    score = scoreOfPenalty(firstLayer, secondLayer, binInfo);

    free(firstLayer);
    free(secondLayer);
        
    return score;
}

float *createBins(gridInfo binInfo)
{
    float *bins;

    bins = (float *)calloc( binInfo.binXnum * binInfo.binYnum, sizeof( float ) );

    return bins;
}

void penaltyInfoOfinstance( const instance instance, const gridInfo binInfo, float *firstLayer, float *secondLayer, bool isGra, bool needMinus, float *graGrade, const int graVariable)
{
    int leftXnum, rightXnum, topYnum, btmYnum;
    int row = (int) binInfo.binXnum;

    float coordinate[5] = { 0.0 };
    int length[4] = {0};
    
    float leftX = 0, rightX = 0, topY = 0, btmY = 0;

    if(isGra)
    {   
        switch (graVariable)
        {
        case 0:
            leftX = instance.tmpX - (instance.width * 0.5);
            rightX = instance.tmpX + (instance.width * 0.5);
            topY = instance.y - (instance.height * 0.5);
            btmY = instance.y + (instance.height * 0.5);
            coordinate[4] = instance.density;
            break;
        
        case 1:
            leftX = instance.x - (instance.width * 0.5);
            rightX = instance.x + (instance.width * 0.5);
            topY = instance.tmpY - (instance.height * 0.5);
            btmY = instance.tmpY + (instance.height * 0.5);
            coordinate[4] = instance.density;
            break;
        
        case 2:
            leftX = instance.x - (instance.width * 0.5);
            rightX = instance.x + (instance.width * 0.5);
            topY = instance.y - (instance.height * 0.5);
            btmY = instance.y + (instance.height * 0.5);
            coordinate[4] = instance.tmpD;
            break;
        }
    }
    else
    {
        leftX = instance.x - (instance.width * 0.5);
        rightX = instance.x + (instance.width * 0.5);
        topY = instance.y - (instance.height * 0.5);
        btmY = instance.y + (instance.height * 0.5);
        coordinate[4] = instance.density;
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

    float grade[2] = {0.0};

    if(needMinus)
        calculatePenaltyArea( coordinate, length, firstLayer, secondLayer, row, instance, binInfo, needMinus, grade);
    
    else
        calculatePenaltyArea( coordinate, length, firstLayer, secondLayer, row, instance, binInfo, false, grade);

    if( graGrade != NULL)
        *graGrade = grade[0] + grade[1];

    // if_left_x_num = int( if_left_x // grid_info[2] )
    // if_right_x_num = int( if_right_x // grid_info[2])
    // if_top_y_num = int( if_top_y // grid_info[3] )
    // if_btm_y_num = int( if_btm_y // grid_info[3])     
    // if_coordinate = [if_left_x, if_right_x , if_top_y, if_btm_y]
    // if_length = [if_left_x_num, if_right_x_num, if_top_y_num, if_btm_y_num ]
}

void calculatePenaltyArea( float coordinate[], int *length, float *firstLayer, float *secondLayer, int row, instance instance, gridInfo binInfo, bool needMinus, float *grade)
{   
    // float routing_overflow;

    float density = coordinate[4];
    float firstLayerGrade = 0.0;
    float secondLayerGrade = 0.0;

    for (int y = 0; y <= length[3] - length[2]; ++y) 
    {
        float y_length, x_length;
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

            if(bin_index > binInfo.binXnum * binInfo.binYnum || bin_index < 0)
                cout << instance.instIndex << endl;

            if(needMinus)
            {
                firstLayerGrade += (binInfo.binHeight * binInfo.binWidth - firstLayer[bin_index] ) * (binInfo.binHeight * binInfo.binWidth - firstLayer[bin_index] );
                secondLayerGrade += (binInfo.binHeight * binInfo.binWidth - secondLayer[bin_index] ) * (binInfo.binHeight * binInfo.binWidth - secondLayer[bin_index] );
                
                firstLayer[bin_index] -= y_length * x_length * density;
                secondLayer[bin_index] -= y_length * x_length * (1.0 - density);
            }
            else
            {
                firstLayer[bin_index] += y_length * x_length * density ;
                secondLayer[bin_index] += y_length * x_length * (1.0 - density) ;
                
                firstLayerGrade += (binInfo.binHeight * binInfo.binWidth - firstLayer[bin_index] ) * (binInfo.binHeight * binInfo.binWidth - firstLayer[bin_index] );
                secondLayerGrade += (binInfo.binHeight * binInfo.binWidth - secondLayer[bin_index] ) * (binInfo.binHeight * binInfo.binWidth - secondLayer[bin_index] );
                
            }
            // routing_overflow = ((x_length + y_length) / 2.0) * y_length * x_length;
        }
    }

    grade[0] = firstLayerGrade;
    grade[1] = secondLayerGrade;

    // instance.inflate_ratio = inflation_ratio(instance, routing_overflow / (instance.width * instance.height));
    // instance.inflate_ratio = 1;
}

float scoreOfPenalty(float *firstLayer, float *secondLayer, gridInfo binInfo)
{
    float score = 0.0;
    float area = binInfo.binWidth * binInfo.binHeight;
    
    int binXnum = (int) binInfo.binXnum;
    int binYnum = (int) binInfo.binYnum;

    for(int i = 0; i < binYnum; i++)
    {
        for(int j = 0; j < binXnum; j++)
        {
            int b = j + i*binXnum;
            score +=  (firstLayer[b] - area) * (firstLayer[b] - area) + (secondLayer[b] - area) * (secondLayer[b] - area);                
        }
    }
    
    
    return score;    
}

void gradientX(vector <RawNet> &rawNet, const float gamma, vector <instance> &instances, gridInfo binInfo, const float penaltyWeight, const float xScore, const float penaltyScore, const float *originFirstLayer, const float *originSecondLayer)
{
    int size = instances.size();
    float *firstLayer = createBins(binInfo);
    float *secondLayer = createBins(binInfo);

    memcpy(firstLayer, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));
    memcpy(secondLayer, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));

    for(int i = 0; i < size; i++)
    {
        if(instances[i].canPass)
            continue;
        
        bool isGra, needMinus;
        float score = 0.0, score2 = 0.0, graGrade = 0.0;
        
        instances[i].tmpX = instances[i].x;
        instances[i].tmpX += 2;
        
        score = scoreOfX(rawNet, gamma, true, instances[i], xScore);

        // Dedecut the original block(needMinus = ture) first and the add the gra one(isGra = true).

        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = true, &graGrade, graVaribaleX);
        score2 -= graGrade;
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = false, &graGrade, graVaribaleX);
        score2 += graGrade;
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = true, &graGrade, graVaribaleX);
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = false, &graGrade, graVaribaleX);

        instances[i].tmpX = instances[i].x;
        instances[i].gra_x =  onlyPenalty*(score)/h + ( penaltyWeight * (score2) ) / h;
    }
    free(firstLayer);
    free(secondLayer);
}

void gradientY(vector <RawNet> &rawNet, const float gamma, vector <instance> &instances, gridInfo binInfo, const float penaltyWeight, const float yScore, const float penaltyScore, const float *originFirstLayer, const float *originSecondLayer)
{
    int size = instances.size();
    
    float *firstLayer = createBins(binInfo);      
    float *secondLayer = createBins(binInfo);
  
    memcpy(firstLayer, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));
    memcpy(secondLayer, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));

    for(int i = 0; i < size; i++)
    {   
        if(instances[i].canPass)
            continue;

        bool needMinus = false, isGra = false;
        float score = 0.0, score2 = 0.0, graGrade = 0.0;

        instances[i].tmpY = instances[i].y;
        instances[i].tmpY += 2;

        score = scoreOfY(rawNet, gamma, true, instances[i], yScore);

        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = true, &graGrade, graVaribaleY);
        score2 -= graGrade;
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = false, &graGrade, graVaribaleY);
        score2 += graGrade;            
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = true, &graGrade, graVaribaleY);
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = false, &graGrade, graVaribaleY);
        
        instances[i].tmpY = instances[i].y;
        instances[i].gra_y = ( onlyPenalty*(score) + penaltyWeight * (score2) ) / h;
    }
    
    free(firstLayer);
    free(secondLayer);
}

void gradientZ(vector <RawNet> &rawNet, const float gamma, vector <instance> &instances, gridInfo binInfo, const float penaltyWeight, const float zScore, const float penaltyScore, const float *originFirstLayer, const float *originSecondLayer, float *densityMap)
{   
    float *fl = createBins(binInfo);
    float *sl = createBins(binInfo);

    memcpy(fl, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));
    memcpy(sl, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));

    for(int i = 0; i < instances.size(); i++)
    {
        if(instances[i].canPass)
            continue;

        float tmpd = instances[i].density;
        float score , score2 = 0.0, graGrade = 0.0;
        bool needMinus = false;
        
        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true, &graGrade, graVaribaleZ);
        score2 -= graGrade;
        
        if(instances[i].z < 4996)
            instances[i].tmpZ = 8888;
        else
            instances[i].tmpZ = 2222;
        
        instances[i].tmpD = returnDensity(instances[i].tmpZ, densityMap);

        score = TSVofNet(rawNet, true, instances[i], zScore, densityMap);
     
        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, false, &graGrade, graVaribaleZ);
        score2 += graGrade;

        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, needMinus = true, &graGrade, graVaribaleZ);
        instances[i].tmpZ = instances[i].z;
        instances[i].tmpD = tmpd;

        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, needMinus = false, &graGrade, graVaribaleZ);
        
        instances[i].gra_z = (alpha) * (score);
        instances[i].gra_d = penaltyWeight * score2;
    }

    free(fl);
    free(sl);
}

float infaltionRatio(instance instance, float routingOverflow)
{   
    return 0.0;
}

float returnTotalScore(vector<RawNet> &rawNet, const float gamma, const gridInfo binInfo, const float penaltyWeight, vector <instance> &instances, float *densityMap, vector <instance> &fillers)
{
    float score_of_x, score_of_y = 0.0, score_of_z = 0.0, densityScore = 0.0, totalScore, wireLength;
    float penaltyScore = 0.0;

#pragma omp parallel sections num_threads(8)
{
    #pragma omp section
        score_of_x = scoreOfX(rawNet, gamma, false, instances[0], 0);
    #pragma omp section
        score_of_y = scoreOfY(rawNet, gamma, false, instances[0], 0);
    #pragma omp section
    {
        densityScore = scoreOfz(rawNet, instances, binInfo, 1, densityMap, fillers);
        score_of_z = TSVofNet(rawNet, false, instances[0], 0, densityMap);
    }
}
    
    penaltyScore = score_of_z * (alpha) + (densityScore) * penaltyWeight; 
    wireLength = score_of_x + score_of_y;

    // writeData(wireLength, score_of_z, densityScore);
    
    if(penaltyWeight == 1)
    {
        score_of_z = TSVofNet(rawNet, false, instances[0], 1, densityMap);
        printf("HPWL: %f, HBT: %f\n", wireLength, score_of_z);
    }
        

    return score_of_z;
}

void CGandGraPreprocessing( vector <instance> &instances, float *nowGra, float *nowCG, float *lastGra, float *lastCG)
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

void conjugateGradient(float *nowGra, float *nowCG, float *lastCG, float *lastGra, int Numinstance, int iteration)
{    
    float beta = 0.0, norm = 0.0;
    for(int index = 0; index < Numinstance * Dimensions; index++)
    {
        norm += fabs(lastGra[index]);
        beta += nowGra[index] * ( nowGra[index] - lastGra[index]);
    }

    norm = norm * norm;
    beta = beta/norm;
    
    if(beta < 0)
        beta = 0;

    for(int index = 0; index < Numinstance * Dimensions; index ++)
        nowCG[index] = (-nowGra[index]) + (beta * lastCG[index]);
    
}

float returnAlpha(float nowCG[])
{   
    float Alpha = 0.0;

    for(int i = 0; i < 3; i++)
        Alpha += nowCG[i] * nowCG[i];

    Alpha = sqrt(Alpha);
    Alpha =  1.0 / Alpha;

    return Alpha;
}

void glodenSearch(instance &inst, const gridInfo binInfo)
{   
    if( inst.x + inst.width/2 > binInfo.dieWidth)
        inst.x = binInfo.dieWidth - inst.width ;

    else if (inst.x - inst.width <= 0.0)
        inst.x = inst.width + 1.0;

    if(inst.y + inst.height/2> binInfo.dieHeight)
        inst.y = binInfo.dieHeight - inst.height;

    else if (inst.y - inst.height/2 <= 0.0)
        inst.y = 0.0 + inst.height + 1.0;

    if(inst.z > 10000.0)
        inst.z = 10000.0;

    else if(inst.z < 0.0)
        inst.z = 0.0;    
}

void newSolution(vector<instance> &instances, float *nowCG, grid_info binInfo)
{
    float score = 0.0, wireLength = 0.0, weight = 1;
    for(int index = 0; index < binInfo.Numinstance; index++)
    {
        if(instances[index].canPass)
            continue;
        
        float tmp[Dimensions] = {0.0};
        float Alpha = 1, spaceX, spaceY, spaceZ;

        tmp[0] = nowCG[index * Dimensions]; 
        tmp[1] = nowCG[index * Dimensions + 1] ;
        tmp[2] = 0;

        Alpha = returnAlpha(tmp);

        spaceX = tmp[0] * Alpha * binInfo.binWidth * 0.99f;
        spaceY = tmp[1] * Alpha * binInfo.binHeight * 0.99f;
        spaceZ = (nowCG[index * Dimensions + 2] > 0)? 5.0f : -5.0f ;

        instances[index].refX = instances[index].x;
        instances[index].refY = instances[index].y;
        instances[index].refZ = instances[index].z;

        instances[index].x += spaceX;
        instances[index].y += spaceY;

        if(instances[index].z < 4997)
            instances[index].z += spaceZ;
        else
            instances[index].z -= spaceZ;
        
        glodenSearch(instances[index], binInfo);
    
        instances[index].tmpX = instances[index].x;
        instances[index].tmpY = instances[index].y;
        instances[index].tmpZ = instances[index].z;
    }
}

void updateGra(vector <RawNet> &rawNets, float gamma, vector<instance> &instances, grid_info &binInfo, float *lastGra, float *nowGra, float *lastCG, float *nowCG, float &penaltyWeight, float *densityMap, vector <instance> &fillers)
{
    float xScore, yScore, zScore, penaltyScore, tmpScore = 0.0;
    float *originFirstLayer = createBins(binInfo);
    float *originSecondLayer = createBins(binInfo);
    const int numInstances = instances.size();
    const int numFiller = fillers.size();

#pragma omp parallel sections num_threads(8)
{
    #pragma omp section
        xScore = scoreOfX(rawNets, gamma, false, instances[0], 0);
    #pragma omp section
        yScore = scoreOfY(rawNets, gamma, false, instances[0], 0);
    #pragma omp section
    {
        for(int j = 0; j < numInstances; j++)
        {
            float tmpD = returnDensity(instances[j].z, densityMap);

            instances[j].density = tmpD;
            instances[j].tmpD = tmpD;
        }
            
        zScore = TSVofNet(rawNets, false, instances[0], 0, densityMap);

        for(int j = 0; j < numInstances; j++)
            penaltyInfoOfinstance(instances[j], binInfo, originFirstLayer, originSecondLayer, false, false, NULL, 0);
        
        // for(int j = 0; j < numFiller; j++)
        //     penaltyInfoOfinstance(fillers[j], binInfo, originFirstLayer, originSecondLayer, false, false, NULL, 0);

        penaltyScore = scoreOfPenalty(originFirstLayer, originSecondLayer, binInfo);  
    }
}

#pragma omp parallel sections num_threads(8)
{
    #pragma omp section
        gradientX(rawNets, gamma, instances, binInfo, penaltyWeight, xScore, penaltyScore, originFirstLayer, originSecondLayer);
    #pragma omp section
        gradientY(rawNets, gamma, instances, binInfo, penaltyWeight, yScore, penaltyScore, originFirstLayer, originSecondLayer);
    #pragma omp section
        gradientZ(rawNets, gamma, instances, binInfo, penaltyWeight, zScore, penaltyScore, originFirstLayer, originSecondLayer, densityMap);
    // #pragma omp section
    // if(needFillers)
    // {
    //     graFillerX(fillers, binInfo, penaltyWeight, originFirstLayer, originSecondLayer);
    //     graFillerY(fillers, binInfo, penaltyWeight, originFirstLayer, originSecondLayer);
    // }
        
}    
    memcpy( lastGra, nowGra, Dimensions * numInstances * sizeof(float) );
    memcpy( lastCG, nowCG, Dimensions * numInstances * sizeof(float) );

    for(int i = 0; i < numInstances; i++)
    {
        nowGra[i*3] = instances[i].gra_x;
        nowGra[i*3 + 1] = instances[i].gra_y;
        nowGra[i*3 + 2] = instances[i].gra_z + instances[i].gra_d;
    }

    free(originFirstLayer);
    free(originSecondLayer);
}

void returnDensityMap(float *densityMap)
{
    float z = 0.0;

    for(int i = 0 ; i < 100001; i++)
    {
        densityMap[i] = returnDensityInDensityMap(z, 0.0);
        
        z += 1e-5;
    }
}

void clacBktrk( vector <instance> &instances, float *lastGra, float *nowGra, int iteration, float *optParam, 
                vector <RawNet> &rawNets, float gamma, grid_info &binInfo, float *lastCG, float *nowCG, 
                float &penaltyWeight, float *densityMap, float *lastRefSoltion, float *curRefSoltion)
{
    float stepSize = 0.0;
    float newParam =  (1+ sqrt( 4.0 * *optParam * *optParam + 1.0) ) / 2;
    int numInstances = instances.size();

    float *curRefGra = new float [numInstances *3] {0.0};
    float *curUk = new float [numInstances *3] {0.0};
    float *newRefSolution = new float [numInstances *3] {0.0};
    float param = ((*optParam-1) / newParam);


    memcpy(curRefGra, nowGra, numInstances * 3 * sizeof(float));

    for(int i = 0; i < numInstances; i++)
    {
        curUk[i*3] = instances[i].x;
        curUk[i*3 + 1] = instances[i].y;
        curUk[i*3 + 2] = instances[i].z;

        curRefSoltion[i*3] = instances[i].refX;
        curRefSoltion[i*3+1] = instances[i].refY;
        curRefSoltion[i*3+2] = instances[i].refZ;
    }

    if(iteration == 0)
    {
        stepSize = 1;
        memcpy(lastRefSoltion, curRefSoltion, numInstances * 3 * sizeof(float));
    }
    else
        stepSize = calcLipschitz(lastRefSoltion, curRefSoltion, lastGra, nowGra, numInstances);
    

    while (true)
    {
        for(int i = 0; i < numInstances; i++)
        {
            float refX, refY, refZ;
            refX = curRefSoltion[i * 3] - curRefGra[i * 3] * stepSize;
            refY = curRefSoltion[i * 3 + 1] - curRefGra[i * 3 + 1] * stepSize;
            refZ = curRefSoltion[i * 3 + 2] - curRefGra[i * 3 + 2] * stepSize;

            instances[i].x = refX + (refX - curUk[i*3]) * param;
            instances[i].y = refY + (refY - curUk[i*3]) * param;
            instances[i].z = refZ + (refZ - curUk[i*3]) * param;

            glodenSearch(instances[i], binInfo);

            newRefSolution[i*3] = instances[i].x;
            newRefSolution[i*3+1] = instances[i].y;
            newRefSolution[i*3+2] = instances[i].z;
            
        }

        // updateGra(rawNets, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap);

        float newStepsize = calcLipschitz(curRefSoltion, newRefSolution, curRefGra, nowGra, numInstances);
        
        if( 0.95 * stepSize <= newStepsize)
        {
            stepSize = newStepsize;
            break;
        }

        stepSize = newStepsize;
    }

    //Net's Method

    for(int i = 0; i < numInstances; i++)
    {   
        instances[i].x = curRefSoltion[i*3] - curRefGra[i*3] * stepSize;
        instances[i].y = curRefSoltion[i*3+1] - curRefGra[i*3+1] * stepSize;
        instances[i].z = curRefSoltion[i*3+2] - curRefGra[i*3+2] * stepSize;

        glodenSearch(instances[i], binInfo);

        instances[i].refX = instances[i].x + ( instances[i].x - curUk[i*3]) * param;
        instances[i].refY = instances[i].y + ( instances[i].y - curUk[i*3]) * param;
        instances[i].refZ = instances[i].z + ( instances[i].z - curUk[i*3]) * param;

        instances[i].x = instances[i].refX;
        instances[i].y = instances[i].refY;
        instances[i].z = instances[i].refZ;

        glodenSearch(instances[i], binInfo);

        instances[i].refX = instances[i].x;
        instances[i].refY = instances[i].y;
        instances[i].refZ = instances[i].z;
    }

    // updateGra(rawNets, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap);

    for(int i = 0; i < numInstances; i++)
    {
        instances[i].x = curRefSoltion[i*3] - curRefGra[i*3] * stepSize;
        instances[i].y = curRefSoltion[i*3+1] - curRefGra[i*3+1] * stepSize;
        instances[i].z = curRefSoltion[i*3+2] - curRefGra[i*3+2] * stepSize;

        glodenSearch(instances[i], binInfo);

    }
    
    memcpy(lastRefSoltion, curRefSoltion, numInstances * 3 * sizeof(float));
    memcpy(lastGra, curRefGra, numInstances * 3 * sizeof(float));

    *optParam = newParam;

    free(curUk);
    free(curRefGra);
    free(newRefSolution);

}


float calcLipschitz(float *lastRefSolution, float *nowRefSolution, float *lastGra, float *nowGra, int numOfStdCell)
{
    float denomenator = 0.0;
    float numerator = 0.0;
    
    for (int i = 0; i < numOfStdCell*3; i++)
    {
        numerator += (nowRefSolution[i] - lastRefSolution[i]) * (nowRefSolution[i] - lastRefSolution[i]);
        denomenator += (nowGra[i] - lastGra[i]) * (nowGra[i] - lastGra[i]);
    }

    return sqrt(numerator)/sqrt(denomenator);
}

void refPosition(vector <instance> &instances)
{
    int size = instances.size();

    for(int i = 0; i < size; i++)
    {
        instances[i].x = instances[i].refX;
        instances[i].y = instances[i].refY;
        instances[i].z = instances[i].refZ;
    }
}

void fillerPreprocess(vector <instance> &filler, gridInfo binInfo, Die topDie, Die btmDie)
{
    float binArea = binInfo.binWidth * 2 * binInfo.binHeight * 2;
    float topDieArea = (topDie.upperRightX * topDie.upperRightY) * (float) (100 - topDie.MaxUtil) / 100.0;
    float btmDieArea = (btmDie.upperRightX * btmDie.upperRightY) * (float) (100 - btmDie.MaxUtil) / 100.0;

    int numTopFiller = (int) (topDieArea / binArea) + 1;
    int numBtmFiller = (int) (btmDieArea / binArea) + 1; 

    for(int i = 0; i < numTopFiller; i++)
    {
        // float X = fmod( (float) rand(), ( maxX - minX + 1) ) + minX ;
        instance tmp;

        tmp.x = fmod( (float) rand(), ( 22000 - 100 + 1) ) + 100;
        tmp.y = fmod( (float) rand(), ( 18000 - 100 + 1) ) + 100;
        tmp.z = 9999;
        tmp.instIndex = i;
        tmp.layer = 3;
        tmp.width = binInfo.binWidth * 2;
        tmp.height = binInfo.binHeight * 2;
        tmp.density = 0.99999;

        filler.emplace_back(tmp);
    }

    for(int i = 0; i < numTopFiller; i++)
    {
        // float X = fmod( (float) rand(), ( maxX - minX + 1) ) + minX ;
        instance tmp;

        tmp.x = fmod( (float) rand(), ( 22000 - 100 + 1) ) + 100 ;
        tmp.y = fmod( (float) rand(), ( 18000 - 100 + 1) ) + 100;
        tmp.z = 1;
        tmp.instIndex = i;
        tmp.layer = 3;
        tmp.width = binInfo.binWidth * 2;
        tmp.height = binInfo.binHeight * 2;
        tmp.density = 0.00001;

        filler.emplace_back(tmp);
    }
}

void graFillerX(vector <instance> &fillers, gridInfo binInfo, const float penaltyWeight, const float *ori1stLayer, const float *ori2ndLayer)
{
    int size = fillers.size();
    float *firstLayer = createBins(binInfo);
    float *secondLayer = createBins(binInfo);

    memcpy(firstLayer, ori1stLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));
    memcpy(secondLayer, ori2ndLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));

    for(int i = 0; i < size; i++)
    { 
        bool isGra, needMinus;
        float score2 = 0.0, graGrade = 0.0;
        
        fillers[i].tmpX = fillers[i].x;
        fillers[i].tmpX += h;
        
        // Dedecut the original block(needMinus = ture) first and the add the gra one(isGra = true).

        penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = true, &graGrade, graVaribaleX);
        score2 -= graGrade;
        penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = false, &graGrade, graVaribaleX);
        score2 += graGrade;
        penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = true, &graGrade, graVaribaleX);
        penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = false, &graGrade, graVaribaleX);

        fillers[i].tmpX = fillers[i].x;
        fillers[i].gra_x = ( penaltyWeight * (score2) ) / h;
    }
    free(firstLayer);
    free(secondLayer);
}

void graFillerY(vector <instance> &fillers, gridInfo binInfo, const float penaltyWeight, const float *ori1stLayer, const float *ori2ndLayer)
{
    int size = fillers.size();
    float *firstLayer = createBins(binInfo);
    float *secondLayer = createBins(binInfo);

    memcpy(firstLayer, ori1stLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));
    memcpy(secondLayer, ori2ndLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(float));

    for(int i = 0; i < size; i++)
    { 
        bool isGra, needMinus;
        float score2 = 0.0, graGrade = 0.0;
        
        fillers[i].tmpY = fillers[i].y;
        fillers[i].tmpY += h;
        
        // Dedecut the original block(needMinus = ture) first and the add the gra one(isGra = true).

        penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = true, &graGrade, graVaribaleY);
        score2 -= graGrade;
        penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = false, &graGrade, graVaribaleY);
        score2 += graGrade;
        penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = true, &graGrade, graVaribaleY);
        penaltyInfoOfinstance(fillers[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = false, &graGrade, graVaribaleY);

        fillers[i].tmpY = fillers[i].y;
        fillers[i].gra_y = ( penaltyWeight * (score2) ) / h;
    }

    free(firstLayer);
    free(secondLayer);
}

void mvFiller(vector <instance> &fillers, gridInfo binInfo)
{
    int size = fillers.size();
    float we;

    for(int i = 0; i < size; i++)
    {
        if( fillers[i].gra_x == 0 && fillers[i].gra_y == 0)
            we = 1;
        else
            we =  1 / sqrt(fillers[i].gra_x * fillers[i].gra_x + fillers[i].gra_y * fillers[i].gra_y);
        
        fillers[i].x += we * fillers[i].gra_x * binInfo.binWidth * 0.8;
        fillers[i].y += we * fillers[i].gra_y * binInfo.binHeight * 0.8;
        
        glodenSearch(fillers[i], binInfo);
    }
}

bool OvRatio(vector <instance> &instances, gridInfo binInfo)
{
    // check overflow ratio

    int size = instances.size();
    float totalInstanceArea = 0;
    float *firstLayer = createBins(binInfo);
    float *secondLayer = createBins(binInfo);
    float score = 0.0;
    float area = binInfo.binWidth * binInfo.binHeight;
    
    int binXnum = (int) binInfo.binXnum;
    int binYnum = (int) binInfo.binYnum;
    
    for(int i = 0; i < size; i++)
    {
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, false, false, 0, 0);
        
        if(!instances[i].canPass)
            totalInstanceArea += instances[i].area;
    }
        

    for(int i = 0; i < binYnum; i++)
    {
        for(int j = 0; j < binXnum; j++)
        {
            int b = j + i*binXnum;
            score +=  fabs((firstLayer[b] - area));
            score +=  fabs((secondLayer[b] - area));
        }
    }

    free(firstLayer);
    free(secondLayer);

    if(score / totalInstanceArea < 0.3f)
        return true;
    else
        return false;
}