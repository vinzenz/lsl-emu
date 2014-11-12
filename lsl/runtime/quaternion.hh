#ifndef GUARD_LSL_RUNTIME_QUATERNION_HH_INCLUDED
#define GUARD_LSL_RUNTIME_QUATERNION_HH_INCLUDED

#include <boost/math/constants/constants.hpp>
#include <lsl/runtime/vector.hh>

namespace lsl {
namespace runtime {
    struct Quaternion {
        double x, y, z, s;
    };

    inline Quaternion add(Quaternion l, Quaternion r) {
        return Quaternion{
            l.x + r.x,
            l.y + r.y,
            l.z + r.z,
            l.s + l.s
        };
    }

    inline Quaternion sub(Quaternion l, Quaternion r) {
        return Quaternion{
            l.x - r.x,
            l.y - r.y,
            l.z - r.z,
            l.s - r.s
        };
    }

    inline Quaternion mul(Quaternion l, Quaternion r) {
        return Quaternion{
            r.s * l.x + r.x * l.s + r.y * l.z - r.z * l.y,
            r.s * l.y + r.y * l.s + r.z * l.x - r.x * l.z,
            r.s * l.z + r.z * l.s + r.x * l.y - r.y * l.x,
            r.s * l.s - r.x * l.x - r.y * l.y - r.z * l.z
        };
    }

    inline Vector mul(Vector l, Quaternion r) {
        Quaternion vq{l.x, l.y, l.z, 0.};
        Quaternion nq{-r.x, -r.y, -r.z, r.s};
        Quaternion result = mul(nq, mul(vq, r));
        return{result.x, result.y, result.z};
    }

    inline Quaternion mul(Quaternion l, double r) {
        return {l.x * r, l.y * r, l.z * r, l.s * r};
    }

    inline Quaternion div(Quaternion l, Quaternion r) {
        r.s = -r.s;
        return mul(l, r);
    }

    inline Vector div(Vector l, Quaternion r) {
        r.s = -r.s;
        return mul(l, r);
    }

    inline Vector operator * (Vector const & a, Quaternion const & b) {
        return mul(a, b);
    }
    inline Quaternion operator * (Quaternion const & a, Quaternion const & b) {
        return mul(a, b);
    }
    inline Quaternion operator * (Quaternion const & a, double b) {
        return mul(a, b);
    }
    inline Quaternion operator / (Quaternion const & a, Quaternion const & b) {
        return div(a, b);
    }
    inline Vector operator / (Vector const & a, Quaternion const & b) {
        return div(a, b);
    }
    inline Quaternion operator + (Quaternion const & a, Quaternion const & b) {
        return add(a, b);
    }
    inline Quaternion operator - (Quaternion const & a, Quaternion const & b) {
        return sub(a, b);
    }

    inline bool operator==(Quaternion l, Quaternion r) {
        return l.x == r.x
            && l.y == r.y
            && l.z == r.z
            && l.s == r.s;
    }

    inline bool operator!=(Quaternion l, Quaternion r) {
        return !(l == r);
    }

    inline Quaternion from_euler(Vector euler) {
        double c1 = cos(euler.x * 0.5);
        double c2 = cos(euler.y * 0.5);
        double c3 = cos(euler.z * 0.5);

        double s1 = sin(euler.x * 0.5);
        double s2 = sin(euler.y * 0.5);
        double s3 = sin(euler.z * 0.5);

        Quaternion b{
            s1 * c2 * c3 + c1 * s2 * s3,
            c1 * s2 * c3 - s1 * c2 * s3,
            s1 * s2 * c3 + c1 * c2 * s3,
            c1 * c2 * c3 - s1 * s2 * s3,
        };

        auto a = Quaternion{0., 0., s3, c3}
                 * Quaternion{0., s2, 0., c2}
                 * Quaternion{s1, 0., 0., s1};
        auto c = a + b;
        auto d = a - b;
        static double const err = 0.00001;
        if(   (fabs(c.x) > err && fabs(d.x) > err)
           || (fabs(c.y) > err && fabs(d.y) > err)
           || (fabs(c.z) > err && fabs(d.z) > err)
           || (fabs(c.s) > err && fabs(d.s) > err)) {
            return b;
        }
        return a;
    }

    static double const FP_MAG_THRESHOLD        = 0.0000001;
    static double const GIMBAL_THRESHOLD        = 0.000436;
    static double const ONE_PART_IN_A_MILLION   = 0.000001;

    inline Vector to_euler(Quaternion r) {
        using namespace boost::math::double_constants;

        double sx = 2. * (r.x * r.s - r.y * r.z);
        double sy = 2. * (r.y * r.s + r.x * r.z);

        double ys = r.s * r.s - r.y * r.y;
        double xz = r.x * r.x - r.z * r.z;

        double cx = ys - xz;
        double cy = sqrt(sx * sx + cx * cx);

        if(cy > GIMBAL_THRESHOLD) {
            return Vector{
                atan2(sx, cx),
                atan2(sy, cy),
                atan2(2 * (r.z * r.s - r.x * r.y), ys + xz)
            };
        }

        if(sy > 0) {
            return Vector{
                0.,
                half_pi,
                2. * atan2(r.z + r.x, r.s + r.y)
            };
        }
        return Vector{
            0.,
            half_pi * -1.,
            atan2(r.z - r.x, r.s - r.y)
        };
    }

    inline double mag(Quaternion q) {
        return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.s * q.s);
    }

    inline Quaternion normalize(Quaternion q) {

        double m = mag(q);
        if(m < FP_MAG_THRESHOLD) {
            return Quaternion{0., 0., 0., 1.};
        }
        if(fabs(1. - m) > ONE_PART_IN_A_MILLION) {
            double oomag = 1. / m;
            return Quaternion{
                q.x * oomag,
                q.y * oomag,
                q.z * oomag,
                q.s * oomag
            };
        }
        return q;
    }
}
}

#endif //GUARD_LSL_RUNTIME_QUATERNION_HH_INCLUDED
