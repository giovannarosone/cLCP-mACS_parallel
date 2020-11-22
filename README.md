# cLCP-mACS_parallel

Efficient parallel construction of the colored longest common prefix array introduced in 
>F. Garofalo, G. Rosone, M. Sciortino and D. Verzotto
*The colored longest common prefix array computed via sequential scans.*
Proceedings of String Processing and Information Retrieval - 25th International Symposium, SPIRE 2018, Lecture Notes in Computer Science, Springer <sup id="a1">[1](#f1)</sup>
and in
>F. Garofalo, D. Greco, G. Rosone, M. Sciortino
*Parallel Computation of Matching Statistics and Average Common Substring.*
BITS 2019 <sup id="a1">[1](#f1)</sup>

Efficient lightweight strategy to solve the multi-string Average Common Substring (ACS) problem, that consists in the pairwise comparison of a single string against a collection of m strings simultaneously, in order to obtain m ACS induced distances.

**Note that this is only a prototypal version. A production release are yet under development.**

### Install

#### Step 1: Check or install OpenMP

GCC provides built-in support for OpenMP starting from version 4.2.0. So if the system has GCC compiler with the version higher than 4.2.0, then it must have OpenMP features configured with it. To check the GCC version of the compiler:

```sh
gcc --version
```

If compilation fails due to OpenMP issues, please refer to [this configuration guide](https://www.geeksforgeeks.org/openmp-introduction-with-installation-guide/).


#### Step 2: Compile and install cLCP-mACS_parallel

```sh
git clone https://github.com/giovannarosone/cLCP-mACS_parallel.git
cd cLCP-mACS_parallel
make clean
make all
```

If everything works fine, you should find the `cLCP-mACS_parallel` binary into the `bin/` directory.

### Run

```sh
export OMP_NUM_THREADS=4 
./bin/cLCP-mACS_parallel [-d] [-f] [-Q BUFFER_SIZE] -I0 target_collection
```

#### Input

For example, let us compute ACS scores for a test set of 3 sequences found in `datasets/test-3seq`:

```sh
cd datasets/test-3seq
export OMP_NUM_THREADS=4 
../../bin/cLCP-mACS_parallel -f 3seq
```

Here, our target collection is `3seq` corresponding to a set of 3 files: 

* `3seq.lcpDa`
* `3seq.info`
* `3seq.len`

which are computed using for instance BCR_LCP_GSA (more details soon) tool on the `datasets/test-3seq/3seq.fasta` FASTA file.

The environment variable `OMP_NUM_THREADS` sets the number of parallel threads used for the computation. 

Finally, we specify the `-f` option for flushing intermediate computation files (recommended because they can grow large in proportion to the number and length of sequences within our target collection)-


#### Output

If `-f`is specified (as in our case), we end up with the following files:

```sh
-rw-r--r--   1 user  staff    27B 21 Jul 18:06 3seq.acs
-rw-r--r--   1 user  staff   189B 21 Jul 18:06 3seq_thread_0.out.txt
-rw-r--r--   1 user  staff   189B 21 Jul 18:06 3seq_thread_1.out.txt
-rw-r--r--   1 user  staff   189B 21 Jul 18:06 3seq_thread_2.out.txt
-rw-r--r--   1 user  staff     0B 21 Jul 18:06 3seq_thread_3.out.txt
```

`3seq.acs` contains the ACS scores matrix (more details soon), whereas `3seq_thread_{j}.out.txt` depicts some time statistics for the j-th thread performing the D array computation and the actual i-vs-all scores computation related to the i-th sequence.

If `-f` is omitted, then we also have:

```sh
-rw-r--r--   1 user  staff    80B 21 Jul 18:19 3seq0.d_pairs
-rw-r--r--   1 user  staff    24B 21 Jul 18:19 3seq0.lcp
-rw-r--r--   1 user  staff    64B 21 Jul 18:19 3seq1.d_pairs
-rw-r--r--   1 user  staff    24B 21 Jul 18:19 3seq1.lcp
-rw-r--r--   1 user  staff    80B 21 Jul 18:19 3seq2.d_pairs
-rw-r--r--   1 user  staff    24B 21 Jul 18:19 3seq2.lcp
```

`.d_pairs` and `.lcp` files for each ACS i-vs-all computation step.


#### Options:

```sh
Usage: [-h] [-v] [-d] [-E] [-f] [-Q BUFFER_SIZE] [-I input_format] target_collection

    h:  help message

    v:  verbose output

    d:  skip D arrays generation step (files .d NEEDED)

    f:  delete intermediate files {i}.d and {i}.lcp after computing i-vs-all

    Q:  set BUFFER_SIZE (default 149488)

    I:  input format
        - 0 BCR file format (LCP+DA)
        - 1 EGSA file format (LCP + DA + SA + BWT) (to be implemented)
```

The option `-f` (flush) deletes the intermediate .d and .lcp right after the i-vs-all computation step involving the i-th sequence of our target collection. Each i-vs-all step is independent from all of the others, therefore at any time there are `OMP_NUM_THREADS` threads, each one working on a single i-vs-all step.

The option `-d` skip the previously described .d computation step: the program expects to find them already in the target_collection directory.

The option `-Q amount` dictates the amount of RAM (in Bytes) available to accomodate partial cLCP page. `amount` has to be at least 2 x _m_, where _m_ is the number of sequences in target collection.

The option `-I input_format` specifies the format of the input to cLCP-mACS. Currently, only `-I0` option is implemented (default) corresponding to the BCR file format as computed by **???** (more details soon). Specifically, `cLCP-mACS_parllel` expects to find `target_collection.lcpDA`, `target_collection.len` and `target_collection.info` files in the current directory (see Input, Output sections above).


#### Citation
If you use cLCP-mACS or cLCP-mACS_parallel you could cite the following paper:

    @inproceedings{GGRS_BITS2019,
    author    = {Garofalo, Fabio and Greco, Daniele and Rosone, Giovanna and Sciortino, Marinella},
    title     = {Parallel Computation of Matching Statistics and Average Common Substring},
    booktitle = {Abstract of the BITS2019-Analysis of Big Omics Data Algorithms and Tools for the Analysis of Big Omics Data,
               {BITS} 2019},
    pages     = {},
    year      = {2019},
    }

and

    @inproceedings{GRSV_Spire18,
    author    = {Garofalo, Fabio and Rosone, Giovanna and Sciortino, Marinella and Verzotto, Davide},
    title     = {The colored longest common prefix array computed via sequential scans},
    booktitle = {String Processing and Information Retrieval - 25th International Symposium,
               {SPIRE} 2018, Proceedings},
    pages     = {153--167},
    year      = {2018},
    series    = {Lecture Notes in Computer Science},
    publisher = {Springer International Publishing}
    }
    

---
1. <small id="f1"> Supported by the project Italian MIUR-SIR CMACBioSeq ("_Combinatorial methods for analysis and compression of biological sequences_")
grant n.~RBSI146R5L.</small> [↩](#a1)
