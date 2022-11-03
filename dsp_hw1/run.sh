#!/bin/bash
n_iter=100
for ((i=1; i<=5; i++))
do
    echo "Running $i"
    ./train ${n_iter} model_init.txt data/train_seq_0${i}.txt model_0${i}.txt
done

./test modellist.txt data/test_seq.txt result.txt