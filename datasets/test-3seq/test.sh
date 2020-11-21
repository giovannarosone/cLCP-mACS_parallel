#!/bin/bash

## cLCP executable path
EXEC="../../bin//cLCP-mACS"

## Set the number of OpenMP concurrent threads
export OMP_NUM_THREADS=4

# -f : deletes (flushes)s .d and .d_pairs files
# -L : each thread prints the cLCP arrays within its log file (thread_i.out.txt)
# -S : master thread prints the pairwise scores and value of ACS
# -I0: input format 0=BCR
# -Q : buffer size in bytes
echo -n 'Executing OpenMP cLCP-mACS...'
$EXEC -f -I0 -Q 400000 3seq 1>stdout.log 2>stderr.log
echo 'DONE!'
echo 'ACS scores are to be found in the 3seq.acs file.'
echo 'To check for correctness run: diff 3seq.acs 3seq_serial_correct.acs.'
echo 'Statistics about single threads computing times are given in 3seq_thread_{i}.out.txt files.'
echo 'Bye.'
