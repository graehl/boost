Improvements to boost::object_pool not (yet) merged into Boost mainline:

see also https://github.com/graehl/boost/tree/dynamic_bitset

object_pool free(ptr) O(1) instead of O(n) - actually O(lg n) amortized when accounting
for extra time spent on ~object-pool.

standalone (in Pool namespace instead of boost): https://github.com/graehl/Pool-object_pool
