Cesar Marin
CSE 420 Assignment 3
Commands used:

Group 1:
For different predictors in each of the benchmarks, the predictor was changed in O3CPU.py, but
the commands are the same
-------------------BFS----------------------
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c benchmark/bin/BFS_opt -o "benchmark/inputs/RL5k.graph"

-------------------MST----------------------
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c benchmark/bin/MST_opt -o "benchmark/inputs/rand-weighted-small.graph"

-------------------SHA----------------------
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c benchmark/bin/sha_opt -o "benchmark/inputs/example-sha-input.txt"

-------------------QUEENS-------------------
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c benchmark/bin/queens_opt -o "-c 10"

--------------------------------------------

For group 2 the arguments where the same, O3CPU.py changed for every predictor, and for G2
BranchPredictor.py is changed to change the sizes of local/global registers,etc.
