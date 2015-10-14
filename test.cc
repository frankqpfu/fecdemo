#include "matrix.h"
#include "gf.h"

void test_getFloatRightReverseMatrix()
{
	printf("[TestCase]xxxxxxxxxUse **Float** Type to check the matrix base reverse function xxxx\n");
#define ROWS 3
#define COLS 4
#define DATA_COLS 4

	Matrix<float> *a = matrix_getEMatrix<float>(COLS, COLS);
	Matrix<float> *b = matrix_getPesuedoLineMatrix_Basic<float>(ROWS, COLS);

	a->AppendDown(b);
	a->RemoveRow(1);
	a->RemoveRow(2);
	a->RemoveRow(3);

	printf("+++++++++++++This is a A matrix++++++++++++++++\n");
	a->Print();
	
    Matrix<float> * aclone = a->Clone();
    printf("+++++++++++++This is a copy of A matrix++++++++++\n");
    aclone->Print();

	Matrix<float> * arev = matrix_reverse(a);
    if (arev != NULL){
		printf("Success to get the A_minus_1 matrix\n");
		arev->Print();
		
		Matrix<float> * result = matrix_multiply(arev, aclone);
        if (result != NULL) {
            printf("Check the A_minus_1 * A = E \n");
            result->Print();
        }
	} else {
		printf("Failed to get the reverse matrix.\n");
	}

	printf("[TestCase]xxxxxxxxxxxxxxxxxxTest case overxxxxxxxxxxxxxxxxx\n");
	return ;
}

void test_cantGetReverse()
{
	float matrix[4][4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix[i][j] = 1;
		}
	}

	Matrix<float> b((float **)matrix, 4, 4);
	Matrix<float> * arev = matrix_reverse(&b);
}

int checkGfReverseMatrix()
{
	printf("[TestName]: ~~~~~~~~~~~~Test the GF space reverse matrix is work.~~~~~~~~~~~~~~~~~\n");
	int rows = 2;
	int cols = 4;

	Matrix<int> *b = matrix_getPesuedoLineMatrix_Gf<int>(rows, cols);
	printf("Produce a fante mende matrix:\n");
    b->Print();

	Matrix<int> *a = matrix_getEMatrix<int>(4, 4);
	a->AppendDown(b);
	a->RemoveRow(1);
	a->RemoveRow(1);
	printf("After remove the matrix index 1,2:\n");
	a->Print();

	Matrix<int> *cloneA = a->Clone();
	Matrix<int> *reverse = matrix_reverse(a);
    if (reverse  != NULL){
		printf("Get the reverse matrix is success, A_minus_1:\n");
		reverse->Print();
		
		printf("The reserved A matrix is:\n");
		cloneA->Print();
		
		Matrix<int> *outBack = matrix_multiply(reverse, cloneA);
		printf("finally result A_minus_1 * A = ? expect E:\n");
		outBack->Print();
	} else {
		printf("Failed to get the reverse matrix.\n");
	}

	return 1;
}

int calculateFec()
{
	printf("[TestName]: ~~~~~~~~~~~~Test the GF space Fec can work~~~~~~~~~~~~~~~~~\n");
	int rows = 3;
	int cols = 4;

	Matrix<int> *b = matrix_getPesuedoLineMatrix_Gf<int>(rows, cols);
	printf("Produce a fante mende matrix:\n");

	int data[4][2] = {
			{1, 2},
			{2, 3},
			{3, 4},
			{4, 5}
	};

	printf("Simulate Raw data before Fec:\n");
	Matrix<int> c((int**)data, 4, 2);
	c.Print();

	Matrix<int> *outFec = matrix_multiply(b, &c);
	printf("Prouce Fec matrix data:\n");
	outFec->Print();

	c.AppendDown(outFec);
	c.RemoveRow(1);
	c.RemoveRow(1);
	c.RemoveRow(1);
	printf("Matrix simulate Rx Raw data m_rx, but lost index 1, 2, 3:\n");
	c.Print();

	Matrix<int> *a = matrix_getEMatrix<int>(4, 4);
	a->AppendDown(b);
	a->RemoveRow(1);
	a->RemoveRow(1);
	a->RemoveRow(1);
	printf("Prepare to recover the raw data, construct the check matrix lost 1,2,4:\n");
	a->Print();

	Matrix<int> *reverse = matrix_reverse(a);
    if (reverse != NULL) {
		printf("Found the A_minus_1 for the check matrix:\n");
		reverse->Print();
		
		Matrix<int> *outBack = matrix_multiply(reverse, &c);
		printf("After A_minus_1 * m_rx, get the recover result matrix:\n");
		outBack->Print();
	} else {
		printf("Failed to get the reverse matrix.\n");
	}

	return 1;
}

int main(int argc, char * argv[])
{
	gf_w = 3;
	gf_init_log_table(gf_w);

    test_getFloatRightReverseMatrix();
	calculateFec();
	//test_getRightReverseMatrix();
}
