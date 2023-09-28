#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void readMatrix(int number, int x, int y, int ** matrix)
{

    FILE *file = fopen("matrix.txt", "r");
    
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1; 
    }
    
    fscanf(file, "%d %d", &x, &y);
    
    if (x <= 0 || y <= 0) {
        printf("Invalid dimensions.\n");
        fclose(file);
        return 1;
    }
    
    int **matrix = (int **)malloc(x * sizeof(int *));
    for (int i = 0; i < x; i++) {
        matrix[i] = (int *)malloc(y * sizeof(int));
    }
    
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                printf("Error reading matrix element at (%d, %d).\n", i, j);
                fclose(file);
                return 1;
            }
        }
    }
    
    for (int i = 0; i < x; i++) {
        free(matrix[i]);
    }
    free(matrix);

    fclose(file);

}

int matrix_multiply(int n1, int m1, int n2, int m2, int ** mat1, int ** mat2, int ** result) {
    if (m1 != n2) {
        printf("Matrix multiplication is not possible. Number of columns in the first matrix must be equal to the number of rows in the second matrix.\n");
        return;
    }

    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < m2; j++) {
            result[i][j] = 0;
            for (int k = 0; k < m1; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

int main()
{

}