#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define MAX_SEGMENT_SIZE 1000000 // Defines the maximum size of each square segment (1,000,000)

typedef struct {
    int startRow;
    int startCol;
    int segmentSize;
    int nonzeros;
} SegmentInfo;

// Function to initialize a matrix
void initializeMatrix(int rows, int cols, int **matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0; // Initialize all elements to 0
        }
    }
}

// Function to print the matrix
void printMatrix(int rows, int cols, int **matrix) {
    FILE *file;
    file = fopen("output.txt", "w");
    fprintf(file, "%d %d\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Function to write the CSR format to an output file
void writeCSRToFile(const char *outputFile, int *values, int *colIndex, int *rowPtr, int nonzeros, int rows) {
    FILE *file = fopen(outputFile, "w");
    if (file == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Values: ");
    for (int i = 0; i < nonzeros; i++) {
        fprintf(file, "%d ", values[i]);
    }
    fprintf(file, "\nColumn Indices: ");
    for (int i = 0; i < nonzeros; i++) {
        fprintf(file, "%d ", colIndex[i]);
    }
    fprintf(file, "\nRow Pointers: ");
    for (int i = 0; i <= rows; i++) {
        fprintf(file, "%d ", rowPtr[i]);
    }
    fprintf(file, "\n");

    fclose(file);
}

// Function to compress a matrix using CSR
int compress() {
    char outputFileName[] = "compressoutput.txt";
    int rows, cols;

    FILE *inputFile = fopen("output.txt", "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // Read matrix dimensions from the first line of the input file
    fscanf(inputFile, "%d %d", &rows, &cols);

    // Allocate memory for the matrix
    int **sparseMatrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        sparseMatrix[i] = (int *)malloc(cols * sizeof(int));
    }

    // Read the sparse matrix from the file
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(inputFile, "%d", &sparseMatrix[i][j]);
        }
    }

    fclose(inputFile);

    // Arrays for CSR format - dynamically allocate memory
    int *values = (int *)malloc(rows * cols * sizeof(int));
    int *colIndex = (int *)malloc(rows * cols * sizeof(int));
    int *rowPtr = (int *)malloc((rows + 1) * sizeof(int));
    int nonzeros = 0;

    // Build the CSR format
    rowPtr[0] = 0; // Start of the first row
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (sparseMatrix[i][j] != 0) {
                values[nonzeros] = sparseMatrix[i][j];
                colIndex[nonzeros] = j; // Column index of the non-zero value
                nonzeros++;
            }
        }
        rowPtr[i + 1] = nonzeros; // End of the current row
    }

    // Write the CSR format to the output file
    writeCSRToFile(outputFileName, values, colIndex, rowPtr, nonzeros, rows);

    printf("CSR format has been written to %s\n", outputFileName);

    // Free dynamically allocated memory
    for (int i = 0; i < rows; i++) {
        free(sparseMatrix[i]);
    }
    free(sparseMatrix);
    free(values);
    free(colIndex);
    free(rowPtr);

    return 0;
}

// First pass: Calculate non-zeros for each segment
void calculateNonzerosPerSegment(FILE *file, SegmentInfo *segments, int numSegments, int nonzeros) {
    int row, col;
    char line[MAX_LINE];

    rewind(file);
    // Skip comment lines again
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '%') {
            break;
        }
    }

    // Read each non-zero value and update the corresponding segment count
    for (int i = 0; i < nonzeros; i++) {
        if (fscanf(file, "%d %d", &row, &col) != 2) {
            perror("Error reading matrix entry during counting");
            exit(EXIT_FAILURE);
        }

        // Find which segment this belongs to
        for (int j = 0; j < numSegments; j++) {
            if (row >= segments[j].startRow && row < segments[j].startRow + segments[j].segmentSize &&
                col >= segments[j].startCol && col < segments[j].startCol + segments[j].segmentSize) {
                segments[j].nonzeros++;
                break;
            }
        }
    }
}

