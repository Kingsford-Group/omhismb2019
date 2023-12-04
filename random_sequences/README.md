1. run `./generate_random_pairs.py -fname <img_file> [-k <k>] > <sequence_file>` (run `./generate_random_pairs.py --help` for other parameter descriptions)
2. ensure the programs are compiled in `../omh_compute`
3. run `./run_omh_and_combine.pl <sequence_file> <sequence_file_with_omh> <k>` (note that if k is not specified in step 1, k=2)
4. run `./multibox.py -out_fname <img_file> -in_fname <sequence_file_with_omh>` to generate the figure 
