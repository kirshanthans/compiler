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
for file in `ls $INPUT|grep micro`
do
	echo "Testing....."$file
	
	java -jar $SAMPLE $INPUT/$file > $OUTPUT/${file%.micro}.out
	SAMPLECHECK=$($SIMULATOR $OUTPUT/${file%.micro}.out < $INPUT/${file%.micro}.input | sed -n '1{p;q;}')
	echo "Sample Output:" $SAMPLECHECK
	$SIMULATOR $OUTPUT/${file%.micro}.out < $INPUT/${file%.micro}.input | grep Instructions
	$SIMULATOR $OUTPUT/${file%.micro}.out < $INPUT/${file%.micro}.input | grep Cycles
	
	$COMPILER $INPUT/$file > $RESULT/${file%.micro}.out
	COMPILERCHECK=$($SIMULATOR $RESULT/${file%.micro}.out < $INPUT/${file%.micro}.input | sed -n '1{p;q;}')
	echo "Compiler Output:" $COMPILERCHECK
	$SIMULATOR $RESULT/${file%.micro}.out < $INPUT/${file%.micro}.input | grep Instructions
	$SIMULATOR $RESULT/${file%.micro}.out < $INPUT/${file%.micro}.input | grep Cycles
	
	if [ "$SAMPLECHECK" = "$COMPILERCHECK" ]; then
			echo "Test Passed"
	else
			echo "Test Failed"
	fi	
done	
