#!/bin/bash

path=./case3btm/case3.aux

./ePlace -aux $path -targetDensity 1.0  -fullPlot -targetOverflow 0.1 -legalizerPath ./legal -outputPath ./output
