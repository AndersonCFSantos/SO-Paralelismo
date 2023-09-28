#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void createMatrix(int number, int rows, int columns) {
    FILE *matrixFile;

    int st;

    if (stat("matrixes", &st) == -1) {
    mkdir("matrixes", 0700);
}  


    char name[100] = "matrix";

    char filename[100];

    sprintf(filename, "%s%d.txt", name, number);

    printf("filename = %s\n", filename);

    matrixFile = fopen(filename, "w");

    if (matrixFile == NULL) {
        printf("The file is not opened. The program will exit now");
        exit(1);
    } else {
        printf("The file is created successfully.\n");
        fprintf(matrixFile, "%d %d\n", rows, columns);
        for(int i=0; i < rows; i++)
        {
            for(int j=0; j<columns; j++)
            {
                int k = rand()%90 + 10;

                if(j< columns - 1)
                {
                    fprintf(matrixFile, "%d ", k);
                }
                else
                {
                    fprintf(matrixFile, "%d\n", k);
                }

                

            }
        }
        printf("Creating matrix %d x %d\n", rows, columns);
        fclose(matrixFile);
    }
}


int main()
{

    

    int n1,m1,n2,m2;

    printf("Insert dimensions of the first matrix: \n");

    scanf("%d %d", &n1, &m1);

    printf("Dimensions of first matrix: %d x %d\n", n1, m1);

    createMatrix(1, n1, m1);

    printf("Insert dimensions of the second matrix: \n");

    scanf("%d %d", &n2, &m2);

    printf("Dimensions of second matrix: %d x %d\n", n2, m2);

    createMatrix(2, n2, m2);


    return 0;


}