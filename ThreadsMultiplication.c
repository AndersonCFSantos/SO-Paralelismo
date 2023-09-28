#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

struct Matrix {
    int **data;
    int rows;
    int columns;
};

// Structure to hold data for each thread
struct ThreadData {
    struct Matrix mat1;
    struct Matrix mat2;
    struct Matrix result;
    int startRow;
    int endRow;
};

// Function to multiply a part of two matrices in parallel
void *multiply_part(void *arg) {
    struct ThreadData *threadData = (struct ThreadData *)arg;

    for (int i = threadData->startRow; i < threadData->endRow; i++) {
        for (int j = 0; j < threadData->mat2.columns; j++) {
            threadData->result.data[i][j] = 0;
            for (int k = 0; k < threadData->mat1.columns; k++) {
                threadData->result.data[i][j] += threadData->mat1.data[i][k] * threadData->mat2.data[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

// Function to multiply two matrices using multiple threads and return the result as a new struct Matrix
// Also, stores the dimension and time in a file while calculating
struct Matrix matrix_multiply_parallel(struct Matrix mat1, struct Matrix mat2, int numThreads, char *folderName) {
    if (mat1.columns != mat2.rows) {
        printf("Matrix multiplication is not possible. Number of columns in the first matrix must be equal to the number of rows in the second matrix.\n");
        struct Matrix emptyMatrix = {NULL, 0, 0};
        return emptyMatrix;
    }

    struct Matrix result;
    result.rows = mat1.rows;
    result.columns = mat2.columns;

    // Allocate memory for the result matrix
    result.data = (int **)malloc(result.rows * sizeof(int *));
    for (int i = 0; i < result.rows; i++) {
        result.data[i] = (int *)malloc(result.columns * sizeof(int));
    }

    // Create the folder if it doesn't exist
    char outputFolder[100]; // Adjust the size as needed
    snprintf(outputFolder, sizeof(outputFolder), "./%s", folderName); // Relative path to the folder
    mkdir(outputFolder, 0777); // Create the folder with full permissions

    // Open the output file for writing in the specified folder
    char outputFilename[100]; // Adjust the size as needed
    snprintf(outputFilename, sizeof(outputFilename), "./%s/output.txt", folderName); // Relative path to the file
    FILE *outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL) {
        printf("Failed to open the output file.\n");
        exit(1); // Terminate the program
    }

    fprintf(outputFile, "Result Matrix Dimension: %d x %d\n", result.rows, result.columns);

    // Measure the time taken for the multiplication
    clock_t start_time = clock();

    // Initialize thread data and create threads
    pthread_t threads[numThreads];
    struct ThreadData threadData[numThreads];
    int rowsPerThread = result.rows / numThreads;
    int remainingRows = result.rows % numThreads;

    for (int i = 0; i < numThreads; i++) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? (startRow + rowsPerThread + remainingRows) : (startRow + rowsPerThread);
        threadData[i].mat1 = mat1;
        threadData[i].mat2 = mat2;
        threadData[i].result = result;
        threadData[i].startRow = startRow;
        threadData[i].endRow = endRow;
        pthread_create(&threads[i], NULL, multiply_part, &threadData[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    fprintf(outputFile, "Time Taken for Calculation: %lf seconds\n", elapsed_time);

    // Close the output file
    fclose(outputFile);

    return result;
}

int main() {
    // Create and initialize two Matrix structs
    struct Matrix mat1;
    mat1.rows = 2;
    mat1.columns = 3;
    mat1.data = (int **)malloc(mat1.rows * sizeof(int *));
    for (int i = 0; i < mat1.rows; i++) {
        mat1.data[i] = (int *)malloc(mat1.columns * sizeof(int));
        for (int j = 0; j < mat1.columns; j++) {
            mat1.data[i][j] = i * mat1.columns + j + 1;
        }
    }

    struct Matrix mat2;
    mat2.rows = 3;
    mat2.columns = 2;
    mat2.data = (int **)malloc(mat2.rows * sizeof(int *));
    for (int i = 0; i < mat2.rows; i++) {
        mat2.data[i] = (int *)malloc(mat2.columns * sizeof(int));
        for (int j = 0; j < mat2.columns; j++) {
            mat2.data[i][j] = i * mat2.columns + j + 1;
        }
    }

    // Specify the number of threads
    int numThreads = 4;

    // Perform matrix multiplication using multiple threads and store dimension and time in a file in a specified folder
    struct Matrix result = matrix_multiply_parallel(mat1, mat2, numThreads, "output_folder");

    // Display the result matrix
    if (result.data != NULL) {
        for (int i = 0; i < result.rows; i++) {
            for (int j = 0; j < result.columns; j++) {
                printf("%d ", result.data[i][j]);
            }
            printf("\n");
        }
    }

    // Free memory for the matrices and result
    for (int i = 0; i < mat1.rows; i++) {
        free(mat1.data[i]);
    }
    free(mat1.data);

    for (int i = 0; i < mat2.rows; i++) {
        free(mat2.data[i]);
    }
    free(mat2.data);

    if (result.data != NULL) {
        for (int i = 0; i < result.rows; i++) {
            free(result.data[i]);
        }
        free(result.data);
    }

    return 0;
}
