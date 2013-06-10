Improvements to dynamic_bitset not (yet) merged into Boost mainline:

see also https://github.com/graehl/boost/tree/object_pool-constant-time-free

    dynamic_bitset: rfind_first/next, blocks access

    add to dynamic_bitset: rfind_first rfind_next (msb instead of lsb - 'rfind' for
    reverse of find), blocks(), blocks_end(), block(i) - for efficient external
    serialization, hash_value, and other bit-parallelizable algorithms.

    add docs and tests

    optional compiler intrinsics count leading / trailing zeros instead of
    integer_log2 (see boost/pending/lowest_bit)


