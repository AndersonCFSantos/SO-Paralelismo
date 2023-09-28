#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct Matrix {
    int **data;
    int rows;
    int columns;
};

// Structure to hold data for each child process
struct ProcessData {
    struct Matrix mat1;
    struct Matrix mat2;
    struct Matrix result;
    int startRow;
    int endRow;
};

// Function to perform matrix multiplication for a part of the matrices
void multiply_part(struct ProcessData *data) {
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = 0; j < data->mat2.columns; j++) {
            data->result.data[i][j] = 0;
            for (int k = 0; k < data->mat1.columns; k++) {
                data->result.data[i][j] += data->mat1.data[i][k] * data->mat2.data[k][j];
            }
        }
    }
}

// Function to perform matrix multiplication using processes
struct Matrix matrix_multiply_processes(struct Matrix mat1, struct Matrix mat2, int numProcesses, char *folderName) {
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
    snprintf(outputFilename, sizeof(outputFilename), "./%s/output.txt", folderName);
    FILE *outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL) {
        printf("Failed to open the output file.\n");
        exit(1);
    }

    fprintf(outputFile, "Result Matrix Dimension: %d x %d\n", result.rows, result.columns);

    clock_t start_time = clock();

    // Calculate rows per process and remaining rows
    int rowsPerProcess = result.rows / numProcesses;
    int remainingRows = result.rows % numProcesses;

    // Create child processes
    for (int i = 0; i < numProcesses; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            struct ProcessData processData;
            processData.mat1 = mat1;
            processData.mat2 = mat2;
            processData.result = result;
            processData.startRow = i * rowsPerProcess;
            processData.endRow = (i == numProcesses - 1) ? (processData.startRow + rowsPerProcess + remainingRows) : (processData.startRow + rowsPerProcess);

            // Perform matrix multiplication for the part assigned to this process
            multiply_part(&processData);

            // Write the results to the output file (each process opens the file separately)
            fprintf(outputFile, "Process %d Result:\n", i);
            for (int j = processData.startRow; j < processData.endRow; j++) {
                for (int k = 0; k < processData.result.columns; k++) {
                    fprintf(outputFile, "%d ", processData.result.data[j][k]);
                }
                fprintf(outputFile, "\n");
            }

            fclose(outputFile);
            exit(0);
        } else if (pid < 0) {
            printf("Failed to create a child process.\n");
            exit(1);
        }
    }

    // Wait for all child processes to complete
    int status;
    for (int i = 0; i < numProcesses; i++) {
        wait(&status);
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    fprintf(outputFile, "Time Taken for Calculation: %lf seconds\n", elapsed_time);

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

    // Specify the number of processes
    int numProcesses = 4;

    // Perform matrix multiplication using multiple processes and store dimension and time in a file in a specified folder
    struct Matrix result = matrix_multiply_processes(mat1, mat2, numProcesses, "output_folder");

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
