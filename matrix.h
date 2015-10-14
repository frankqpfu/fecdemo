/**
 * Description:
 *      Test the Matrix multiply or reverse.
 * Author:
 *      @frankqpfu
 * Date:
 *      2015-04-15
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#define DEBUG
/**
 * TODO:Replace it into boost scoped ptr.
 */
template <typename T> class SimpleResourcePtr
{
public:
	explicit SimpleResourcePtr(T ** ptr){_ptr = ptr;}
	~SimpleResourcePtr()
	{
		if (*_ptr != NULL) {
			delete *_ptr;
			*_ptr = NULL;
		}
	}
private:
	T ** _ptr;
};

template <typename T> class Matrix
{
#define Default_Max_Row_Size (100)
#define Default_Max_Col_Size (100)
    public:

	    //Sequence buffer, 1 dimension to 2 dimension, matrix.
        Matrix(T * buffer, int rows, int cols)
        {
            _max_rows = Default_Max_Row_Size;
            _max_cols = Default_Max_Col_Size;

            _data = new T*[_max_rows];
            for (int i = 0; i < rows; i++) {
            	_data[i] = new T[_max_cols];
            	memcpy(_data[i], buffer + i*cols, cols * sizeof(T));
            }
            _rows = rows;
            _cols = cols;
            _alloc = 1;
        }

        //Sequence Buffer, 2 dimension to matrix.
        Matrix(T ** buffer, int rows, int cols)
        {
            _max_rows = Default_Max_Row_Size;
            _max_cols = Default_Max_Col_Size;

            _data = new T*[_max_rows];
            for (int i = 0; i < rows; i++) {
            	_data[i] = new T[_max_cols];
            	memcpy(_data[i], (T*)buffer + i * cols, cols * sizeof(T));
            }
            _rows = rows;
            _cols = cols;
            _alloc = 1;
        }

        //for you want to new a matrix, only know the rows and cols.
        //choice1, buffer should be allocate from outside. we are simple.
        //choice2, buffer we allocate, but the function **addRow** must be copy, this is good too,
        //		   because we don't know the buffer is in our charge or allocated from outside.
        //non-sequece buffer, for speical purpose. + AddRow() to make a matrix. row can be 0.
        Matrix(int rows, int cols)
        {
            _max_rows = Default_Max_Row_Size;
            _max_cols = Default_Max_Col_Size;

            _data = new T*[_max_rows];
            for (int i = 0; i < rows; i++) {
                _data[i] = new T[_max_cols];
                memset(_data[i], 0, sizeof(T) * cols);
            }
            _rows = rows;
            _cols = cols;
            _alloc = 1;
        }

        ~Matrix()
        {
           //If we make the buffer.
           if (_alloc == 1) {
        	   for (int i = 0; i < _rows; i++) {
        		   delete[] _data[i];
        		   _data[i] = NULL;
        	   }
           }
           delete [] _data;
           _data = NULL;
        }

        int Cols() {return _cols;}
        int Rows() {return _rows;}

        T* GetRow(int rowIndex)
        {
        	return _data[rowIndex];
        }

        void AppendRow(T * buffer)
        {
            assert(_rows <= _max_rows);

            _data[_rows] = new T[_max_cols];
            memcpy(_data[_rows], buffer, sizeof(T) * _cols);
            _rows++;
        }

        void RemoveRow(int rowIndex)
        {
        	assert (rowIndex >= 0 && rowIndex <_rows);
        	if (rowIndex != (_rows - 1)) {
        		memmove(&_data[rowIndex], &_data[rowIndex+1], (_rows-(rowIndex+1)) * sizeof(T*));
        	}
        	_rows--;
        }

        void SwapRow(int rowIndex1, int rowIndex2)
        {
        	assert (rowIndex1 >= 0 && rowIndex1 < _rows && rowIndex2 >=0 && rowIndex2 < _rows);
        	T* swap = _data[rowIndex1];
        	_data[rowIndex1] = _data[rowIndex2];
        	_data[rowIndex2] = swap;
        }

        void AddRow(int rowBaseIndex, int rowEraseIndex, int col)
        {
        	T baseValue = _data[rowBaseIndex][col];
        	T eraseValue = _data[rowEraseIndex][col];

        	for (int j = 0; j < _cols; j++) {
        		_data[rowBaseIndex][j] = this->multiFunc( _data[rowBaseIndex][j], eraseValue);
        		_data[rowEraseIndex][j] = this->multiFunc(_data[rowEraseIndex][j], baseValue);
        		_data[rowEraseIndex][j] = this->subFunc(  _data[rowEraseIndex][j], _data[rowBaseIndex][j]);
        	}
        }

        void DivRow(int rowIndex, int colIndex)
        {
         	T value = _data[rowIndex][colIndex];

         	for (int j = 0; j < _cols; j++) {
         		_data[rowIndex][j] = this->divFunc(_data[rowIndex][j], value);
         	}
        }

        /**
         * This two function is used for if we do not want to use submatrix way to get the reverse matrix.
         * way 1. AddRowWithMatrixE, DivRowWithMatrixE
         * way 2. AppendMatrixRight, AddRow, DivRow, SubMatrix.
         * I have not decide which way is better.
         */
        void AddRowWithMatrixE(int rowBaseIndex, int rowEraseIndex, int col, Matrix<T> * e)
        {
        	T baseValue = _data[rowBaseIndex][col];
        	T eraseValue = _data[rowEraseIndex][col];

        	for (int j = 0; j < _cols; j++) {
        		_data[rowBaseIndex][j] *= eraseValue;
        		_data[rowEraseIndex][j] *= baseValue;
        		_data[rowEraseIndex][j] -= _data[rowBaseIndex][j];

        		(*e)[rowBaseIndex][j] *= eraseValue;
        		(*e)[rowEraseIndex][j] *= baseValue;
        		(*e)[rowEraseIndex][j] -= (*e)[rowBaseIndex][j];
        	}
        }

        void DivRowWithMatrixE(int rowIndex, int colIndex, Matrix<T> * e)
        {
         	T value = _data[rowIndex][colIndex];

         	for (int j = 0; j < _cols; j++) {
         		_data[rowIndex][j] /= value;
         		(*e)[rowIndex][j] /= value;
         	}
        }


        T* operator[](int rowIndex)
        {
        	assert(rowIndex <= _rows);
        	return _data[rowIndex];
        }

        void Print()
        {
        	printf("_rows:%d x _cols:%d\n", _rows, _cols);
        	printf("[----------------------------\n");
        	for (int i = 0; i < _rows; i++) {
        		for (int j = 0; j < _cols; j++) {
        			printf("%-3d ", _data[i][j]);
        		}
        		printf("\n");
        	}
        	printf("----------------------------]\n");
        	printf("\n");
        }

        void  AppendDown(Matrix<T> * pDownMatrix)
        {
        	assert(this->Cols() == pDownMatrix->Cols());

        	for (int i = 0; i < pDownMatrix->Rows(); i++) {
        		this->AppendRow(pDownMatrix->GetRow(i));
        	}
        	return;
        }

        void AppendRight(Matrix<T> *pRightMatrix)
        {
        	assert(this->Rows() == pRightMatrix->Rows());
        	assert(this->Cols() + pRightMatrix->Cols() <= _max_cols);

        	for (int i = 0; i < pRightMatrix->Rows(); i++) {
        		memcpy(_data[i] + _cols, pRightMatrix->GetRow(i), pRightMatrix->Cols() * sizeof(T));
        	}
        	_cols += pRightMatrix->Cols();
        	return;
        }

        Matrix<T> * GetSubMatrix(int leftx, int lefty, int rightx, int righty)
        {
        	assert(0 <= leftx && leftx <= rightx && rightx < this->Cols());
        	assert(0 <= lefty && lefty <= righty && righty < this->Rows());

        	int cols = rightx - leftx + 1;
        	int rows = righty - lefty + 1;

            Matrix * subMatrix = new Matrix<T>(0, cols);
        	for (int i = 0; i < rows; i++) {
        		subMatrix->AppendRow(_data[lefty+i] + leftx);
        	}
            return subMatrix;
        }

        Matrix * Clone()
        {
        	Matrix *pCloneMatrix = new Matrix<T>(0, _cols);
        	for (int i = 0; i < this->Rows(); i++)
        	{
        		(pCloneMatrix)->AppendRow(this->GetRow(i));
        	}
        	return pCloneMatrix;
        }

        T addFunc (T a, T b)
        {
        	return a + b;
        }

        T subFunc (T a, T b)
        {
        	return a - b;
        }

        T multiFunc(T a, T b)
        {
        	return a * b;
        }

        T divFunc(T a, T b)
        {
            assert(b != 0);
        	return a / b;
        }
    private:
        int _rows; //number of "--"
        int _cols; //column, number of "|"
        T ** _data;
        int _max_rows;
        int _max_cols;
        int _alloc;
};

