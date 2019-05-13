#!bin/bash


./Analysis -t 4 -b 10000 -m -1
cd Histograms
./Merge -t 4 -p 40
. mvscript_left.sh
cd ..

echo left done


./Analysis -t 4 -b 10000 -m 0
cd Histograms
./Merge -t 4 -p 40
. mvscript.sh
cd ..

echo mid done


./Analysis -t 4 -b 10000 -m 1
cd Histograms
./Merge -t 4 -p 40
. mvscript_right.sh
cd ..

echo right done