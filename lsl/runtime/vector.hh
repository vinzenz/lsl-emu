#ifndef GUARD_LSL_RUNTIME_VECTOR_HH_INCLUDED
#define GUARD_LSL_RUNTIME_VECTOR_HH_INCLUDED

#include <boost/numeric/ublas/matrix.hpp>

namespace lsl {
namespace runtime {

    struct Vector {
        double x, y, z;
    };

    inline Vector add(Vector l, Vector r) {
        return Vector{
            l.x + r.x,
            l.y + r.y,
            l.z + r.z
        };
    }

    inline Vector add(Vector l, double r) {
        return Vector{
            l.x + r,
            l.y + r,
            l.z + r
        };
    }

    inline Vector sub(Vector l, Vector r) {
        return Vector{
            l.x - r.x,
            l.y - r.y,
            l.z - r.z
        };
    }

    inline Vector sub(Vector l, double r) {
        return Vector{
            l.x - r,
            l.y - r,
            l.z - r
        };
    }

    inline Vector mul(Vector l, Vector r) {
        return Vector{
            l.x * r.x,
            l.y * r.y,
            l.z * r.z
        };
    }

    inline Vector mul(Vector l, double r) {
        return Vector{
            l.x * r,
            l.y * r,
            l.z * r
        };
    }

    inline Vector div(Vector l, Vector r) {
        return Vector{
            l.x / r.x,
            l.y / r.y,
            l.z / r.z
        };
    }

    inline Vector div(Vector l, double r) {
        return Vector{
            l.x / r,
            l.y / r,
            l.z / r
        };
    }
    
    inline Vector cross(Vector l, Vector r) {
        return Vector{
            l.y * r.z - l.z * r.y,
            l.z * r.x - l.x * r.z,
            l.x * r.y - l.y * r.x
        };
    }

    inline double dot(Vector l, Vector r) {
        return (l.x * r.x) + (l.y * r.y) + (l.z * r.z);
    }

    inline double mag(Vector v) {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    inline Vector norm(Vector v) {
        double m = mag(v);
        if(m > 0.) {
            double inv_mag = 1.f / m;
            return mul(v, inv_mag);
        }
        return Vector{0., 0., 0.};
    }

    inline Vector operator * (Vector const & a, Vector const & b) {
        return mul(a, b);
    }
    inline Vector operator / (Vector const & a, Vector const & b) {
        return div(a, b);
    }
    inline Vector operator + (Vector const & a, Vector const & b) {
        return add(a, b);
    }
    inline Vector operator - (Vector const & a, Vector const & b) {
        return sub(a, b);
    }
    inline Vector operator * (Vector const & a, double b) {
        return mul(a, b);
    }
    inline Vector operator / (Vector const & a, double b) {
        return div(a, b);
    }
    inline Vector operator + (Vector const & a, double b) {
        return add(a, b);
    }
    inline Vector operator - (Vector const & a, double b) {
        return sub(a, b);
    }

    inline bool operator == (Vector const & a, Vector const & b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
}}

#endif //GUARD_LSL_RUNTIME_VECTOR_HH_INCLUDED
