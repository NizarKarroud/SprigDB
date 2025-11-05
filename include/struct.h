#ifndef KV_STORE_H
#define KV_STORE_H

#include <stddef.h>

// ---------------- Return Codes ----------------
#define INSERT_OK 0
#define INSERT_ERR_KEY_DUPLICATE 1
#define INSERT_ERR_ALLOC_FAILED 2
#define INSERT_ERR_KEY_EMPTY 3
#define DELETE_OK 4
#define DELETE_ERR_KEY_NOT_FOUND 5

#define UPDATE_OK 6


#define RESIZE_OK 7
#define RESIZE_ERR_MEM_ALL 8

// ---------------- Value Types ----------------
typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STR,
    TYPE_CHAR,
} value_type_t;

// ---------------- Value Structure ----------------
typedef struct
{
    union
    {
        int int_val;
        float float_val;
        char char_val;
        char *str_val;
    } data;

    value_type_t type;
} Value;

// ---------------- KV Node ----------------
typedef struct kv_node
{
    char *key;
    Value value;
    struct kv_node *next;
} kv_node;

// ---------------- KV Hash Table ----------------
typedef struct
{
    kv_node **buckets;
    size_t size;
} kv_hash_table;

// ---------------- Hash Functions ----------------
unsigned long djb2(const char *str);
size_t hash_key(const char *key, size_t table_size);

// ---------------- Admin Password ----------------
char *hash_password(char *password);

// ---------------- KV Table Functions ----------------
kv_hash_table *init_kv_hash_table(size_t size);
void free_kv_hash_table(kv_hash_table *store);

int insert_pair(char *key, Value value, kv_hash_table *store);
kv_node *kv_node_search(kv_hash_table *store, char *key);
int delete_pair(char *key, kv_hash_table *store);
int update_value(kv_hash_table *store, char *key, Value new_value);

int resize_buckets_array(size_t new_size, kv_hash_table *store);
void free_kv_node(kv_node *node);

#endif // KV_STORE_H
