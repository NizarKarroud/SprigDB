#include "struct.h"   
#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>



#define NUM_BUCKETS 1000
#define NEW_BUCKETS_SIZE 1100

// void setup(void) {

//     kv_hash_table *store = init_kv_hash_table(NUM_BUCKETS);

// }

// void teardown(void) {

// }




// TestSuite(db, .init = setup, .fini = teardown);

Test(kv_hash_table , init){
    kv_hash_table *store = init_kv_hash_table(NUM_BUCKETS);
    cr_assert_not_null(store);
}


Test(kv_hash_table , init_0){
    kv_hash_table *store = init_kv_hash_table(0);
    cr_assert_null(store);
}

Test(kv_hash_table , insertion){

    kv_hash_table *store = init_kv_hash_table(NUM_BUCKETS);

    Value val1 = {.type = TYPE_INT}; val1.data.int_val = 42;
    Value val2 = {.type = TYPE_STR}; val2.data.str_val = strdup("hello");
    Value val3 = {.type = TYPE_FLOAT}; val3.data.float_val = 3.14f;
    Value val4 = {.type = TYPE_CHAR}; val1.data.char_val = 'C';

    cr_assert_eq(insert_pair("key1", val1, store), INSERT_OK);
    cr_assert_eq(insert_pair("key2", val2, store), INSERT_OK);
    cr_assert_eq(insert_pair("key3", val3, store), INSERT_OK);
    cr_assert_eq(insert_pair("key4", val4, store), INSERT_OK);

}



Test(kv_hash_table , resize_buckets_empty){

    kv_hash_table *store = init_kv_hash_table(NUM_BUCKETS);
    int result = resize_buckets_array(NEW_BUCKETS_SIZE , store);
    cr_assert_neq(result,RESIZE_ERR_MEM_ALL);
}


Test(kv_hash_table, resize_buckets_not_empty) {
    kv_hash_table *store = init_kv_hash_table(NUM_BUCKETS);

    Value val1 = {.type = TYPE_INT}; val1.data.int_val = 42;
    Value val2 = {.type = TYPE_STR}; val2.data.str_val = strdup("hello");
    Value val3 = {.type = TYPE_FLOAT}; val3.data.float_val = 3.14f;

    cr_assert_eq(insert_pair("key1", val1, store), INSERT_OK);
    cr_assert_eq(insert_pair("key2", val2, store), INSERT_OK);
    cr_assert_eq(insert_pair("key3", val3, store), INSERT_OK);

    int result = resize_buckets_array(NEW_BUCKETS_SIZE, store);

    cr_assert_neq(result, RESIZE_ERR_MEM_ALL);


}

Test (kv_hash_table , rehashing_in_resize){


}