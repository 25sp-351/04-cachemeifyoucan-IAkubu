#ifndef RODS_H
#define RODS_H

#define MAX_CUTS 100 // Maximum number of possible cuts

// Dictionary struct to store the length and value of each cut
typedef struct {
  int length;
  int value;
} length_value_pair;

// Function declarations
int rod_cutting_recursive(int rod_length, length_value_pair cuts[],
                          int num_cuts, int *best_cut);
void print_solution(int rod_length, length_value_pair cuts[], int num_cuts);

#endif // RODS_H