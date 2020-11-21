# cLCP-mACS_parallel

Efficient parallel construction of the colored longest common prefix array introduced in 
>F. Garofalo, G. Rosone, M. Sciortino and D. Verzotto
*The colored longest common prefix array computed via sequential scans.*
Proceedings of String Processing and Information Retrieval - 25th International Symposium, SPIRE 2018, Lecture Notes in Computer Science, Springer <sup id="a1">[1](#f1)</sup>

Efficient lightweight strategy to solve the multi-string Average Common Substring (ACS) problem, that consists in the pairwise comparison of a single string against a collection of m strings simultaneously, in order to obtain m ACS induced distances.

### Install

```sh
git clone https://github.com/giovannarosone/cLCP-mACS_parallel.git
cd cLCP-mACS_parallel

```

### Run

```sh
./cLCP-mACS_parallel [-h] [-v] [-p] [-l] [-f input_format] [-Q amount] ref_seq target_seqs ref_color output
```

##### Input



##### Options:

```sh
-h    help message
-v    verbose textual output
```


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
    
    
---
1. <small id="f1"> Supported by the project Italian MIUR-SIR CMACBioSeq ("_Combinatorial methods for analysis and compression of biological sequences_")
grant n.~RBSI146R5L.</small> [↩](#a1)
