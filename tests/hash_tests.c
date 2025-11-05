#include "struct.h"   
#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>




// ---------------- Hash functions Tests ----------------

Test(hashing_key , determinism){
    unsigned long first_hash = djb2("hello");
    unsigned long second_hash = djb2("hello");

    cr_assert( first_hash == second_hash && second_hash == 210714636441);
}

    
Test(hashing_key , collision){

    unsigned long hashed_key1 = djb2("abc");
    unsigned long hashed_key2 = djb2("abd");
    cr_assert_neq(hashed_key1 , hashed_key2);

}


Test(hashing_key , length_variation){
    unsigned long short_hash = djb2("aaaa");
    unsigned long long_hash = djb2("aaaaaa");
    
    cr_assert_neq(short_hash , long_hash);

}


Test(hashing_key , case_sensitivity){
    unsigned long lower_case = djb2("test");
    unsigned long upper_case = djb2("TEST");

    cr_assert_neq(lower_case , upper_case);
}


Test(hashing_key , performance_test){

    size_t large_size = 1024 * 1024 * 10 *10; // 100 MB
    char *huge_key = malloc(large_size + 1);
    memset(huge_key, 'A', large_size); 
    huge_key[large_size] = '\0';  
    clock_t t; 
    t = clock(); 
    unsigned long huge_hash = djb2(huge_key);
    t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC; 

    cr_log_info("Time taken to hash a 100 MB key : %f sec" , time_taken);
}