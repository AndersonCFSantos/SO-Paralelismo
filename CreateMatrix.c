#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void createMatrix(int number, int rows, int columns) {
    FILE *matrixFile;

    int st;

    if (stat("matrixes", &st) == -1) {
    mkdir("matrixes", 0700);
}  


    char name[100] = "matrixes/matrix";

    //char extension[20] = ".txt";
    char filename[100];

    sprintf(filename, "%s%d.txt", name, number);

    printf("filename = %s\n", filename);

    matrixFile = fopen(filename, "w");

    if (matrixFile == NULL) {
        printf("The file is not opened. The program will exit now");
        exit(1); // You may want to exit with a non-zero code to indicate an error
    } else {
        printf("The file is created successfully.\n");
        fprintf(matrixFile, "%d ", 40);
        fprintf(matrixFile, "%d\n", 43);
        fprintf(matrixFile, "%d ", 63);
        fprintf(matrixFile, "%d\n", 63);
        printf("Creating matrix %d x %d\n", rows, columns);
        fclose(matrixFile); // Close the file when done
    }
}


int main()
{

    

    int n1,m1,n2,m2;

    printf("Insert dimensions of the first matrix: \n");

    scanf("%d %d", &n1, &m1);

    printf("Dimensions of first matrix: %d x %d\n", n1, m1);

    createMatrix(1, n1, m1);

    printf("Insert dimensions of the first matrix: \n");

    scanf("%d %d", &n2, &m2);

    printf("Dimensions of first matrix: %d x %d\n", n2, m2);

    createMatrix(2, n2, m2);


    return 0;


}