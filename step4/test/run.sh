#!/bin/bash
echo "Cleaning....."
rm -rf result output
mkdir result output
INPUT=./input
SIMULATOR=./simulator/tiny
SAMPLE=./sample/Micro.jar
COMPILER=../Micro
OUTPUT=./output
RESULT=./result
for file in `ls $INPUT`
do
	echo "Testing....."$file
	java -jar $SAMPLE $INPUT/$file > $OUTPUT/${file%.micro}.out
	SAMPLECHECK=$($SIMULATOR $OUTPUT/${file%.micro}.out | sed -n '1{p;q;}')
	echo "Sample Output:" $SAMPLECHECK
	$COMPILER $INPUT/$file > $RESULT/${file%.micro}.out
	COMPILERCHECK=$($SIMULATOR $RESULT/${file%.micro}.out | sed -n '1{p;q;}')
	echo "Compiler Output:" $COMPILERCHECK
	if [ "$SAMPLECHECK" = "$COMPILERCHECK" ]; then
			echo "Test Passed"
	else
			echo "Test Failed"
	fi	
done	