// Second pass: Write non-zero entries to their respective segment files
void writeSegments(FILE *file, SegmentInfo *segments, int numSegments, int nonzeros) {
    int row, col;
    char line[MAX_LINE];
    FILE **segmentFiles = (FILE **)malloc(numSegments * sizeof(FILE *));
    char segmentFileName[256];

    // Open files for each segment
    for (int i = 0; i < numSegments; i++) {
        snprintf(segmentFileName, sizeof(segmentFileName), "segment_%d_%d.mtx", segments[i].startRow, segments[i].startCol);
        segmentFiles[i] = fopen(segmentFileName, "w");
        if (segmentFiles[i] == NULL) {
            perror("Error opening segment file");
            exit(EXIT_FAILURE);
        }

        // Write segment header
        fprintf(segmentFiles[i], "%d %d %d\n", segments[i].segmentSize, segments[i].segmentSize, segments[i].nonzeros);
    }

    rewind(file);
    // Skip comment lines again
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '%') {
            break;
        }
    }

    // Write each non-zero value to the corresponding segment file
    for (int i = 0; i < nonzeros; i++) {
        if (fscanf(file, "%d %d", &row, &col) != 2) {
            perror("Error reading matrix entry during writing");
            exit(EXIT_FAILURE);
        }

        // Find which segment this belongs to
        for (int j = 0; j < numSegments; j++) {
            if (row >= segments[j].startRow && row < segments[j].startRow + segments[j].segmentSize &&
                col >= segments[j].startCol && col < segments[j].startCol + segments[j].segmentSize) {
                // Adjust row and column for the segment
                int adjustedRow = row - segments[j].startRow + 1;
                int adjustedCol = col - segments[j].startCol + 1;
                fprintf(segmentFiles[j], "%d %d\n", adjustedRow, adjustedCol);
                break;
            }
        }
    }

    // Close all segment files
    for (int i = 0; i < numSegments; i++) {
        fclose(segmentFiles[i]);
    }

    free(segmentFiles);
}

// New function for square segmentation (ensuring proper control of segment size)
void squareSegmentMatrix(FILE *file, int rows, int cols, int nonzeros) {
    int segmentSize = (rows < cols) ? rows : cols;
    if (segmentSize > MAX_SEGMENT_SIZE) {
        segmentSize = MAX_SEGMENT_SIZE;
    }

    // Calculate the number of segments needed
    int numSegments = 0;
    for (int rowStart = 1; rowStart <= rows; rowStart += segmentSize) {
        for (int colStart = 1; colStart <= cols; colStart += segmentSize) {
            numSegments++;
        }
    }

    // Allocate memory for storing segment information
    SegmentInfo *segments = (SegmentInfo *)malloc(numSegments * sizeof(SegmentInfo));
    if (segments == NULL) {
        perror("Error allocating memory for segments");
        exit(EXIT_FAILURE);
    }

    // Populate segment information
    int segmentIndex = 0;
    for (int rowStart = 1; rowStart <= rows; rowStart += segmentSize) {
        for (int colStart = 1; colStart <= cols; colStart += segmentSize) {
            segments[segmentIndex].startRow = rowStart;
            segments[segmentIndex].startCol = colStart;
            segments[segmentIndex].segmentSize = (rowStart + segmentSize - 1 <= rows && colStart + segmentSize - 1 <= cols) ? segmentSize : MAX_SEGMENT_SIZE;
            segments[segmentIndex].nonzeros = 0; // Initialize non-zero count to zero
            segmentIndex++;
        }
    }

    // First pass: Calculate non-zero counts for each segment
    calculateNonzerosPerSegment(file, segments, numSegments, nonzeros);

    // Second pass: Write non-zero entries to their respective segment files
    writeSegments(file, segments, numSegments, nonzeros);

    // Free allocated memory for segment information
    free(segments);
}

int main() {
    FILE *file = fopen("delaunay_n20.mtx", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Variables to store matrix dimensions and non-zero entries
    int rows, cols, nonzeros;

    // Skip comment lines (starting with %)
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '%') {
            break; // Stop at the first non-comment line
        }
    }

    // Read matrix dimensions and number of non-zero entries
    sscanf(line, "%d %d %d", &rows, &cols, &nonzeros);

    // Check if the matrix is too large for regular processing
    if (rows > MAX_SEGMENT_SIZE || cols > MAX_SEGMENT_SIZE) {
        printf("Matrix is too large. Segmenting into smaller square blocks...\n");
        squareSegmentMatrix(file, rows, cols, nonzeros);
        fclose(file);
        return 0;
    }

    // Proceed with regular matrix operations if size is manageable
    int **sparseMatrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        sparseMatrix[i] = (int *)malloc(cols * sizeof(int));
    }

    initializeMatrix(rows, cols, sparseMatrix);

    // Read non-zero entries from the file and populate the matrix
    for (int i = 0; i < nonzeros; i++) {
        int row, col;
        if (fscanf(file, "%d %d", &row, &col) != 2) {
            perror("Error reading matrix entries");
            fclose(file);
            return EXIT_FAILURE;
        }
        row--; // Convert 1-based to 0-based
        col--; // Convert 1-based to 0-based
        sparseMatrix[row][col] = 1; // Mark the non-zero position in the matrix
    }

    fclose(file);

    // Print the resulting sparse matrix to an output file
    printMatrix(rows, cols, sparseMatrix);

    // Compress the matrix using CSR format
    compress();

    // Free allocated memory for the matrix
    for (int i = 0; i < rows; i++) {
        free(sparseMatrix[i]);
    }
    free(sparseMatrix);

    return 0;
}
