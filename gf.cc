#include "matrix.h"
#include "gf.h"

int gf_w;
int gf_log_table[512];
int gf_logi_table[512 * 3];

int gf_multiply(int a, int b)
{
	if (a == 0 || b == 0) 
	{
		return 0;
	}

	int exp = gf_log_table[a] + gf_log_table[b];
	return gf_logi_table[exp];
}

int gf_div(int a, int b, int w)
{
	if (b == 0)
	{
		printf("0 is wrong!\n");
		return -1;
	}
	if (a == 0)
	{
		return 0;
	}
	int nw = 1 << w;
	int exp = gf_log_table[a] - gf_log_table[b] + (nw - 1);
	return gf_logi_table[exp];
}

int gf_add(int a, int b)
{
	return a ^ b;
}

int gf_sub(int a, int b)
{
	return a ^ b;
}


void print_logi_table()
{
	printf("-------------logi_table------------\n");
	int nw = 1 << gf_w;
	for (int i = 0; i < 2*nw; i++){
		printf("%-3d -> %-3d\n", i, gf_logi_table[i]);
	}
}

void print_log_table()
{
	printf("-------------log_table------------\n");
	int nw = 1 << gf_w;
	for (int i = 0; i < 2*nw; i++){
		printf("%-3d -> %-3d\n", i, gf_log_table[i]);
	}
}

int gf_init_log_table(int w)
{
#define MAX_GF_W (8)
	assert (w <= MAX_GF_W);
	int prim_poly[MAX_GF_W] = {0, 0, 0xB, 0x13, 0x29, 0x43, 0x83, 0x11D};
	int max_length = w << 1;

	memset(gf_log_table, -1, sizeof(gf_log_table));
	memset(gf_logi_table, -1, sizeof(gf_logi_table));

	int nw = 1 << w; //->max_length
	int b = 1;
	for (int i = 0; i < nw-1; i++) {
		gf_logi_table[i] = b;
		gf_log_table[b] = i;
		b = b << 1;
		if (b & nw)	{
			b = b ^ prim_poly[w-1];
		}
	}

	//The logi table is cycle and to avoid the add result will over-flow the natural scope, 
	//like 7 + 7 = 14 > 7, so we copy them to the tail,  
	//while the last two position is empty, so we copy it another copy.
	// 0 can look up the logitable, but can not look up the logtable.
	memcpy(gf_logi_table + nw - 1, gf_logi_table, (nw - 1) * sizeof(int));
	memcpy(gf_logi_table + 2 * (nw - 1), gf_logi_table, (nw - 1) * sizeof(int));
	
	print_log_table();
	print_logi_table();
	printf("\n");
}
