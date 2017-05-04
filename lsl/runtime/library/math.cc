// Copyright 2014 Vinzenz Feenstra
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <lsl/runtime/library/functions.hh>
#include <cmath>
#include <random>

namespace lsl {
namespace runtime {
namespace lib {

Float llSin(ScriptRef, Float a)            { return sin(a); }
Float llCos(ScriptRef, Float a)            { return cos(a); }
Float llTan(ScriptRef, Float a)            { return tan(a); }
Float llAtan2(ScriptRef, Float a, Float b) {return atan2(a, b); }
Float llSqrt(ScriptRef, Float a)           { return sqrt(a); }
Float llPow(ScriptRef, Float a, Float b)   { return pow(a, b); }
Integer llAbs(ScriptRef, Integer a)        { return abs(a); }
Float llFabs(ScriptRef, Float a)           { return fabs(a); }
Integer llFloor(ScriptRef, Float a)        { return static_cast<Integer>(floor(a)); }
Float   llLog(ScriptRef, Float a)          { return log(a); }
Integer llCeil(ScriptRef, Float a)         { return static_cast<Integer>(ceil(a)); }
Integer llRound(ScriptRef, Float a)        { return static_cast<Integer>(round(a)); }
Float llAcos(ScriptRef, Float a)           { return acos(a); }
Float llAsin(ScriptRef, Float a)           { return asin(a); }

Float llVecMag(ScriptRef, Vector a)        { return mag(a); }
Vector llVecNorm(ScriptRef, Vector a)      { return norm(a); }
Float llVecDist(ScriptRef, Vector a, Vector b) { return dist(a, b); }

Rotation llAxes2Rot(ScriptRef, Vector fwd, Vector left, Vector up) {
    Float s = 0.;
    Float tr = fwd.x + left.y + up.z + 1.0;
    if (tr >= 1.0) {
        s = 0.5 / sqrt(tr);
        return {
            ((left.z - up.y) * s),
            ((up.x - fwd.z) * s),
            ((fwd.y - left.x) * s),
            (0.25 / s)
        };
    }
    else {
        Float max = (left.y > up.z) ? left.y : up.z;
        if (max < fwd.x) {
            s = sqrt(fwd.x - (left.y + up.z) + 1.0);
            Float x = s * 0.5;
            s = 0.5 / s;
            return {
                x,
                ((fwd.y + left.x) * s),
                ((up.x + fwd.z) * s),
                ((left.z - up.y) * s)
            };
        }
        else if (max == left.y) {
            s = sqrt(left.y - (up.z + fwd.x) + 1.0);
            Float y = s * 0.5;
            s = 0.5 / s;
            return {
                ((fwd.y + left.x) * s),
                y,
                ((left.z + up.y) * s),
                ((up.x - fwd.z) * s)
            };
        }
        else {
            s = sqrt(up.z - (fwd.x + left.y) + 1.0);
            Float z = s * 0.5f;
            s = 0.5f / s;
            return {
                ((up.x + fwd.z) * s),
                ((left.z + up.y) * s),
                z,
                ((fwd.y - left.x) * s)
            };
        }
    }
}

Vector llRot2Fwd(ScriptRef, Rotation r) {
    return Vector{1., 0., 0.} * r;
}

Vector llRot2Left(ScriptRef, Rotation r) {
    return Vector{0., 1., 0.} * r;
}


Vector llRot2Up(ScriptRef, Rotation r) {
    return Vector{0., 0., 1.} * r;
}

Rotation llRotBetween(ScriptRef, Vector start, Vector end) {
    start = norm(start);
    end = norm(end);

    auto dotp = dot(start, end);
    auto crossp = cross(start, end);
    auto magp = mag(start) * mag(end);
    auto angle = acos(dotp / magp);
    auto axis = norm(crossp);
    auto s = sin(angle / 2.);
    auto x = axis.x * s;
    auto y = axis.y * s;
    auto z = axis.z * s;
    auto w = cos(angle / 2.);

    if(isnan(x) || isnan(y) || isnan(z) || isnan(w)) {
        return identity(Rotation());
    }
    return {
        x, y, z, w
    };
}

Vector llRot2Euler(ScriptRef, Rotation a)  { return to_euler(a); }
Rotation llEuler2Rot(ScriptRef, Vector a)  { return from_euler(a); }

Integer llModPow(ScriptRef, Integer base, Integer exponent, Integer modulus){
    base %= modulus;
    Integer result = 1;
    while(exponent > 0) {
        if((exponent % 2) == 0) {
            return (result * base) % modulus;
        }
        exponent >>= 1;
        base = (base * base) % modulus;
    }
    return result;
}

}}}
