#include <stddef.h>
#include <argon2.h>
#include <openssl/rand.h>
#include <string.h>
#include <logger.h>

#define USER_OK 0
#define USER_ERR_DUPLICATE 1
#define USER_ERR_ALLOC_FAILED 2
#define USER_ERR_INVALID_INPUT 3

#define INSERT_OK 0
#define INSERT_ERR_KEY_DUPLICATE 4
#define INSERT_ERR_ALLOC_FAILED 5

typedef enum
{

    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STR,
    TYPE_CHAR,

} value_type_t;

typedef enum
{

    ROLE_USER,
    ROLE_ADMIN
} Role;

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

typedef struct user_node
{
    char *username;
    char *password;
    Role role;
    kv_hash_table store;
    struct user_node *next;

} user_node;

typedef struct
{
    user_node **buckets;
    size_t size;
} global_user_table;

unsigned long djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

size_t hash_username(const char *username, size_t table_size)
{

    return djb2(username) % table_size;
}

size_t hash_key(const char *key, size_t table_size)
{
    return djb2(key) % table_size;
}

char *hash_password(char *password)
{

    unsigned char salt[16];
    RAND_bytes(salt, 16);
    uint8_t hash[32];
    char encoded[108];
    int result = argon2_hash(2, 1 << 12, 2, password, strlen(password), salt, 16, hash, sizeof(hash), encoded, sizeof(encoded), Argon2_i, ARGON2_VERSION_13);
    if (result == ARGON2_OK)
    {
        logger("INFO", "Hash generated!\n");
        return encoded;
    }
    else
    {
        logger("ERROR", argon2_error_message(result));
        return NULL;
    }
}

int insert_pair(char *key, Value value, user_node *user)
{

    size_t hashed_key = hash_key(key, user->store.size);
    kv_node *current_node = user->store.buckets[hashed_key];
    kv_node *last_node = NULL;

    while (current_node != NULL)
    {

        if (strcmp(current_node->key, key) == 0)
        {
            logger("ERROR", "Pair insertion failed , Duplicate key found");
            return INSERT_ERR_KEY_DUPLICATE;
        }

        last_node = current_node;
        current_node = current_node->next;
    }

    kv_node *node = malloc(sizeof(kv_node));
    if (node == NULL)
    {
        logger("ERROR", "Pair insertion failed: memory allocation failed");
        return INSERT_ERR_ALLOC_FAILED;
    }

    node->key = strdup(key);
    node->value = value;
    node->next = NULL;
    if (last_node == NULL)
    {
        // Bucket was empty this is the first insert
        user->store.buckets[hashed_key] = node;
    }
    else
    {
        last_node->next = node;
    }
    return INSERT_OK;
}

user_node *user_search(char *username, global_user_table *user_store)
{

    size_t hashed_username = hash_username(username, user_store->size);
    user_node *current_node = user_store->buckets[hashed_username];

    while (current_node != NULL)
    {

        if (strcmp(current_node->username, username) == 0)
        {
            return current_node;
        }

        current_node = current_node->next;
    }
    return NULL;
}

kv_node *kv_node_search(user_node *user, char *key)
{

    size_t hashed_key = hash_key(key, user->store.size);
    kv_node *current_node = user->store.buckets[hashed_key];

    while (current_node != NULL)
    {

        if (strcmp(current_node->key, key) == 0)
        {
            logger("INFO", "Found value for the given key");
            return current_node;
        }

        current_node = current_node->next;
    }
    logger("INFO", "No value found for the given key");

    return NULL ;
}


