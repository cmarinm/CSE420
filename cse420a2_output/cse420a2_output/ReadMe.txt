Cesar Marin 
CSE 420 Assignment 2

AnswersTablesGraphs folder contains the anaylsis pdf and the graphs.

Outputfiles folder contains the config and stats files for each of the runs, separated by workload folders, each file is named after the replacement algorithm used that run.

Source files folder contains the new files added to gem5 and well as original gem5 modified files. Does not contain files modified from a1 (like process.cc etc).

------------------------------------------------------------------------------------------------------
Commands used in gem5:

--------------------------------- BFS ---------------------------------------------------
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="LRU()" -c benchmark/bin/BFS_opt -o "benchmark/inputs/RL5k.graph"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="RandomRepl()" -c benchmark/bin/BFS_opt -o "benchmark/inputs/RL5k.graph"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="PLRU()" -c benchmark/bin/BFS_opt -o "benchmark/inputs/RL5k.graph"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="LRUIPV()" -c benchmark/bin/BFS_opt -o "benchmark/inputs/RL5k.graph"

--------------------------------- MST ---------------------------------------------------
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="LRU()" -c benchmark/bin/MST_opt -o "benchmark/inputs/rand-weighted-small.graph"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="RandomRepl()" -c benchmark/bin/MST_opt -o "benchmark/inputs/rand-weighted-small.graph"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="PLRU()" -c benchmark/bin/MST_opt -o "benchmark/inputs/rand-weighted-small.graph"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="LRUIPV()" -c benchmark/bin/MST_opt -o "benchmark/inputs/rand-weighted-small.graph"

--------------------------------- SHA ---------------------------------------------------
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="LRU()" -c benchmark/bin/sha_opt -o "benchmark/inputs/example-sha-input.txt"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="RandomRepl()" -c benchmark/bin/sha_opt -o "benchmark/inputs/example-sha-input.txt"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="PLRU()" -c benchmark/bin/sha_opt -o "benchmark/inputs/example-sha-input.txt"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="LRUIPV()" -c benchmark/bin/sha_opt -o "benchmark/inputs/example-sha-input.txt"

--------------------------------- QUEENS ------------------------------------------------
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="LRU()" -c benchmark/bin/queens_opt -o "-c 10"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="RandomRepl()" -c benchmark/bin/queens_opt -o "-c 10"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="PLRU()" -c benchmark/bin/queens_opt -o "-c 10"

./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=256kB --l2_assoc=16  --l2_tags="LRUIPV()" -c benchmark/bin/queens_opt -o "-c 10"







