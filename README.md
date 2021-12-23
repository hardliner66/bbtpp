# bbtpp
C++ port of [bbt](https://github.com/DataWraith/bbt)

# Differences to the original
- uses exceptions instead of Result
- uses asserts for invariant checks
- limits mu so it can't go negative or above the maximum points
