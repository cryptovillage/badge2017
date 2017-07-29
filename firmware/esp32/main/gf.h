#include <stdint.h>

extern int gf_computeShards(uint8_t* key, size_t key_len, uint8_t** shards, size_t threshold, size_t numShards);
extern int gf_computeCombine(uint8_t* shares_x, uint8_t** shards, size_t numShard, size_t shardLength, uint8_t *out);
