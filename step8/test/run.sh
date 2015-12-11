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
	$SIMULATOR $OUTPUT/${file%.micro}.out < $INPUT/${file%.micro}.input > stat.out
	LINESAMPLE=$(awk '/STATISTICS/ {print FNR}' stat.out)
	if [[ $LINESAMPLE =~ ^-?[0-9]+$ ]]; then
			SAMPLECHECK=$(head -$LINESAMPLE stat.out)
			echo "Sample Output:" $SAMPLECHECK
			cat stat.out | grep Instructions
			cat stat.out | grep Cycles
	else
			echo "Error Sample"
	fi
	
	$COMPILER $INPUT/$file > $RESULT/${file%.micro}.out
	$SIMULATOR $RESULT/${file%.micro}.out < $INPUT/${file%.micro}.input > stat.out
	LINECOMPILER=$(awk '/STATISTICS/ {print FNR}' stat.out)
	if [[ $LINECOMPILER =~ ^-?[0-9]+$ ]]; then
			COMPILERCHECK=$(head -$LINECOMPILER stat.out)
			echo "Compiler Output:" $COMPILERCHECK
			cat stat.out | grep Instructions
			cat stat.out | grep Cycles
	else
			echo "Error Compiler"
	fi
	if [[ $LINESAMPLE =~ ^-?[0-9]+$ ]] && [[ $LINECOMPILER =~ ^-?[0-9]+$ ]]; then
			if [ "$SAMPLECHECK" = "$COMPILERCHECK" ]; then
					echo "Test Passed"
			else
					echo "Test Failed"
			fi	
	fi
done	
rm -rf stat.out
