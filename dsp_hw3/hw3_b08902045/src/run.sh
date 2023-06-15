echo "make and compile the files"
make
# run mydisambig on the test data
mkdir ../result
for i in `seq 1 10`; do
    echo "Generating answer for data $i"
    ../mydisambig -text ../test_data/$i.seg -map ../ZhuYin-Big5.map -lm ../bigram.lm -order 2 > ../result/$i.txt
done

# Compare the output of mydisambig with the output of disambig
for i in `seq 1 10`; do
    echo "Comparing result for data $i"
    diff ../ans/$i.txt ../result/$i.txt
done