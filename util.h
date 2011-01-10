/*
 * Utilities
 */

// Sort array a[0..n-1] in-place
void sort(int *a, int n);

// Buffer
#define BUFSIZE 20

// Read analog pin n (<= 20) times and return the median value
int analogReadMedian(int pin, int n = 10);

