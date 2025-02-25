Test of using std::mdspan to iterate over 2d data. 
Surprisingly, using std::mdspan was for me much slower than using std::vector<std::vector<T>>. 
In my head, it should have been faster, as it is just a view of a std::vector<T>, which will have contiguous memory (not guaranteed for a vector of vectors). 
