#ifndef ROTOR_H
#define ROTOR_H

/* Rotor */
extern void normalise_rotor_scalar(float rotor[4], float mag);
extern void normalise_rotor_bivec(float rotor[4], float mag);
extern void normalise_rotor(float rotor[4], float mag);
extern void normalise_vec(float vec[3], float len);
extern void geometric_product(float rotor[4], float a[3], float b[3]);
extern void apply_rotor(float rotor[4], float vec[3]);
extern void combine_rotor(float S[4], float T[4], float result[4]);
extern void rotor_to_matrix(float mat[16], float rotor[4]);

#endif
