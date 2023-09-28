#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

struct Matrix {
    int **data;
    int rows;
    int columns;
    
};

struct ThreadData {
    struct Matrix mat1;
    struct Matrix mat2;
    struct Matrix result;
    int startRow;
    int endRow;
    int number;
    clock_t startTime;
    char * folderName;
};

void *multiply_part(void *arg) {
    struct ThreadData *threadData = (struct ThreadData *)arg;

    char outputFolder[100]; 
    snprintf(outputFolder, sizeof(outputFolder), "./%s", threadData->folderName); 
    mkdir(outputFolder, 0777);

    char outputFilename[100];
    snprintf(outputFilename, sizeof(outputFilename), "./%s/ThreadMultiplication%d.txt", threadData->folderName, threadData->number);
    FILE *outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL) {
        printf("Failed to open the output file.\n");
        exit(1); 
    }

    fprintf(outputFile, "%d %d\n", threadData->result.rows, threadData->result.columns);

    clock_t tStartTime = clock();

    for (int i = threadData->startRow; i < threadData->endRow; i++) {
        for (int j = 0; j < threadData->mat2.columns; j++) {
            threadData->result.data[i][j] = 0;
            for (int k = 0; k < threadData->mat1.columns; k++) {
                threadData->result.data[i][j] += threadData->mat1.data[i][k] * threadData->mat2.data[k][j];
            }
            fprintf(outputFile, "c%d-%d %d\n",i,j,threadData->result.data[i][j]);
        }
    }

    clock_t end_time = clock();

    double elapse_time = ((double)(end_time - tStartTime )) / CLOCKS_PER_SEC;
    fprintf(outputFile,"%lf", elapse_time);

    fclose(outputFile);
    //fclose(timeOutputFile);

    pthread_exit(NULL);
}

struct Matrix matrixThreadMultiply(struct Matrix mat1, struct Matrix mat2, int numThreads, char *folderName) {
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
    snprintf(outputFolder, sizeof(outputFolder), "./%s", folderName); 
    mkdir(outputFolder, 0777);

    
    char outputFilename[100]; 
    snprintf(outputFilename, sizeof(outputFilename), "./%s/tempoTotal.txt", folderName); // Relative path to the file
    FILE *outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL) {
        printf("Failed to open the output file.\n");
        exit(1); 
    }

    //fprintf(outputFile, "Result Matrix Dimension: %d x %d\n", result.rows, result.columns);

    
    clock_t start_time = clock();

    
    pthread_t threads[numThreads];
    struct ThreadData threadData[numThreads];
    int rowsPerThread = result.rows / numThreads;
    int remainingRows = result.rows % numThreads;

    for (int i = 0; i < numThreads; i++) {
        int startRow = i * rowsPerThread;
        //The last thread gets de remaining rows
        int endRow = (i == numThreads - 1) ? (startRow + rowsPerThread + remainingRows) : (startRow + rowsPerThread);
        threadData[i].mat1 = mat1;
        threadData[i].mat2 = mat2;
        threadData[i].result = result;
        threadData[i].startRow = startRow;
        threadData[i].endRow = endRow;
        threadData[i].startTime = start_time;
        threadData[i].folderName = folderName;
        threadData[i].number = i+1;
        pthread_create(&threads[i], NULL, multiply_part, &threadData[i]);
    }


    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;


    // Close the output file
    fclose(outputFile);

    printf("DONE!");

    return result;
}

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

int main() {
    
    char filename1 [100], filename2[100];
    int p;

    printf("Digite o nome da matriz1: \n");
    scanf("%s", filename1);
    printf("Digite o nome da matriz2: \n");
    scanf("%s", filename2);
    printf("Digite o valor de P \n");
    scanf("%d", &p);


    struct Matrix matrix1 = readMatrix(filename1);
    struct Matrix matrix2 = readMatrix(filename2);

    // Specify the number of threads
    int numThreads = matrix1.rows*matrix2.columns/((matrix1.rows*matrix2.columns)/p);

    // Perform matrix multiplication using multiple threads and store dimension and time in a file in a specified folder
    struct Matrix result = matrixThreadMultiply(matrix1, matrix2, numThreads, "ThreadsResult");

    // Display the result matrix
    // if (result.data != NULL) {
    //     for (int i = 0; i < result.rows; i++) {
    //         for (int j = 0; j < result.columns; j++) {
    //             printf("%d ", result.data[i][j]);
    //         }
    //         printf("\n");
    //     }
    // }

    // Free memory for the matrices and result
    for (int i = 0; i < matrix1.rows; i++) {
        free(matrix1.data[i]);
    }
    free(matrix1.data);

    for (int i = 0; i < matrix2.rows; i++) {
        free(matrix2.data[i]);
    }
    free(matrix2.data);

    if (result.data != NULL) {
        for (int i = 0; i < result.rows; i++) {
            free(result.data[i]);
        }
        free(result.data);
    }

    return 0;
}
