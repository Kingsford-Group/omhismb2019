1. run `./generate_random_pairs.py -fname <file>`
2. ensure the programs and seed are compiled/generated in `../omh_compute`
3. run `./run_omh_and_combine.pl <file> <file_withomh>`
4. run `./multibox.py -out_fname <img_file> -in_fname <file_with_omh>` to generate the figure 
