#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CUTS 100

// Structure to match rods.c implementation
typedef struct {
  int length;
  int value;
} length_value_pair;

// Structure for rod cutting results
typedef struct {
  int rod_length;
  int num_cuts;
  length_value_pair cuts[MAX_CUTS]; // Store the original cuts array
  int cut_counts[MAX_CUTS];         // Store how many times each length was used
  int total_value;
  int remainder;
} RodResult;

// Structure for cache entries
typedef struct {
  int rod_length;    // Key is the rod length
  RodResult *result; // Value is the rod cutting result
  int is_valid;      // Flag to indicate if entry is used
} CacheEntry;

// Structure for the Random Replacement cache
typedef struct {
  CacheEntry *entries; // Array of cache entries
  int capacity;
  int size;
} RandomCache;

// Function to create a new rod result
RodResult *createRodResult(int rod_length, length_value_pair cuts[],
                           int num_cuts, int cut_counts[], int total_value,
                           int remainder) {
  RodResult *result = (RodResult *)malloc(sizeof(RodResult));
  if (!result)
    return NULL;

  result->rod_length = rod_length;
  result->num_cuts = num_cuts;
  result->total_value = total_value;
  result->remainder = remainder;

  // Copy both the cuts array and cut counts
  memcpy(result->cuts, cuts, sizeof(length_value_pair) * num_cuts);
  memcpy(result->cut_counts, cut_counts, sizeof(int) * MAX_CUTS);

  return result;
}

// Function to create a new Random Replacement cache
RandomCache *createCache(int capacity) {
  RandomCache *cache = (RandomCache *)malloc(sizeof(RandomCache));
  if (!cache)
    return NULL;

  cache->entries = (CacheEntry *)calloc(capacity, sizeof(CacheEntry));
  if (!cache->entries) {
    free(cache);
    return NULL;
  }

  cache->capacity = capacity;
  cache->size = 0;

  // Initialize random seed
  srand(time(NULL));
  return cache;
}

// Function to get a value from the cache
RodResult *get(RandomCache *cache, int rod_length) {
  if (!cache)
    return NULL;

  for (int i = 0; i < cache->capacity; i++) {
    if (cache->entries[i].is_valid &&
        cache->entries[i].rod_length == rod_length) {
      return cache->entries[i].result;
    }
  }
  return NULL;
}

// Function to find an empty slot or randomly select one to replace
int findSlot(RandomCache *cache) {
  // First try to find an empty slot
  for (int i = 0; i < cache->capacity; i++) {
    if (!cache->entries[i].is_valid) {
      return i;
    }
  }

  // If no empty slot, randomly select one to replace
  return rand() % cache->capacity;
}

// Function to put a key-value pair in the cache
void put(RandomCache *cache, int rod_length, RodResult *result) {
  if (!cache || !result)
    return;

  // Check if key already exists
  for (int i = 0; i < cache->capacity; i++) {
    if (cache->entries[i].is_valid &&
        cache->entries[i].rod_length == rod_length) {
      // Update existing entry
      free(cache->entries[i].result);
      cache->entries[i].result = result;
      return;
    }
  }

  // Find a slot (empty or random)
  int slot = findSlot(cache);

  // If replacing an existing entry, free its memory
  if (cache->entries[slot].is_valid) {
    free(cache->entries[slot].result);
  } else {
    cache->size++;
  }

  // Insert new entry
  cache->entries[slot].rod_length = rod_length;
  cache->entries[slot].result = result;
  cache->entries[slot].is_valid = 1;
}

// Function to free the cache
void freeCache(RandomCache *cache) {
  if (!cache)
    return;

  for (int i = 0; i < cache->capacity; i++) {
    if (cache->entries[i].is_valid) {
      free(cache->entries[i].result);
    }
  }
  free(cache->entries);
  free(cache);
}

// Function to print cache contents
void printCache(RandomCache *cache) {
  if (!cache) {
    printf("Cache is NULL\n");
    return;
  }

  printf("Cache contents (capacity: %d, size: %d):\n", cache->capacity,
         cache->size);

  for (int i = 0; i < cache->capacity; i++) {
    if (cache->entries[i].is_valid) {
      printf("\nSlot %d (Rod Length: %d):\n", i, cache->entries[i].rod_length);

      // Print cuts in the same format as rods.c
      for (int j = 0; j < cache->entries[i].result->num_cuts; j++) {
        int count = cache->entries[i].result->cut_counts[j];
        if (count > 0) {
          printf("%d @ %d = %d\n", count,
                 cache->entries[i].result->cuts[j].length,
                 count * cache->entries[i].result->cuts[j].value);
        }
      }
      printf("Remainder: %d\n", cache->entries[i].result->remainder);
      printf("Value: %d\n", cache->entries[i].result->total_value);
    }
  }
  printf("\n");
}

// Example usage
int main() {
  // Create a cache with capacity 3
  RandomCache *cache = createCache(3);
  if (!cache) {
    printf("Failed to create cache\n");
    return 1;
  }

  // First, handle input like rods.c
  int rod_length;
  printf("Enter the rod length: \n");
  scanf("%d", &rod_length);

  length_value_pair cuts[MAX_CUTS];
  int num_cuts = 0;

  // Take length-value pairs as input until EOF
  printf("Enter the cut length and value as 'length, value':\n");
  while (num_cuts < MAX_CUTS &&
         scanf("%d, %d", &cuts[num_cuts].length, &cuts[num_cuts].value) == 2) {
    num_cuts++;
  }

  // Now start caching after EOF
  int cut_counts[MAX_CUTS] = {0};
  int remaining_length = rod_length;
  int total_value = 0;

  // Try to get result from cache first
  RodResult *cached_result = get(cache, rod_length);
  if (cached_result) {
    // Print cached result
    for (int i = 0; i < cached_result->num_cuts; i++) {
      if (cached_result->cut_counts[i] > 0) {
        printf("%d @ %d = %d\n", cached_result->cut_counts[i],
               cached_result->cuts[i].length,
               cached_result->cut_counts[i] * cached_result->cuts[i].value);
      }
    }
    printf("Remainder: %d\n", cached_result->remainder);
    printf("Value: %d\n", cached_result->total_value);
    return 0;
  }

  // If not in cache, calculate solution
  while (remaining_length > 0) {
    int best_cut = -1;
    int max_value = 0;

    // Find best cut for current remaining length
    for (int i = 0; i < num_cuts; i++) {
      if (cuts[i].length <= remaining_length) {
        int possible_value = cuts[i].value;
        if (possible_value > max_value) {
          max_value = possible_value;
          best_cut = i;
        }
      }
    }

    if (best_cut == -1) {
      break; // No valid cut found
    }

    // Record this cut
    cut_counts[best_cut]++;
    remaining_length -= cuts[best_cut].length;
    total_value += cuts[best_cut].value;
  }

  // Create result and add to cache
  RodResult *result = createRodResult(rod_length, cuts, num_cuts, cut_counts,
                                      total_value, remaining_length);
  put(cache, rod_length, result);

  // Print results
  for (int i = 0; i < num_cuts; i++) {
    if (cut_counts[i] > 0) {
      printf("%d @ %d = %d\n", cut_counts[i], cuts[i].length,
             cut_counts[i] * cuts[i].value);
    }
  }
  printf("Remainder: %d\n", remaining_length);
  printf("Value: %d\n", total_value);

  // Free the cache
  freeCache(cache);
  return 0;
}
