# reuse_distance

Run '/ScaleTree/analyze.c'  
Remember to set the datasource in line 90 'std::ifstream in("/home/zhiwei2/data_row_2.txt");'   
Remember to set the result address in line 107 '_PrintResults("./result_whole.txt");'  
'ScaleTree/result.txt' is the result for 10% indices in '/dlrm_datasets/embedding_bag/fbgemm_t856_bs65536_15.pt'  
'ScaleTree/result_whole.txt' is the result for all indices  


Sample Input:   
 116024  
 123524  
 523441  
 562352    
 ...  
 ...  
 ...  

Sample result:  
  Total data is 7202  
  Total access is 2798  
  Distance 0     100  
  Distance 0 to 1        13  
  Distance 2 to 3        39  
  Distance 4 to 7        27  
  Distance 8 to 15       29  
  Distance 16 to 31      26  
  Distance 32 to 63      42  
  Distance 64 to 127     44  
  Distance 128 to 255    59  
  Distance 256 to 511    104  
  Distance 512 to 1023   168  
  Distance 1024 to 2047  274  
  Distance 2048 to 3071  160  
  Distance 3072 to 4095  114  
  Distance 4096 to 5119  78  
  Distance 5120 to 6143  58  
  Distance 6144 to 7167  1463  
  End tree size is 3499  