/**
 * Specialize it for type **float**
 */
template<>
inline void Matrix<float>::Print()
{
	printf("_rows:%d x _cols:%d\n", _rows, _cols);
	printf("[----------------------------\n");
	for (int i = 0; i < _rows; i++) {
		for (int j = 0; j < _cols; j++) {
			printf("%6.2f ", _data[i][j]);
		}
		printf("\n");
	}
	printf("----------------------------]\n");
	printf("\n");
}

/**
 * Specialize it for type **GF space + - / **
 */

template <typename T>
inline Matrix<T> * matrix_getEMatrix(int rows, int cols)
{
	Matrix<T> *e = new Matrix<T>(rows, cols);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (i == j) {
				(*e)[i][j] = 1;
			} else {
				(*e)[i][j] = 0;
			}
		}
	}
	return e;
}

/**
 * Here this fantemede is a1...an is the root of the sequence.
 * Here is special, different from int. 
 */
template <typename T>
inline Matrix<T>* matrix_getPesuedoLineMatrix_Basic(int rows, int cols)
{
    Matrix<T> *p = new Matrix<T>(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            (*p)[i][j] = pow(j+1, i);
        }
    }
    return p;
}

template <typename T>
inline Matrix<T> * matrix_multiply(Matrix<T> * a, Matrix<T> * b)
{
	assert(a->Cols() == b->Rows());

	Matrix<T> *c = new Matrix<T>(a->Rows(), b->Cols());

    for (int b_col = 0; b_col < b->Cols(); b_col++) {
    	for (int a_row = 0; a_row < a->Rows(); a_row++) {
    		for (int a_col = 0; a_col < a->Cols(); a_col++) {
    			//(**c)[a_row][b_col] += (*a)[a_row][a_col] * (*b)[a_col][b_col];
    			(*c)[a_row][b_col] = a->addFunc((*c)[a_row][b_col], a->multiFunc((*a)[a_row][a_col], (*b)[a_col][b_col]));
    		}
    	}
    }
    return c;
}

