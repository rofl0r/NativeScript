#!/bin/bash
#Run all tests for scriptSpeed

start=`date +%s`

echo $(date) > output.csv

repeats=10

echo "Running each test" $repeats "times."

# standard tests available in all languages

testCaseList=(
"expression 2"
"expression 3"
"expression 4"
"expression 5"
"expression 6"
"expression 2 opt"
"expression 3 opt"
"expression 4 opt"
"expression 5 opt"
"expression 6 opt"
"callback 0"
"callback 1"
"callback 2"
"callback 3"
"callback 4"
)

for lang in js lua cs ns;
do
	for testCase in "${testCaseList[@]}"
	do
		echo "" >> output.csv;
		echo $lang $testCase;
		echo $lang $testCase >> output.csv;

		for ((i=1;i<=repeats;i++));
		do
			./scrSpeed $lang $testCase >> output.csv
			echo "" >> output.csv
		done;
	done;
done;

# point simulation

testCaseList=(
"pointSimul allScript"
"pointSimul allNative"
"pointSimul allScriptMinCallback"
"pointSimul scriptToNative"
)

for lang in js lua cs;
do
        for testCase in "${testCaseList[@]}"
        do
                echo "" >> output.csv;
                echo $lang $testCase;
                echo $lang $testCase >> output.csv;

		for ((i=1;i<=repeats;i++));
                do 
                        ./scrSpeed $lang $testCase >> output.csv
                        echo "" >> output.csv
                done;
        done;
done;


# point simul cs struct

testCaseList=(
"pointSimul allScriptStruct"
"pointSimul allScriptMinCallbackStruct"
"pointSimul scriptToNativeStruct"
)

for testCase in "${testCaseList[@]}"
do
        echo "" >> output.csv;
        echo $lang $testCase;
        echo $lang $testCase >> output.csv;

	for ((i=1;i<=repeats;i++));
        do
               ./scrSpeed $lang $testCase >> output.csv
                echo "" >> output.csv
        done;
done;

end=`date +%s`
dt=$(($end - $start))
dh=$(($dt/3600))
dt3=$(($dt-3600*$dh))
dm=$(($dt3/60))
ds=$(($dt3-60*$dm))

printf "Total runtime: %02d:%02d:%02d\n" $dh $dm $ds
