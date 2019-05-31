1. ensure that NC_000913.fasta has been downloaded and is in the same folder (or symlinked)
2. run `./produce_is.pl <folder>` to produce the family of genomic sequences
3. ensure the tools are built and the seed has been produced in `../omh_compute`
4. run `create_all_sketches.pl <folder>` to compute the necceasry k-mer sketches
5. ensure `RapidNJ` is installed and in your `$PATH`
6. run `compute_tree.pl <folder>` to compare sketches and build an neighbor joining tree
7. use your favorite visualizer to view the trees produced inside `<folder>/sketches/m*.l*.binary.newik`, where `l=1` is weighted jaccard and `l=3` is the OMH used in the paper. 
