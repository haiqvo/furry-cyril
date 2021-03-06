// $Id: hashset.c,v 1.1 2012-12-04 23:41:47-08 - - $

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugf.h"
#include "hashset.h"
#include "strhash.h"

#define HASH_NEW_SIZE 15

struct hashset {
   size_t length;
   int load;
   char **array;
};

hashset_ref new_hashset (void) {
   hashset_ref new = malloc (sizeof (struct hashset));
   assert (new != NULL);
   new->length = HASH_NEW_SIZE;
   new->load = 0;
   new->array = malloc (new->length * sizeof (char*));
   assert (new->array != NULL);
   for (size_t index = 0; index < new->length; ++index) {
      new->array[index] = NULL;
   }
   DEBUGF ('h', "%p -> struct hashset {length = %d, array=%p}\n",
                new, new->length, new->array);
   return new;
}

void free_hashset (hashset_ref hashset) {
   DEBUGF ('h', "free (%p), free (%p)\n", hashset->array, hashset);
   for(size_t index = 0; index<hashset->length; index++){
       free(hashset->array[index]);
   }
   memset (hashset->array, 0, hashset->length * sizeof (char*));
   free (hashset->array);
   memset (hashset, 0, sizeof (struct hashset));
   free (hashset);
}

void put_hashset (hashset_ref hashset, char *item) {
   if(has_hashset(hashset, item)){
       return;
   }else{
       char *word = strdup(item);
       size_t index = strhash(item) % hashset->length;
       while(hashset->array[index] != NULL){
           index = (index+1) % hashset->length;
       }
       hashset->array[index] = word;
       hashset->load++;
       if(hashset->load*4>(int)hashset->length){
           char **newarray = malloc((hashset->length*2+1)
                             * sizeof (char*));
           for (size_t index = 0; index < hashset->length; ++index){
               if(hashset->array[index] != NULL){
                   char *temp = hashset->array[index];
                   size_t starting_index = strhash(temp)%
                           (hashset->length*2+1);
                   newarray[starting_index] = hashset->array[index];
               }
           }
           hashset->length = (2*hashset->length)+1;
           free(hashset->array);
           hashset->array = newarray;
       }
       
   }
   
}

void debug(hashset_ref hashset, int xopt_coounter){
    printf("%d words in the hash set.\n%d length of "
            "the hash array\n", hashset->load, (int)hashset->length);
    int clusters [hashset->load];
    for(int i = 0; i<hashset->load; i++){
        clusters[i] = 0;
    }
    int count = 0;
    for(size_t j = 0; j<hashset->length; j++){
        if(hashset->array[j] != NULL){
            ++count;
            continue;
        }
        if(count>0){
           ++clusters[count];
        }
        count = 0;
    }
    for(int k = 0; k<hashset->load; k++){
        if(clusters[k] != 0){
            printf("%d clusters of size %d\n", 
                    clusters[k],k);
        }
    }
    if(xopt_coounter>1){
        for(size_t l=0; l<hashset->length; l++){
           if(hashset->array[l] != NULL){
              printf("array[%10d] = %12u \"%s\"\n", 
                   (int)l, strhash(hashset->array[l]),
                      hashset->array[l]);
           }
        }
    }
}

bool has_hashset (hashset_ref hashset, char *item) {
    size_t index = strhash(item) % hashset->length;
    for(;;){
        if(hashset->array[index] == NULL){
            return false;
        }
        if(strcmp(hashset->array[index], item) == 0){
            return true;
            
        }
        index = (index+1) % hashset->length;
    }
   return false;
}