template <typename T>
inline Matrix<T> * matrix_reverse(Matrix<T> *a)
{
	assert(a->Rows() == a->Cols());

	Matrix<T> *E = matrix_getEMatrix<T>(a->Rows(), a->Cols());
	a->AppendRight(E);

	for (int c = 0; c < a->Rows(); c++) {
		for (int r = 0; r < a->Rows(); r++) {
			if ((*a)[r][c] != 0) {
		        //First step, find one line the prev elements are all ZERO.
				int right_base_col = 1;
				for (int prev_col = c-1; prev_col >= 0; prev_col--) {
					if ((*a)[r][prev_col] != 0) {
						right_base_col = 0;
					}
				}

				if (right_base_col == 1) {
					//Erase the other lines based on the base line.
                    for (int r1 = 0; r1 < a->Rows(); r1++) {
						if (((*a)[r1][c] != 0) && r1 != r) {
							a->AddRow(r, r1, c);
							//printf("debug, after add base%d, erase%d, %d\n", r, r1, c);
							//a->Print();
						}
					}

                    //Exchange the line to make the non-zero element is on the corner-line.
					if (r != c) {
						a->SwapRow(r, c);
						//printf("debug, after swap %d, %d\n", r, c);
						//a->Print();
					}
				}
			}
		}
	}

	//print_logi_table();
    //Change the corner element to one.
	for (int r = 0; r < a->Rows(); r++) { 
		int allZero = 1;
		for (int c = 0; c < a->Rows(); c++) {
			if (r == c && (*a)[r][c] != 0) {
				a->DivRow(r, c);
				//printf("debug, after div %d, %d\n", r, c);
				//a->Print();
				allZero = 0;
			}
		}
		if (allZero == 1) {
			printf("This matrix can not found a reverse.\n");
			return NULL;
		}
	}


	Matrix<T> * reverse = a->GetSubMatrix(a->Rows(), 0, a->Cols()-1, a->Rows()-1);
    if (reverse != NULL)
	    (reverse)->Print();
	return reverse;
}



