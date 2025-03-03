#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CUTS 100

// Structure for rod cutting results
typedef struct {
  int rod_length;
  int num_cuts;
  int cut_counts[MAX_CUTS];
  int total_value;
  int remainder;
} RodResult;

// Structure for cache entries
typedef struct CacheEntry {
  int rod_length;
  RodResult *result;
  time_t last_used;
  struct CacheEntry *next;
  struct CacheEntry *prev;
} CacheEntry;

// Structure for the LRU cache
typedef struct {
  CacheEntry *head; // Most recently used
  CacheEntry *tail; // Least recently used
  int capacity;
  int size;
} LRUCache;

// Function to create a new rod result
RodResult *createRodResult(int rod_length, int num_cuts, int cut_counts[],
                           int total_value, int remainder) {
  RodResult *result = (RodResult *)malloc(sizeof(RodResult));
  if (!result)
    return NULL;

  result->rod_length = rod_length;
  result->num_cuts = num_cuts;
  result->total_value = total_value;
  result->remainder = remainder;
  memcpy(result->cut_counts, cut_counts, sizeof(int) * MAX_CUTS);

  return result;
}

// Function to create a new cache entry
CacheEntry *createEntry(int rod_length, RodResult *result) {
  CacheEntry *entry = (CacheEntry *)malloc(sizeof(CacheEntry));
  if (!entry)
    return NULL;

  entry->rod_length = rod_length;
  entry->result = result;
  entry->last_used = time(NULL);
  entry->next = NULL;
  entry->prev = NULL;

  return entry;
}

// Function to create a new LRU cache
LRUCache *createCache(int capacity) {
  LRUCache *cache = (LRUCache *)malloc(sizeof(LRUCache));
  if (!cache)
    return NULL;

  cache->head = NULL;
  cache->tail = NULL;
  cache->capacity = capacity;
  cache->size = 0;

  return cache;
}

// Function to remove a cache entry
void removeEntry(LRUCache *cache, CacheEntry *entry) {
  if (!entry)
    return;

  if (entry->prev) {
    entry->prev->next = entry->next;
  } else {
    cache->head = entry->next;
  }

  if (entry->next) {
    entry->next->prev = entry->prev;
  } else {
    cache->tail = entry->prev;
  }

  free(entry->result);
  free(entry);
  cache->size--;
}

// Function to move an entry to the front (most recently used)
void moveToFront(LRUCache *cache, CacheEntry *entry) {
  if (entry == cache->head)
    return;

  // Remove from current position
  if (entry->prev) {
    entry->prev->next = entry->next;
  } else {
    cache->head = entry->next;
  }

  if (entry->next) {
    entry->next->prev = entry->prev;
  } else {
    cache->tail = entry->prev;
  }

  // Add to front
  entry->next = cache->head;
  entry->prev = NULL;
  if (cache->head) {
    cache->head->prev = entry;
  }
  cache->head = entry;
  entry->last_used = time(NULL);
}

// Function to get a value from the cache
RodResult *get(LRUCache *cache, int rod_length) {
  if (!cache)
    return NULL;

  CacheEntry *current = cache->head;
  while (current) {
    if (current->rod_length == rod_length) {
      moveToFront(cache, current);
      return current->result;
    }
    current = current->next;
  }
  return NULL;
}

// Function to put a key-value pair in the cache
void put(LRUCache *cache, int rod_length, RodResult *result) {
  if (!cache || !result)
    return;

  // Check if key already exists
  CacheEntry *current = cache->head;
  while (current) {
    if (current->rod_length == rod_length) {
      free(current->result);
      current->result = result;
      moveToFront(cache, current);
      return;
    }
    current = current->next;
  }

  // Create new entry
  CacheEntry *newEntry = createEntry(rod_length, result);
  if (!newEntry) {
    free(result);
    return;
  }

  // If cache is full, remove least recently used entry
  if (cache->size >= cache->capacity) {
    removeEntry(cache, cache->tail);
  }

  // Add new entry to front
  newEntry->next = cache->head;
  if (cache->head) {
    cache->head->prev = newEntry;
  }
  cache->head = newEntry;

  // Update tail if this is the first entry
  if (!cache->tail) {
    cache->tail = newEntry;
  }

  cache->size++;
}

// Function to free the cache
void freeCache(LRUCache *cache) {
  if (!cache)
    return;

  CacheEntry *current = cache->head;
  while (current) {
    CacheEntry *next = current->next;
    free(current->result);
    free(current);
    current = next;
  }

  free(cache);
}

// Function to print cache contents
void printCache(LRUCache *cache) {
  if (!cache) {
    printf("Cache is NULL\n");
    return;
  }

  printf("Cache contents (capacity: %d, size: %d):\n", cache->capacity,
         cache->size);
  CacheEntry *current = cache->head;
  while (current) {
    printf("Rod Length: %d\n", current->rod_length);
    printf("Total Value: %d\n", current->result->total_value);
    printf("Remainder: %d\n", current->result->remainder);
    printf("Cuts:\n");
    for (int i = 0; i < current->result->num_cuts; i++) {
      if (current->result->cut_counts[i] > 0) {
        printf("  %d pieces of length %d\n", current->result->cut_counts[i], i);
      }
    }
    printf("Last Used: %ld\n\n", current->last_used);
    current = current->next;
  }
}

// Example usage
int main() {
  // Create a cache with capacity 3
  LRUCache *cache = createCache(3);
  if (!cache) {
    printf("Failed to create cache\n");
    return 1;
  }

  // Example rod cutting results
  int cut_counts1[MAX_CUTS] = {0};
  cut_counts1[2] = 2; // 2 pieces of length 2
  cut_counts1[3] = 1; // 1 piece of length 3
  RodResult *result1 = createRodResult(7, 3, cut_counts1, 15, 0);

  int cut_counts2[MAX_CUTS] = {0};
  cut_counts2[4] = 2; // 2 pieces of length 4
  cut_counts2[2] = 1; // 1 piece of length 2
  RodResult *result2 = createRodResult(10, 3, cut_counts2, 20, 0);

  // Test put operations
  put(cache, 7, result1);
  put(cache, 10, result2);
  printCache(cache);

  // Test get operation
  RodResult *retrieved = get(cache, 7);
  if (retrieved) {
    printf("Retrieved result for rod length 7:\n");
    printf("Total Value: %d\n", retrieved->total_value);
    printf("Remainder: %d\n", retrieved->remainder);
    printf("Cuts:\n");
    for (int i = 0; i < retrieved->num_cuts; i++) {
      if (retrieved->cut_counts[i] > 0) {
        printf("  %d pieces of length %d\n", retrieved->cut_counts[i], i);
      }
    }
  }

  // Free the cache
  freeCache(cache);
  return 0;
}
