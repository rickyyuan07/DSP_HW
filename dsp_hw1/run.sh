#!/bin/bash
n_iter=$1
for ((i=1; i<=5; i++))
do
    echo "Running $i"
    ./train ${n_iter} model_init.txt data/train_seq_0${i}.txt model_0${i}.txt
done

./test modellist.txt data/test_seq.txt result.txt

echo "======= Accuracy ======="
./cal_acc result.txt data/test_lbl.txt