#!/bin/bash

n_channels=$(($2 - 1));

#echo $1; 
#echo $n_channels;

for i in `seq 0 $n_channels`;
do 

./makeSpectrum $1 $i

done 
