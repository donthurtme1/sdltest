#include <math.h>
#include <stdio.h>

#define PI 3.131592f

void debug_calcndc(float *m);
void print_matrix(char *s, float *mat, int m, int n);

void
debug_calcndc(float *m) {
	float ndc_coords[] = {
		m[0] / m[3], m[1] / m[3], m[2] / m[3],
		m[4] / m[7], m[5] / m[7], m[6] / m[7],
		m[8] / m[11], m[9] / m[11], m[10] / m[11],
		m[12] / m[15], m[13] / m[15], m[14] / m[15],
	};
	print_matrix("debug_ndc_coords", ndc_coords, 3, 4);
}

void
print_matrix(char *s, float *mat, int m, int n) {
	int i, j;
	printf("%s:\n", s);
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			printf("%f, ", mat[j * m + i]);
		}
		printf("\n");
	}
	printf("\n");
}

/* 'fov' is in degrees */
void
project_matrix(float *m, float fov, float r, float near, float far) {
	/* Calculate cot(vertical fov / 2) */
	register float t = 1 / tanf(fov * PI / 360.0f);
	m[0] = t / r;
	m[5] = t;

	/* Calculate view depth */
	m[10] = 2 / (near + far);
	m[11] = 1.0f;
	m[14] = -1.0f;

	/* Set the rest to 0 */
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[12] = m[13] = m[15] = 0.0f;
}

void
rotatex_matrix(float *mat, float deg) {
	const float rad = deg * PI / 180.0f;
	mat[5] = cosf(rad);
	mat[6] = -1 * sinf(rad);
	mat[9] = sinf(rad);
	mat[10] = cosf(rad);
}

void
rotatey_matrix(float *mat, float deg) {
	const float rad = deg * PI / 180.0f;
	mat[0] = cosf(rad);
	mat[2] = -1 * sinf(rad);
	mat[8] = sinf(rad);
	mat[10] = cosf(rad);
}

void
rotatez_matrix(float *mat, float deg) {
	const float rad = deg * PI / 180.0f;
	mat[0] = cosf(rad);
	mat[1] = sinf(rad);
	mat[4] = -1 * sinf(rad);
	mat[5] = cosf(rad);
}
