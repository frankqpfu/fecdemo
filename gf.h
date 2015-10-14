/**
 * GF space is different from normal.
 */

int gf_add(int a, int b);
int gf_sub(int a, int b);
int gf_multiply(int a, int b);
int gf_div(int a, int b, int w);
int gf_init_log_table(int w);
    
extern int gf_w;
extern int gf_logi_table[];
extern int gf_log_table[];

template<>
inline int Matrix<int>::addFunc(int a, int b)
{
	return gf_add(a, b);
}

template<>
inline int Matrix<int>::subFunc(int a, int b)
{
	return gf_sub(a, b);
}

template<>
inline int Matrix<int>::multiFunc(int a, int b)
{
	int result = 0;
	result = gf_multiply(a, b);
	if ((a == 1 && b == 5) || (a == 5 && b== 1)) {
		printf("1 * 5 is:%d\n", result);
	}
	return result;
}

template<>
inline int Matrix<int>::divFunc(int a, int b)
{
	return gf_div(a, b, gf_w);
}

template <typename T>
inline Matrix<T>* matrix_getPesuedoLineMatrix_Gf(int rows, int cols)
{
    Matrix<T> *p = new Matrix<T>(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            (*p)[i][j] = 1;
        }
    }

    for (int i = 1; i < rows; i++) {
        for (int j = 1; j < cols; j++) {
            T base = gf_logi_table[j];
            (*p)[i][j] = p->multiFunc((*p)[i-1][j], base);
        }
    }
    return p;
}

void print_logi_table();
void print_log_table();

