#pragma once

#include <exception>
#include <stdexcept>
#include <math.h>
#include <new>
#include <algorithm>
#include <sstream>
#include <limits>
#define EPSILON 0.00001
#define PI 3.1415926535898

template <typename T>
T** create2DArray(unsigned nrows, unsigned ncols, const T& val = T())
{
   if (nrows == 0)
        throw std::invalid_argument("number of rows is 0");
   if (ncols == 0)
        throw std::invalid_argument("number of columns is 0");
   T** ptr = nullptr;
   T* pool = nullptr;
   try
   {
       ptr = new T*[nrows];  // allocate pointers (can throw here)
       pool = new T[nrows*ncols]{val};  // allocate pool (can throw here)
       // now point the row pointers to the appropriate positions in
       // the memory pool
       for (unsigned i = 0; i < nrows; ++i, pool += ncols )
           ptr[i] = pool;
       return ptr;
   }
   catch (std::bad_alloc& ex)
   {
       delete [] ptr; // either this is nullptr or it was allocated
       throw ex;  // memory allocation error
   }
}

template <typename T>
void delete2DArray(T** arr)
{
   delete [] arr[0];  // remove the pool
   delete [] arr;     // remove the pointers
}

inline bool doubleEqual(double a, double b) {
    // handle special case of +/- infinity
    if ( (a == std::numeric_limits<double>::infinity() &&
          b == std::numeric_limits<double>::infinity()) ||
         (a == -std::numeric_limits<double>::infinity() &&
          b == -std::numeric_limits<double>::infinity()) )
    {
              return true;
    }
    return ( abs(a - b) < EPSILON );
}

inline std::string getFileExtension(const std::string &filename)
{
    size_t i = filename.rfind('.', filename.length());
    if (i != std::string::npos) {
        return(filename.substr(i+1, filename.length() - i));
    }
    return ("");
}

inline double clamp_unit_interval(double v) {
    return std::max(0.0, std::min(v, 1.0));
}

inline double floatmod(double v, double d) {
    return v - d * floor(v / d);
}