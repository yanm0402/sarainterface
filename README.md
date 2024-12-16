# sarainterface

This program is designed to process sparse matrices by Yanming Liang, offering two key functionalities:

1) Compressed Sparse Row (CSR) Conversion: Converts sparse matrices into the efficient CSR format, which significantly reduces memory usage by storing only non-zero elements and their metadata.
2) Matrix Segmentation: For matrices exceeding hardware or memory limits, the program segments the input into smaller, square submatrices for independent processing.

Features:

1) Reads input matrices in Matrix Market (MTX) format.
2) Converts manageable sparse matrices to CSR format and saves the results to a file.
3) Automatically segments large matrices into smaller blocks for easier handling, saving each segment as a separate file.
4) Outputs CSR or segmented data for further analysis or computation.

Requirements：
1) C Compiler: Compatible with gcc or similar.
2) Input matrices in MTX format.

How to Use:

Compile the Program:

$$$$$ "gcc -o matrix_processor main.c"

Run the Program with an input MTX File by replacing delaunay_n20.mtx in the main.c line 260 (fopen) with your own dataset:

$$$$$ "./matrix_processor"

The program will:

Process the input file delaunay_n20.mtx by default (you can replace this file with your own MTX file).
Automatically segment large matrices or directly compress smaller ones.

Output:

1) CSR Output: For manageable matrices, the CSR components are saved in compressoutput.txt.
2) Segmented Output: For large matrices, segmented blocks are saved in files named segment_<startRow>_<startCol>.mtx.

Program Flow:

The program opens the specified MTX file and reads the matrix dimensions and non-zero elements. If the matrix size exceeds the MAX_SEGMENT_SIZE (1,000,000 by default), the matrix is segmented into square submatrices. Each segment's data is saved in a separate file. If the matrix size is manageable, the matrix is processed into CSR format.
The CSR components (Values, Column Indices, Row Pointers) are saved to an output file.

Customization:

1) Adjust the MAX_SEGMENT_SIZE constant in the code to control the segmentation threshold.
2) Replace delaunay_n20.mtx in the main.c line 260 (fopen) with your own dataset in the appropriate MTX format.

