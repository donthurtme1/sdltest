#include <openblas/cblas.h>
#include <math.h>
#include <string.h>

/* Normalises by changing the scalar part */
void
normalise_rotor_scalar(float rotor[4], float mag) {
	register float dot = (rotor[1]*rotor[1]) + (rotor[2]*rotor[2]) + (rotor[3]*rotor[3]);
	rotor[0] = sqrtf((mag * mag) - dot);
}

void
normalise_rotor_bivec(float rotor[4], float len) {
	float r = (rotor[1] * rotor[1]) + (rotor[2] * rotor[2]) + (rotor[3] * rotor[3]);
	float s = sqrtf(r);
	//printf("input:     %f, %f, %f, %f\n", rotor[0], rotor[1], rotor[2], rotor[3]);
	if (s == 0) {
		r = (rotor[1]*rotor[1]) + (rotor[2]*rotor[2]) + (rotor[3]*rotor[3]);
		rotor[0] = 1;
		//printf("geo prod:  %f, %f, %f, %f\n\n", rotor[0], rotor[1], rotor[2], rotor[3]);
		return;
	}
	rotor[1] *= len / s;
	rotor[2] *= len / s;
	rotor[3] *= len / s;
	r = (rotor[1]*rotor[1]) + (rotor[2]*rotor[2]) + (rotor[3]*rotor[3]);
	rotor[0] = sqrtf(1 - r);
	//printf("geo prod:  %f, %f, %f, %f\n\n", rotor[0], rotor[1], rotor[2], rotor[3]);
}

void
normalise_rotor(float rotor[4], float len) {
	register float r = sqrtf((rotor[0]*rotor[0]) + (rotor[1]*rotor[1]) + (rotor[2]*rotor[2]) + (rotor[3]*rotor[3]));
	rotor[0] *= len / r;
	rotor[1] *= len / r;
	rotor[2] *= len / r;
	rotor[3] *= len / r;
}

void
normalise_vec(float vec[3], float len) {
	register float r = sqrtf((vec[0]*vec[0]) + (vec[1]*vec[1]) + (vec[2]*vec[2]));
	printf("%f\n", r);
	vec[0] *= len / r;
	vec[1] *= len / r;
	vec[2] *= len / r;
}

void
geometric_product(float rotor[4], float a[3], float b[3]) {
	/* Normalise vectors */
	cblas_sscal(3, 1 / sqrtf(cblas_sdot(3, b, 1, b, 1)), b, 1);
	cblas_sscal(3, 1 / sqrtf(cblas_sdot(3, a, 1, a, 1)), a, 1);
	/* Calculate rotor */
	rotor[0] = cblas_sdot(3, b, 1, a, 1);
	rotor[1] = (a[0] * b[1]) - (a[1] * b[0]);
	rotor[2] = (a[1] * b[2]) - (a[2] * b[1]);
	rotor[3] = (a[2] * b[0]) - (a[0] * b[2]);
}

void
apply_rotor(float inrotor[4], float vec[3]) {
	/* Calculate inverse rotor
	 * TODO: instead recalculate the rest of the function to be R^vR instead of RvR^ (R^ is R inverse) */
	float rotor[4];
	rotor[0] = inrotor[0];
	rotor[1] = -inrotor[1];
	rotor[2] = -inrotor[2];
	rotor[3] = -inrotor[3];
	/* Assumes the rotor is formed by the multiplication of two unit vectors */
	float S_x = rotor[0]*vec[0] + rotor[1]*vec[1] - rotor[3]*vec[2];
    float S_y = rotor[0]*vec[1] - rotor[1]*vec[0] + rotor[2]*vec[2];
    float S_z = rotor[0]*vec[2] - rotor[2]*vec[1] + rotor[3]*vec[0];
    float S_xyz = rotor[1]*vec[2] + rotor[2]*vec[0] + rotor[3]*vec[1];

    vec[0] = S_x*rotor[0] + S_y*rotor[1] + S_xyz*rotor[2] - S_z*rotor[3];
    vec[1] = S_y*rotor[0] - S_x*rotor[1] + S_z*rotor[2] + S_xyz*rotor[3];
    vec[2] = S_z*rotor[0] + S_xyz*rotor[1] - S_y*rotor[2] + S_x*rotor[3];
}

void
combine_rotor(float S[4], float T[4], float result[4]) {
	float R[4];
    R[0] = S[0]*T[0] - S[1]*T[1] - S[2]*T[2] - S[3]*T[3];
    R[1] = S[0]*T[1] + S[1]*T[0] - S[2]*T[3] + S[3]*T[2];
    R[2] = S[0]*T[2] + S[1]*T[3] + S[2]*T[0] - S[3]*T[1];
    R[3] = S[0]*T[3] - S[1]*T[2] + S[2]*T[1] + S[3]*T[0];
	cblas_scopy(4, R, 1, result, 1);
}

void
rotor_to_matrix(float mat[16], float rotor[4]) {
	memset(mat, 0, 16 * sizeof(float));
	mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
	apply_rotor(rotor, &mat[0]);
	apply_rotor(rotor, &mat[4]);
	apply_rotor(rotor, &mat[8]);
}
