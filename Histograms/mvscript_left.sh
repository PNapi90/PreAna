#!bin/bash

for i in {0..39}
do
    mv File_Merge_Cs_${i} File_Merge_Br_mid_${i}
    echo ${i} moved
done