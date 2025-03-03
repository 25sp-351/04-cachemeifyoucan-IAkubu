#include "rods.h"
#include <stdio.h>
#include <stdlib.h>

// Recursive method to find the optimal solution
int rod_cutting_recursive(int rod_length, length_value_pair cuts[],
                          int num_cuts, int *best_cut) {
  if (rod_length == 0) {
    return 0;
  }

  int max_value = 0;
  int best_cut_choice = -1;

  // Recursively try all possible cuts
  for (int i = 0; i < num_cuts; i++) {
    if (cuts[i].length <= rod_length) {
      int sub_value = rod_cutting_recursive(rod_length - cuts[i].length, cuts,
                                            num_cuts, best_cut);
      int possible_value = cuts[i].value + sub_value;

      if (possible_value > max_value) {
        max_value = possible_value;
        best_cut_choice = cuts[i].length;
      }
    }
  }

  *best_cut = best_cut_choice;
  return max_value;
}

// Helper to print the optimal solution
void print_solution(int rod_length, length_value_pair cuts[], int num_cuts) {
  int remaining_length = rod_length;
  int total_value = 0;
  int cut_counts[MAX_CUTS] = {0};

  // Find the best cuts using recursion
  while (remaining_length > 0) {
    int best_cut = -1;
    int max_value =
        rod_cutting_recursive(remaining_length, cuts, num_cuts, &best_cut);

    if (best_cut == -1) {
      break; // No valid cut found
    }

    // Find the index of the best cut in our cuts array
    int cut_index = -1;
    for (int i = 0; i < num_cuts; i++) {
      if (cuts[i].length == best_cut) {
        cut_index = i;
        break;
      }
    }

    if (cut_index != -1) {
      cut_counts[cut_index]++;
      remaining_length -= best_cut;
      total_value += cuts[cut_index].value;
    }
  }

  // Print the cut list in the required format
  for (int i = 0; i < num_cuts; i++) {
    if (cut_counts[i] > 0) {
      printf("%d @ %d = %d\n", cut_counts[i], cuts[i].length,
             cut_counts[i] * cuts[i].value);
    }
  }

  printf("Remainder: %d\n", remaining_length);
  printf("Value: %d\n", total_value);
}
