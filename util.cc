#include <WProgram.h>

#include "util.h"

/**
 * Sort array a[0..n-1] in-place using insertion sort
 */
void sort(int *a, int n)
{
    int k;
    for (k = 1; k < n; ++k) {
        int key = a[k];
        int i = k - 1;
        while ((i >= 0) && (key < a[i])) {
            a[i + 1] = a[i];
            --i;
        }
        a[i + 1] = key;
    }
}

static int buf[ABUFSIZE];

/**
 * Read analog pin n (<= BUFSIZE) times and return the median value
 */
int analogReadMedian(int pin, int n)
{
    int i;
    
    if (n > ABUFSIZE)
        n = ABUFSIZE;
    
    for (i = 0; i < n; i++) {
        buf[i] = analogRead(pin);
    }
    
    sort(buf, n);
    
    return buf[n / 2];
}

