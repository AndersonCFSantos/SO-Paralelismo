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

struct Matrix matrix_multiply(struct Matrix mat1, struct Matrix mat2) {
    if (mat1.columns != mat2.rows) {
        printf("Matrix multiplication is not possible. Number of columns in the first matrix must be equal to the number of rows in the second matrix.\n");
        struct Matrix emptyMatrix = {NULL, 0, 0};
        return emptyMatrix;
    }

    struct Matrix result;
    result.rows = mat1.rows;
    result.columns = mat2.columns;

    
    result.data = (int **)malloc(result.rows * sizeof(int *));
    for (int i = 0; i < result.rows; i++) {
        result.data[i] = (int *)malloc(result.columns * sizeof(int));
    }

    for (int i = 0; i < mat1.rows; i++) {
        for (int j = 0; j < mat2.columns; j++) {
            result.data[i][j] = 0;
            for (int k = 0; k < mat1.columns; k++) {
                result.data[i][j] += mat1.data[i][k] * mat2.data[k][j];
            }
        }
    }

    return result;
}

int main()
{

    int rows, columns;
    char filename1 [100], filename2[100];

    printf("Digite o nome do arquivo: \n");

    scanf("%s", filename1);
    scanf("%s", filename2);


    struct Matrix matrix1 = readMatrix(filename1);
    struct Matrix matrix2 = readMatrix(filename2);
    

    struct Matrix multipliedMatrix = matrix_multiply(matrix1,matrix2);

    for (int i = 0; i < matrix1.rows; i++) {
        free(matrix1.data[i]);
    }
    free(matrix1.data);

    for (int i = 0; i < matrix2.rows; i++) {
        free(matrix2.data[i]);
    }
    free(matrix2.data);

    for(int i=0; i<multipliedMatrix.rows;i++)
    {
        for (int j = 0; j < multipliedMatrix.columns; j++)
        {
            printf("%d", multipliedMatrix.data[i][j]);
        }
        printf("\n");
    }


    return 0;
}