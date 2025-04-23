#!/bin/bash

path=./case3top/case3.aux
ad4=./ad4/adaptec4.aux

./ePlace -aux $path -targetDensity 0.8  -fullPlot -targetOverflow 0.1 -legalizerPath ./legal -outputPath ./output -noQP
