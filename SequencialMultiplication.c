#include <stdio.h>
#include <stdlib.h>
#include <time.h>


struct Matrix {
    int ** data;
    int rows;
    int columns;
};

struct Matrix readMatrix(char *filename) {
    int rows, columns;

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failed to open the file.\n");
        exit(1);
    }

    if (fscanf(file, "%d %d", &rows, &columns) != 2) {
        printf("Error reading matrix dimensions.\n");
        fclose(file);
        exit(1);
    }

    if (rows <= 0 || columns <= 0) {
        printf("Invalid dimensions.\n");
        fclose(file);
        exit(1);
    }

    // Allocate memory for the matrix
    struct Matrix matrix;
    matrix.rows = rows;
    matrix.columns = columns;
    matrix.data = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix.data[i] = (int *)malloc(columns * sizeof(int));
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (fscanf(file, "%d", &matrix.data[i][j]) != 1) {
                printf("Error reading matrix element at (%d, %d).\n", i, j);
                fclose(file);
                exit(1);
            }
        }
    }

    fclose(file);

    return matrix;
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

    int rows, columns;
    char filename [100];

    printf("Digite o nome do arquivo: \n");

    scanf("%s", filename);

    printf("nome: %s", filename);

    struct Matrix matrix = readMatrix(filename);

    for(int i=0; i<matrix.rows;i++)
    {
        for (int j = 0; j < matrix.columns; j++)
        {
            printf("%d", matrix.data[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < matrix.rows; i++) {
        free(matrix.data[i]);
    }
    free(matrix.data);

    return 0;
}