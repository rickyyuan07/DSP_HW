# generate segmented input data
for i in `seq 1 10`; do
    echo "Generating segmented input data for data $i"
    perl separator_big5.pl ../test_data/$i.txt > ../test_data/$i.seg
done

# generate all the answers for the homework
mkdir ../ans
for i in `seq 1 10`; do
    echo "Generating answer for data $i"
    disambig -text ../test_data/$i.seg -map ../ZhuYin-Big5.map -lm ../bigram.lm -order 2 > ../ans/$i.txt
done