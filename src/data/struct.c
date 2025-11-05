#include <stddef.h>
#include <argon2.h>
#include <openssl/rand.h>
#include <string.h>
#include <logger.h>
#include <stdlib.h>


#define INSERT_OK 0
#define INSERT_ERR_KEY_DUPLICATE 1
#define INSERT_ERR_ALLOC_FAILED 2
#define INSERT_ERR_KEY_EMPTY 3
#define DELETE_OK 4
#define DELETE_ERR_KEY_NOT_FOUND 5

#define UPDATE_OK 6


#define RESIZE_OK 7
#define RESIZE_ERR_MEM_ALL 8


typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STR,
    TYPE_CHAR,
} value_type_t;

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

typedef struct kv_node
{
    char *key;
    Value value;
    struct kv_node *next;
} kv_node;

typedef struct
{
    kv_node **buckets;
    size_t size;
} kv_hash_table;

// ---------------- Hash functions ----------------
unsigned long djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

size_t hash_key(const char *key, size_t table_size)
{
    return djb2(key) % table_size;
}

// ---------------- Admin password ----------------
char *hash_password(char *password)
{
    unsigned char salt[16];
    RAND_bytes(salt, 16);
    uint8_t hash[32];
    char encoded[108];
    int result = argon2_hash(2, 1 << 12, 2, password, strlen(password),
                             salt, 16, hash, sizeof(hash), encoded, sizeof(encoded),
                             Argon2_i, ARGON2_VERSION_13);
    if (result == ARGON2_OK)
    {
        logger("INFO", "Admin password hash generated!\n");
        return strdup(encoded);
    }
    else
    {
        logger("ERROR", argon2_error_message(result));
        return NULL;
    }
}

// ---------------- KV functions ----------------
int insert_pair(char *key, Value value, kv_hash_table *store)
{
    if (key == NULL || key[0] == '\0'){
        logger("ERROR" , "empty key not allowed in hash table\n");
        return INSERT_ERR_KEY_EMPTY;
    }

    size_t hashed_key = hash_key(key, store->size);
    kv_node *current_node = store->buckets[hashed_key];
    kv_node *last_node = NULL;

    while (current_node != NULL)
    {
        if (strcmp(current_node->key, key) == 0)
        {
            logger("ERROR", "Pair insertion failed, duplicate key found\n");
            return INSERT_ERR_KEY_DUPLICATE;
        }
        last_node = current_node;
        current_node = current_node->next;
    }

    kv_node *node = malloc(sizeof(kv_node));
    if (!node)
    {
        logger("ERROR", "Pair insertion failed: memory allocation failed\n");
        return INSERT_ERR_ALLOC_FAILED;
    }

    node->key = strdup(key);
    node->value = value;
    node->next = NULL;

    if (!last_node)
    {
        store->buckets[hashed_key] = node;
    }
    else
    {
        last_node->next = node;
    }
    return INSERT_OK;
}

kv_node *kv_node_search(kv_hash_table *store, char *key)
{
    size_t hashed_key = hash_key(key, store->size);
    kv_node *current_node = store->buckets[hashed_key];

    while (current_node != NULL)
    {
        if (strcmp(current_node->key, key) == 0)
        {
            logger("INFO", "Found value for the given key\n");
            return current_node;
        }
        current_node = current_node->next;
    }

    logger("ERROR", "No value found for the given key\n");
    return NULL;
}

void free_kv_node(kv_node *node){

    free(node->key);
    if (node->value.type == TYPE_STR && node->value.data.str_val != NULL){
        free(node->value.data.str_val);
    }
    free(node);
}
int delete_pair(char *key , kv_hash_table *store){

    size_t hashed_key = hash_key(key, store->size);
    kv_node *current_node = store->buckets[hashed_key];
    kv_node *last_node = NULL;

    while (current_node != NULL)
    {
        if (strcmp(current_node->key, key) == 0)
        {
            if (last_node)
                last_node->next = current_node->next;
            else
                store->buckets[hashed_key] = current_node->next;

            free_kv_node(current_node);
            logger("INFO", "Pair Deleted\n");
            return DELETE_OK;
        }
        last_node = current_node;
        current_node = current_node->next;
    }

    logger("ERROR" , "Delete operation failed , key not found\n");
    return DELETE_ERR_KEY_NOT_FOUND;

}



int update_value(kv_hash_table *store , char *key , Value new_value){

    kv_node *node = kv_node_search(store , key);

    if (node->value.type == TYPE_STR && node->value.data.str_val != NULL) {
        free(node->value.data.str_val);
    }
    node->value = new_value;
    return UPDATE_OK;   

}


kv_hash_table *init_kv_hash_table(size_t size){

    kv_hash_table *store = malloc(sizeof(kv_hash_table));
    if (!store) {
        logger("ERROR", "Failed to allocate memory for KV hash table\n");
        return NULL;
    }

    store->buckets = calloc(size, sizeof(kv_node*));
    if (!store->buckets) {
        free(store);
        logger("ERROR", "Failed to allocate memory for buckets array\n");
        return NULL;
    }

    store->size = size;
    logger("INFO", "KV HASH TABLE INITIATED SUCCESSFULLY\n");
    return store;
}

int resize_buckets_array(size_t new_size , kv_hash_table *store){

    kv_node **new_buckets_array = realloc(store->buckets , new_size * sizeof(kv_node*));
    if (!new_buckets_array){
        return RESIZE_ERR_MEM_ALL;
    }


    if (new_size > store->size)
        for (size_t i = store->size; i < new_size; i++)
            new_buckets_array[i] = NULL;

    store->buckets = new_buckets_array;
    store->size = new_size;

    return RESIZE_OK;
}

void free_kv_hash_table(kv_hash_table *store) {

    for (size_t i = 0; i < store->size; i++) {

        kv_node *node = store->buckets[i];
        while (node) {
            kv_node *tmp = node;
            node = node->next;
            free_kv_node(tmp);
        }
    }
    free(store->buckets);
    free(store);
}
