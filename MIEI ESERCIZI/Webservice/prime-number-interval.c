#include <stdio.h>
#include <stdlib.h> 

int main(int argc, char **argv)
{
    int a, b, i, j, flag;

    if(argc < 3)    {
        printf("USAGE: %s min max\n", argv[0]);
        return -1;
    }

    // lower bound 
    a = atoi(argv[1]);
    
    // upper bound
    b = atoi(argv[2]);
 
    // Traverse each number in the interval
    // with the help of for loop
    for (i = a; i <= b; i++) {
        // Skip 0 and 1 as they are
        // neither prime nor composite
        if (i == 1 || i == 0)
            continue;
 
        // flag variable to tell
        // if i is prime or not
        flag = 1;
 
        for (j = 2; j <= i / 2; ++j) {
            if (i % j == 0) {
                flag = 0;
                break;
            }
        }
 
        // flag = 1 means i is prime
        // and flag = 0 means i is not prime
        if (flag == 1)
            printf("%d\n", i);
    }
 
    return 0;
}
