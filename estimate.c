#include <stdio.h>
#include <stdlib.h>
double* identityMatrix(int dimension){
    double* identity = calloc(sizeof(double), dimension*dimension);
    for(int i = 0; i < dimension; i++){
        identity[i*dimension+i] = 1;
    }
    return identity;
}
void printMatrix(double* matrix, int rows, int cols){
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++){
            printf("%.0f", matrix[r*cols + c]);
        }
        printf("\n");
    }
}

double* transpose(double* matrix, int rows, int cols){
    double* tMatrix = calloc(sizeof(double), rows*cols);
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++){
            tMatrix[c*rows + r] = matrix[r*cols + c];
        }
    }
    return tMatrix;
}
double* multiplyMatrices(double* matrix1, double* matrix2, int rows1, int cols1, int rows2, int cols2){
    double* resultMatrix = calloc(sizeof(double), rows1*cols2);
    for(int i = 0; i < rows1; i++){
        for(int j = 0; j < cols2; j++){
            for(int k = 0; k < cols1; k++){
                resultMatrix[i*cols2+j] += matrix1[i*cols1 + k] * matrix2[k*cols2 + j];
            }
        }
    }
    return resultMatrix;
}
double* invertMatrix(double* matrix, int dimension){
    double* identity = identityMatrix(dimension);
    for(int p = 0; p < dimension; p++){
        double f = matrix[p*dimension + p];
        for(int i = 0; i < dimension; i++){
            matrix[p*dimension + i] /= f;
            identity[p*dimension + i] /= f; 
        }

        for(int i = p + 1; i < dimension; i++){
            f = matrix[i*dimension + p];
            for(int k = 0; k < dimension; k++){
                matrix[i*dimension + k] -= f * matrix[p*dimension+k];
                identity[i*dimension + k] -= f * identity[p*dimension+k];
            } 

        }
    }
    for(int p = dimension - 1; p >= 0; p--){
        for(int i = p - 1; i >= 0; i--){
            double f = matrix[i*dimension + p];
            for(int k = 0; k < dimension; k++){
                matrix[i*dimension + k] -= f * matrix[p*dimension+k];
                identity[i*dimension + k] -= f * identity[p*dimension+k];
            }
        }
    }
    return identity;
}

/*---------------------TRAINING METHOD---------------------*/

double* trainData(FILE* trainFile, int rows, int cols){
    double *matrix = malloc(rows*cols*sizeof(double));
    double *prices = malloc(rows*sizeof(double));
    // Read matrix from input file
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++){
            if(c==0){
                matrix[r*cols + c] = 1;
            }
            else{
                fscanf(trainFile, "%lf", &matrix[r*cols + c]);
                if(c == cols - 1){
                    fscanf(trainFile, "%lf", &prices[r]);
                }
            }
        }
    }
    //printf("Original Matrix\n");
    //printMatrix(matrix, rows, cols);

    // printf("\nPrices Matrix\n");
    // printMatrix(prices, rows, 1);

    double* tMatrix = transpose(matrix, rows, cols);
    // printf("\nTransposed Matrix\n");
    // printMatrix(tMatrix, cols, rows);

    double* mMatrix = multiplyMatrices(tMatrix, matrix, cols, rows, rows, cols);
    // printf("\nMultiplied Matrix\n");
    // printMatrix(mMatrix, cols, cols);

    double* iMatrix = invertMatrix(mMatrix, cols);
    //printf("\nInverted Matrix\n");
    //printMatrix(iMatrix, cols, cols);

    double* mMatrix2 = multiplyMatrices(iMatrix, tMatrix, cols, cols, cols, rows);
    // printf("\nMultiplied Matrix 2\n");
    // printMatrix(mMatrix2, cols, rows );
    
    double* weights = multiplyMatrices(mMatrix2, prices, cols, rows, rows, 1);
    // printf("\nWeights Matrix\n");
    // printMatrix(weights, rows, 1);
    free(matrix);
    free(tMatrix);
    free(mMatrix);
    free(iMatrix);
    free(mMatrix2);
    free(prices);
    return weights;
}
int main(int argc, char* argv[]){
    FILE* trainFile = fopen(argv[1], "r");
    int rows;
    int cols;
    char train[10];
    fscanf(trainFile, "%s", train);
    fscanf(trainFile, "%d", &cols);
    fscanf(trainFile, "%d", &rows);
    cols += 1;
    
    double* weights = trainData(trainFile, rows, cols);
    // printf("\nWeights Matrix\n");
    // printMatrix(weights, rows, 1); 

    

    // TRAINING COMPLETE, APPLY TO DATA
    FILE* dataFile = fopen(argv[2], "r");
    int rows1;
    int cols1;
    char data[10];
    fscanf(dataFile, "%s", data);
    fscanf(dataFile, "%d", &cols1);
    fscanf(dataFile, "%d", &rows1);
    
    cols1 += 1;

    double *x = malloc(rows*cols*sizeof(double));
    // READ MATRIX FROM INPUT FILE
    for(int r = 0; r < rows1; r++){
        for(int c = 0; c < cols1; c++){
            if(c==0){
                x[r*cols1 + c] = 1;
            }
            else{
                fscanf(dataFile, "%lf", &x[r*cols1 + c]);
            }
        }
    }
    // printf("\nData Matrix\n");
    // printMatrix(x, rows1, cols1);

    double *pricePrediction = multiplyMatrices(x,weights,rows1,cols1,rows,1);
    
    //printf("\nPrice Prediction\n");
    printMatrix(pricePrediction, rows1,1);
    
    fclose(trainFile);
    fclose(dataFile);
    
    free(weights);
    free(pricePrediction);
    free(x);
    return EXIT_SUCCESS;
}
