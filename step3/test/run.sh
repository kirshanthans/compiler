#!/bin/bash
echo "Cleaning....."
rm -rf result
mkdir result
INPUT=./input
RESULT=./result
OUTPUT=./output
for file in `ls $INPUT`
do
	echo "Testing....."$file
	../Micro $INPUT/$file > $RESULT/${file%.micro}.out 
	CHECK=$(diff -U 0 $OUTPUT/${file%.micro}.out $RESULT/${file%.micro}.out | grep -v ^@ | wc -l)
	if [ $CHECK = "0" ]; then 
			echo "Test Passed"
	else
			echo "Test Failed. Number different lines: $CHECK"
	fi
done	
