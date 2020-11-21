# cLCP-mACS_parallel

Efficient parallel construction of the colored longest common prefix array introduced in 
>F. Garofalo, G. Rosone, M. Sciortino and D. Verzotto
*The colored longest common prefix array computed via sequential scans.*
Proceedings of String Processing and Information Retrieval - 25th International Symposium, SPIRE 2018, Lecture Notes in Computer Science, Springer <sup id="a1">[1](#f1)</sup>

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
./bin/cLCP-mACS_parallel [-d] [-E] [-f] [-Q BUFFER_SIZE] -I0 target_collection
```

##### Input

`OMP_NUM_THREADS` sets the number of parallel threads used for the computation. 
*TO DO*

##### Output
*TO DO*


##### Options:

```sh
Usage: [-h] [-v] [-d] [-E] [-f] [-Q BUFFER_SIZE] [-I input_format] target_collection

    h:  help message

    v:  verbose output

    d:  skip D arrays generation step (files .d NEEDED)

    f:  delete intermediate files {i}.d and {i}.lcp after computing i-vs-all

    Q:  set BUFFER_SIZE (default 149488)

    I:  input format
        - 0 BCR file format (LCP + DA)
        - 1 EGSA file format (LCP + DA + SA + BWT) (to be implemented)
```

The option `-f` (flush) deletes the intermediate .d and .lcp right after the i-vs-all computation step involving the i-th sequence of our target collection. Each i-vs-all step is independent from all of the others, therefore at any time there are `OMP_NUM_THREADS` threads, each one working on a single i-vs-all step.

The option `-d` skip the previously described .d computation step: the program expects to find them already in the target_collection directory.

The option `-Q amount` dictates the amount of RAM (in Bytes) available to accomodate partial cLCP page. `amount` has to be at least 2 x _m_, where _m_ is the number of sequences in target collection.

The option `-I input_format` specifies the format of the input to cLCP-mACS. Currently, only `-I0` option is implemented (default) corresponding to the BCR file format as computed by <span style="color:red">*???*</span>


#### Citation
If you use cLCP-mACS or cLCP-mACS_parallel you could cite the following paper:

    @inproceedings{GRSV_Spire18,
    author    = {Garofalo, Fabio and Rosone, Giovanna and Sciortino, Marinella and Verzotto, Davide},
    title     = {The colored longest common prefix array computed via sequential scans},
    booktitle = {String Processing and Information Retrieval - 25th International Symposium,
               {SPIRE} 2018, Proceedings},
    pages     = {},
    year      = {2018},
    series    = {Lecture Notes in Computer Science},
    volume    = {},
    publisher = {Springer}
    }
    
<span style="color:red">*Daniele: penso che dovremmo aggiungere la conferenza in cui è stato presentato il lavoro ed il PDF della presentazione.*</span>
    
---
1. <small id="f1"> Supported by the project Italian MIUR-SIR CMACBioSeq ("_Combinatorial methods for analysis and compression of biological sequences_")
grant n.~RBSI146R5L.</small> [↩](#a1)
