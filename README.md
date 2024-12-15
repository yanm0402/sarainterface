# sarainterface
Overview
This program is designed to process sparse matrices, offering two key functionalities:

Compressed Sparse Row (CSR) Conversion: Converts sparse matrices into the efficient CSR format, which significantly reduces memory usage by storing only non-zero elements and their metadata.
Matrix Segmentation: For matrices exceeding hardware or memory limits, the program segments the input into smaller, square submatrices for independent processing.
Features
Reads input matrices in Matrix Market (MTX) format.
Converts manageable sparse matrices to CSR format and saves the results to a file.
Automatically segments large matrices into smaller blocks for easier handling, saving each segment as a separate file.
Outputs CSR or segmented data for further analysis or computation.
Requirements
C Compiler: Compatible with gcc or similar.
Input matrices in MTX format.
How to Use
Compile the Program:

gcc -o matrix_processor main.c
Run the Program with an Input Matrix File:

./matrix_processor <.mtx file>

The program will:

Process the input file delaunay_n20.mtx by default (you can replace this file with your own MTX file).
Automatically segment large matrices or directly compress smaller ones.
Output:

CSR Output: For manageable matrices, the CSR components are saved in compressoutput.txt.
Segmented Output: For large matrices, segmented blocks are saved in files named segment_<startRow>_<startCol>.mtx.

Program Flow
The program opens the specified MTX file and reads the matrix dimensions and non-zero elements.
If the matrix size exceeds the MAX_SEGMENT_SIZE (1,000,000 by default):
The matrix is segmented into square submatrices.
Each segment's data is saved in a separate file.
If the matrix size is manageable:
The matrix is processed into CSR format.
The CSR components (Values, Column Indices, Row Pointers) are saved to an output file.
Customization
Adjust the MAX_SEGMENT_SIZE constant in the code to control the segmentation threshold.
Replace delaunay_n20.mtx with your own dataset in the appropriate MTX format.
Known Limitations
Handles square segmentation for simplicity; other shapes may require additional logic.
Input files must strictly follow the MTX forma
