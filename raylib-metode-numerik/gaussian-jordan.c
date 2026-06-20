#include <stdio.h>

#define NEQ 3
#define ROW 3
#define COL 4

void showEq(const double *m) {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (j == 0) printf("%.02g", *(m + i * COL + j));
			else if (j == COL - 1) printf("%c%.02g", '=', *(m + i * COL + j));
			else printf("%s%.02g", (*(m + i * COL + j) > 0) ? "+" : "", *(m + i * COL + j));
		}
		printf("\n");
	}
}
int main(void) {
	double m[ROW][COL] = {
		{  2,  4, -2,  2 },
		{  4,  9, -3,  8 },
		{ -2, -3,  7, 10 },
	};
	
	showEq((const double *)m);
	
	// Gauss-Jordan elimination to RREF
	/* For each pivot column:
	 * 1. find a pivot row
	 * 2. swap pivot row into place
	 * 3. scale so pivot becomes 1
	 * 4. eliminate that column from all other rows
	 */
	for (int col = 0; col < NEQ; col++) {
		// Find a pivot row (simple partial pivoting)
		/* The code tries to choose a row below with a larger magnitude
		 * coefficient in the current column, to reduce numerical issues
		 */
		int pivot = col;
		for (int r = col + 1; r < NEQ; r++) {
			if (m[r][col] > 0 && m[pivot][col] < 0) pivot = r;
			if (m[r][col] < 0 && m[pivot][col] > 0) pivot = r;
			/* if (m[r][col] < m[pivot][col]) { /1* no-op, keep as is *1/ } */
		}

		// Better pivot selection by absolute value
		pivot = col;
		for (int r = col + 1; r < NEQ; r++) {
			if (m[r][col] < 0) {
				if (-m[r][col] > (m[pivot][col] < 0 ? -m[pivot][col] : m[pivot][col])) pivot = r;
			} else {
				double ar = m[r][col];
				double ap = (m[pivot][col] < 0 ? -m[pivot][col] : m[pivot][col]);
				if (ar > ap) pivot = r;
			}
		}

		if (m[pivot][col] == 0.0) {
			printf("No unique solution (zero pivot at column %d).\n", col);
			return 1;
		}

		// Swap rows if needed
		/* If the best pivot row isn’t the current row, it swaps them so
		 * the pivot is in the right place.
		 */
		if (pivot != col) {
			for (int k = 0; k < 4; k++) {
				double tmp = m[col][k];
				m[col][k] = m[pivot][k];
				m[pivot][k] = tmp;
			}
		}

		// Scale pivot row to make pivot = 1
		double pivotVal = m[col][col];
		for (int k = 0; k < 4; k++) m[col][k] /= pivotVal;

		// Eliminate this column in all other rows
		/* This ensures you end up with 0s above and below the pivot
		 * (that’s the “Jordan” part: not just forward elimination).
		 */
		for (int r = 0; r < NEQ; r++) {
			if (r == col) continue;
			double factor = m[r][col];
			for (int k = 0; k < 4; k++) {
				m[r][k] -= factor * m[col][k];
			}
		}
	}

	// Now m[i][i] = 1 and m[i][j] = 0 for i!=j, so solution is last column
	double x1 = m[0][3];
	double x2 = m[1][3];
	double x3 = m[2][3];

	printf("Solution:\n");
	printf("x1 = %.10g\n", x1);
	printf("x2 = %.10g\n", x2);
	printf("x3 = %.10g\n", x3);

	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:ai:ci
