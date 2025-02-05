#ifndef MATRIX_H
#define MATRIX_H

/* Matrix */
extern void debug_calcndc(float *m);
extern void print_matrix(char *s, float *mat, int m, int n);
extern void project_matrix(float *m, float fov, float r, float near, float far);
extern void rotate_object_transform(float *mat, float pitch, float yaw, float roll);
extern void rotatex_matrix(float *m, float deg);
extern void rotatey_matrix(float *m, float deg);
extern void rotatez_matrix(float *m, float deg);

#endif
