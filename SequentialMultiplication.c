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

struct Matrix matrix_multiply(struct Matrix mat1, struct Matrix mat2, const char * foldername) {
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

    char outputFolder[100];
    snprintf(outputFolder,sizeof(outputFolder),"%s", foldername);
    mkdir(outputFolder, 0777);

    char outputFilename[100];
    snprintf(outputFilename, sizeof(outputFilename),"./%s/SequentialMultiplication.txt", foldername);

    FILE * outputFile = fopen(outputFilename, "w");

    fprintf(outputFile, "%d %d\n", result.rows, result.columns);

    if(outputFile == NULL)
    {
        printf("Failed to open output file. Exiting\n");
        exit(1);
    }

    clock_t start_time = clock();

    for (int i = 0; i < mat1.rows; i++) {
        for (int j = 0; j < mat2.columns; j++) {
            result.data[i][j] = 0;
            for (int k = 0; k < mat1.columns; k++) {
                result.data[i][j] += mat1.data[i][k] * mat2.data[k][j];
            }
            fprintf(outputFile,"c%d-%d = %d\n", i, j, result.data[i][j]);
        }
    }

    clock_t end_time = clock();

    double elapse_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    fprintf(outputFile,"%lf sec", elapse_time);

    printf("DONE!\n");

    return result;
}

int main()
{

    int rows, columns;
    char filename1 [100], filename2[100];

    printf("Digite o nome dos dois arquivos: \n");

    scanf("%s", filename1);
    scanf("%s", filename2);


    struct Matrix matrix1 = readMatrix(filename1);
    struct Matrix matrix2 = readMatrix(filename2);
    

    struct Matrix multipliedMatrix = matrix_multiply(matrix1,matrix2, "SequentialResult");

    for (int i = 0; i < matrix1.rows; i++) {
        free(matrix1.data[i]);
    }
    free(matrix1.data);

    for (int i = 0; i < matrix2.rows; i++) {
        free(matrix2.data[i]);
    }
    free(matrix2.data);

    // for(int i=0; i<multipliedMatrix.rows;i++)
    // {
    //     for (int j = 0; j < multipliedMatrix.columns; j++)
    //     {
    //         printf("%d ", multipliedMatrix.data[i][j]);
    //     }
    //     printf("\n");
    // }

    for (int i = 0; i < multipliedMatrix.rows; i++) {
        free(multipliedMatrix.data[i]);
    }
    free(multipliedMatrix.data);


    return 0;
}