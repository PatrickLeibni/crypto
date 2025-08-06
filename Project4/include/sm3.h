#ifndef SM3_H
#define SM3_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// SM3 constants
#define SM3_BLOCK_SIZE 64
#define SM3_DIGEST_SIZE 32
#define SM3_STATE_SIZE 8

// SM3 context structure
typedef struct {
    uint32_t state[SM3_STATE_SIZE];  // A, B, C, D, E, F, G, H
    uint64_t length;                 // message length in bits
    uint8_t buffer[SM3_BLOCK_SIZE];  // message buffer
    size_t buffer_size;              // current buffer size
} sm3_ctx_t;

// Basic SM3 implementation
void sm3_init(sm3_ctx_t *ctx);
void sm3_update(sm3_ctx_t *ctx, const uint8_t *data, size_t len);
void sm3_final(sm3_ctx_t *ctx, uint8_t *digest);
void sm3_hash(const uint8_t *data, size_t len, uint8_t *digest);

// Optimized implementations
void sm3_init_optimized(sm3_ctx_t *ctx);
void sm3_update_optimized(sm3_ctx_t *ctx, const uint8_t *data, size_t len);
void sm3_final_optimized(sm3_ctx_t *ctx, uint8_t *digest);
void sm3_hash_optimized(const uint8_t *data, size_t len, uint8_t *digest);

// SIMD optimized implementations (if available)
void sm3_init_simd(sm3_ctx_t *ctx);
void sm3_update_simd(sm3_ctx_t *ctx, const uint8_t *data, size_t len);
void sm3_final_simd(sm3_ctx_t *ctx, uint8_t *digest);
void sm3_hash_simd(const uint8_t *data, size_t len, uint8_t *digest);

// Length extension attack functions
typedef struct {
    uint8_t digest[SM3_DIGEST_SIZE];
    uint64_t length;
    uint8_t padding[SM3_BLOCK_SIZE];
} sm3_length_extension_state_t;

int sm3_length_extension_attack(const uint8_t *original_digest, 
                               const uint8_t *original_message, 
                               size_t original_len,
                               const uint8_t *extension, 
                               size_t extension_len,
                               uint8_t *new_digest);

// Padding calculation function
int sm3_calculate_padding_length(size_t message_len);

// Create padding for length extension attack
void create_padding(uint8_t *padding, size_t original_len, size_t *padding_len);

// Merkle tree structures and functions
typedef struct merkle_node {
    uint8_t hash[SM3_DIGEST_SIZE];
    struct merkle_node *left;
    struct merkle_node *right;
    struct merkle_node *parent;
    int is_leaf;
    int index;  // for leaf nodes
} merkle_node_t;

typedef struct {
    merkle_node_t *root;
    int leaf_count;
    int height;
} merkle_tree_t;

// Merkle tree functions
merkle_tree_t* merkle_tree_create(uint8_t **leaf_hashes, int leaf_count);
void merkle_tree_destroy(merkle_tree_t *tree);
uint8_t* merkle_tree_get_root_hash(merkle_tree_t *tree);

// Proof structures
typedef struct {
    uint8_t hash[SM3_DIGEST_SIZE];
    int is_left;  // 1 if this hash is from left child, 0 if from right
} merkle_proof_step_t;

typedef struct {
    merkle_proof_step_t *steps;
    int step_count;
    uint8_t leaf_hash[SM3_DIGEST_SIZE];
    int leaf_index;
} merkle_proof_t;

// Proof functions
merkle_proof_t* merkle_tree_create_existence_proof(merkle_tree_t *tree, int leaf_index);
merkle_proof_t* merkle_tree_create_nonexistence_proof(merkle_tree_t *tree, int leaf_index, uint8_t *target_hash);
int merkle_tree_verify_existence_proof(merkle_tree_t *tree, merkle_proof_t *proof);
int merkle_tree_verify_nonexistence_proof(merkle_tree_t *tree, merkle_proof_t *proof);
void merkle_proof_destroy(merkle_proof_t *proof);

// Utility functions
void sm3_print_digest(const uint8_t *digest);
void sm3_hex_to_bytes(const char *hex, uint8_t *bytes, size_t len);
void sm3_bytes_to_hex(const uint8_t *bytes, char *hex, size_t len);

// Performance testing
typedef struct {
    double basic_time;
    double optimized_time;
    double simd_time;
    size_t data_size;
} sm3_performance_result_t;

sm3_performance_result_t sm3_benchmark(const uint8_t *data, size_t len, int iterations);

#ifdef __cplusplus
}
#endif

#endif // SM3_H 