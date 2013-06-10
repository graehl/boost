object_pool free(ptr) O(1) instead of O(n) - actually O(lg n) amortized when accounting
for extra time spent on ~object-pool.

see also standalone (in Pool namespace instead of boost)  

https://github.com/graehl/Pool-object_pool
