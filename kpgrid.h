#ifndef KPGRID_H
#define KPGRID_H
/*		File	:	kpgrid.h */
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <vector>
#include <sstream>

#define PI 3.14159265359
#define MAXLENGTH 1024
#define SOFT "kpoint-grid"
#define EXE "kpgrid"
#define VERS "0.2"
#define STDER stderr


/** \brief Template to convert a \b `std::string` type to a numeric type
  * \tparam numstr - Address of the string to convert
  * \returns a number of type \b `retVal`
  */
template<typename retVal>
retVal 
str2num(const std::string& numstr)
{
    retVal number;
    std::stringstream stream(numstr);
    stream >> number;
    return number;
}


typedef struct {
    double dk;
    double compression;
    int    compCell;
    int    gammaKGrid;
}param_t;

/** \brief Read atomic data from a VASP formatted file
  * \param[in] fname - VASP \a POSCAR or \a CONTCAR
  * \returns 1 if successful and prints to stdout
  */  
double** read_box(char* fname);

/** \brief Determine kgrid divisions based on a density
  * \param[in] dk - kpoint mesh density, in Å^-1
  * \param[out] nx - number of divisions along x
  * \param[out] ny - number of divisions along y
  * \param[out] nz - number of divisions along z
  */ 
void compute_grid(double dk, double** G, int* nx, int* ny, int* nz);

/** \brief Compute reciprocal lattice vectors
  *  updates the global vectors b1,b2,b3
  */
double** recp_vectors(double** H);

/** \brief Compress lattice vectors by 50% if needed
  *  updates the global vectors a1,a2,a3
  */
void compress_vectors(double scale, double** H);

/** \brief Compute norm of an 3 length vector
  * \param[in] a - primary vector
  * \returns c = (|a.a|)
  */
double vmag(double* v);

/** \brief Compute dot product of two 3 vectors
  * \param[in] a - primary vector
  * \param[in] b - secondary vector
  * \returns c = (a . b)
  */
double vdot(double* a, double* b);

double angle(double* a, double* b);

/** \brief Compute cross product of two 3D vectors
  * \param[in] a - primary lattice vector
  * \param[in] b - secondary lattice vector
  * \returns c = (a x b)
  */
double* vcross(double* a,double* b);

#endif