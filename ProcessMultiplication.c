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

struct ProcessData {
    struct Matrix matrix1;
    struct Matrix matrix2;
    struct Matrix result;
    int startRow;
    int endRow;
    int number;
    clock_t startTime;
    char * folderName;
};

// Function to perform matrix multiplication for a part of the matrices
void multiply_part(struct ProcessData *processData) {

    char outputFilename[100];
    snprintf(outputFilename, sizeof(outputFilename), "./%s/ProcessMultiplication%d.txt", processData->folderName, processData->number);
    FILE *outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL) {
        printf("Failed to open the output file.\n");
        exit(1);
    }
    fprintf(outputFile,"%d %d\n", processData->result.rows, processData->result.columns);

    for (int i = processData->startRow; i < processData->endRow; i++) {
        for (int j = 0; j < processData->matrix2.columns; j++) {
            processData->result.data[i][j] = 0;
            for (int k = 0; k < processData->matrix1.columns; k++) {
                processData->result.data[i][j] += processData->matrix1.data[i][k] * processData->matrix2.data[k][j];
            }
            fprintf(outputFile, "c%d-%d %d\n",i,j,processData->result.data[i][j]);
        }
    }
    clock_t end_time = clock();
    double elapse_time = ((double)(end_time - processData->startTime))/CLOCKS_PER_SEC;
    fprintf(outputFile,"%lf", elapse_time);

    fclose(outputFile);
}

// Function to perform matrix multiplication using processes
struct Matrix matrix_multiply_processes(struct Matrix matrix1, struct Matrix matrix2, int numProcesses, char *folderName) {
    if (matrix1.columns != matrix2.rows) {
        printf("Matrix multiplication is not possible. Number of columns in the first matrix must be equal to the number of rows in the second matrix.\n");
        struct Matrix emptyMatrix = {NULL, 0, 0};
        return emptyMatrix;
    }

    struct Matrix result;
    result.rows = matrix1.rows;
    result.columns = matrix2.columns;

    result.data = (int **)malloc(result.rows * sizeof(int *));
    for (int i = 0; i < result.rows; i++) {
        result.data[i] = (int *)malloc(result.columns * sizeof(int));
    }

    char outputFolder[100];
    snprintf(outputFolder, sizeof(outputFolder), "./%s", folderName);
    mkdir(outputFolder, 0777);

    // char outputFilename[100];
    // snprintf(outputFilename, sizeof(outputFilename), "./%s/output.txt", folderName);
    // FILE *outputFile = fopen(outputFilename, "w");
    // if (outputFile == NULL) {
    //     printf("Failed to open the output file.\n");
    //     exit(1);
    // }

    // fprintf(outputFile, "Result Matrix Dimension: %d x %d\n", result.rows, result.columns);

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
            processData.matrix1 = matrix1;
            processData.matrix2 = matrix2;
            processData.result = result;
            processData.startRow = i * rowsPerProcess;
            processData.endRow = (i == numProcesses - 1) ? (processData.startRow + rowsPerProcess + remainingRows) : (processData.startRow + rowsPerProcess);
            processData.startTime = start_time;
            processData.number = i+1;
            processData.folderName = folderName;

            // Perform matrix multiplication for the part assigned to this process
            multiply_part(&processData);

            // // Write the results to the output file (each process opens the file separately)
            // fprintf(outputFile, "Process %d Result:\n", i);
            // for (int j = processData.startRow; j < processData.endRow; j++) {
            //     for (int k = 0; k < processData.result.columns; k++) {
            //         fprintf(outputFile, "%d ", processData.result.data[j][k]);
            //     }
            //     fprintf(outputFile, "\n");
            // }

            // fclose(outputFile);
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

    // clock_t end_time = clock();
    // double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    // fprintf(outputFile, "Time Taken for Calculation: %lf seconds\n", elapsed_time);

    // fclose(outputFile);

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

    int numProcesses = matrix1.rows*matrix2.columns/((matrix1.rows*matrix2.columns)/p);

    // Perform matrix multiplication using multiple processes and store dimension and time in a file in a specified folder
    struct Matrix result = matrix_multiply_processes(matrix1, matrix2, numProcesses, "ProcessesResults");

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
