Answers documents contains photos of graphs and tables and answers to questions, as well as notes etc. Stats folder contains the 3 stats files. 
These are the commands use for perf and gem5. Included as well is the excel file with all the graphs and tables, for better examination, since the pdf contaisn only images.


Commands:
perf stat -e instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branch-instructions,branch-misses bin/BFS inputs/RL5k.graph
perf stat -e instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branch-instructions,branch-misses bin/BFS_opt inputs/RL5k.graph
perf stat -e instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branch-instructions,branch-misses bin/MST inputs/rand-weighted-small.graph
perf stat -e instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branch-instructions,branch-misses bin/MST_opt inputs/rand-weighted-small.graph
perf stat -e instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branch-instructions,branch-misses bin/sha inputs/example-sha-input.txt
perf stat -e instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branch-instructions,branch-misses bin/sha_opt inputs/example-sha-input.txt
perf stat -e instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branch-instructions,branch-misses bin/queens -c 10
perf stat -e instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branch-instructions,branch-misses bin/queens_opt -c 10




Commands for gem5:
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=128kB --l1d_size=64kB --l2cache --l2_size=512kB --l2_tags=”LRU()” -c ../benchmark/bin/BFS_opt -o “benchmark/inputs/RL5k.graph”
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=128kB --l1d_size=64kB --l2cache --l2_size=512kB --l2_tags=”RandomRepl()” -c ../benchmark/bin/BFS_opt -o “benchmark/inputs/RL5k.graph”
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=128kB --l1d_size=64kB --l2cache --l2_size=512kB --l2_tags=”LRU()” -c ../benchmark/bin/MST_opt -o “inputs/rand-weighted-small.graph”
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=128kB --l1d_size=64kB --l2cache --l2_size=512kB --l2_tags=”LRU()” -c ../benchmark/bin/sha_opt -o “inputs/example-sha-input.txt”
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=128kB --l1d_size=64kB --l2cache --l2_size=512kB --l2_tags=”RandomRepl()” -c ../benchmark/bin/sha_opt -o “inputs/example-sha-input.txt”
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=128kB --l1d_size=64kB --l2cache --l2_size=512kB --l2_tags=”LRU()” -c ../benchmark/bin/queens_opt -o “-c 10”
./build/X86/gem5.opt configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=128kB --l1d_size=64kB --l2cache --l2_size=512kB --l2_tags=”RandomRepl()” -c ../benchmark/bin/queens_opt -o “-c 10”
