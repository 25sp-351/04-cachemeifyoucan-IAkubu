#include "rods.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char line[256];
  int rod_length;
  length_value_pair cuts[MAX_CUTS];
  int num_cuts = 0;

  // Read rod length
  printf("Enter the rod length:\n");
  if (!fgets(line, sizeof(line), stdin) ||
      sscanf(line, "%d", &rod_length) != 1) {
    fprintf(stderr, "Failed to read rod length\n");
    return 1;
  }

  // Read cut lengths and values
  printf("Enter cut lengths and values as 'length, value' (Ctrl+D to end):\n");
  while (num_cuts < MAX_CUTS) {
    int length, value;
    if (scanf("%d, %d", &length, &value) == 2) {
      cuts[num_cuts].length = length;
      cuts[num_cuts].value = value;
      num_cuts++;
    } else {
      if (feof(stdin)) {
        break; // Exit on EOF
      }
      // Clear invalid input
      scanf("%*[^\n]");
      scanf("%*c");
    }
  }

  if (num_cuts == 0) {
    fprintf(stderr, "No valid cuts provided\n");
    return 1;
  }

  // Print the solution
  print_solution(rod_length, cuts, num_cuts);

  return 0;
}