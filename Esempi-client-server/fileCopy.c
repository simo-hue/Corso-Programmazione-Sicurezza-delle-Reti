#include <stdio.h>
#include <stdlib.h> // For exit()
  
int main()
{
    FILE *fptr1, *fptr2;
    char filename[100], c;
  
    printf("Nome del file sorgente:\n");
    scanf("%s", filename);
  
    // Apro il file sorgente in lettura
    fptr1 = fopen(filename, "r");
    if (fptr1 == NULL)
    {
        printf("Errore apertura file %s \n", filename);
        exit(0);
    }
  
    printf("Nome del file destinazione:\n");
    scanf("%s", filename);
  
    // Apro il file destinazione in scrittura
    fptr2 = fopen(filename, "w");
    if (fptr2 == NULL)
    {
        printf("Errore apertura file %s \n", filename);
        exit(0);
    }
  
    // Leggo il primo byte dalla sorgente
    c = fgetc(fptr1);
    while (c != EOF)
    {
        // Scrivo il byte nella destinazione
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }
  
    printf("\nContenuto copiato su %s\n", filename);
  
    fclose(fptr1);
    fclose(fptr2);
    return 0;
}
