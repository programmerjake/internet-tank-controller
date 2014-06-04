#include "util.h"
#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <stdexcept>
#include <random>
#include <ostream>

using namespace std;

struct VectorF;

struct VectorI //integer vector
{
    int x, y, z;
    constexpr VectorI(int x, int y, int z)
        : x(x), y(y), z(z)
    {
    }

private:
    VectorI(float x, float y, float z) = delete;
public:

    constexpr VectorI(int v = 0)
        : x(v), y(v), z(v)
    {
    }

    constexpr const VectorI operator -() const //switching sign
    {
        return VectorI(-x, -y, -z);
    }

    constexpr const VectorI & operator +() const
    {
        return *this;
    }

    constexpr const VectorI operator +(const VectorI & r) const //addition
    {
        return VectorI(x + r.x, y + r.y, z + r.z);
    }

    constexpr const VectorI operator -(const VectorI & r) const //subtraction
    {
        return VectorI(x - r.x, y - r.y, z - r.z);
    }

    constexpr const VectorI operator *(const VectorI & r) const //multiplication
    {
        return VectorI(x * r.x, y * r.y, z * r.z);
    }

    constexpr const VectorI operator *(int r) const//scalar multiplication
    {
        return VectorI(x * r, y * r, z * r);
    }

    friend constexpr  VectorI operator *(int a, const VectorI & b) //multiplication
    {
        return VectorI(a * b.x, a * b.y, a * b.z);
    }

    constexpr bool operator ==(const VectorI & r) const //comparison
    {
        return x == r.x && y == r.y && z == r.z;
    }

    constexpr bool operator !=(const VectorI & r) const //inequality
    {
        return x != r.x || y != r.y || z != r.z;
    }

    const VectorI & operator +=(const VectorI & r) //adding to itself
    {
        x += r.x;
        y += r.y;
        z += r.z;
        return *this;
    }
    const VectorI & operator -=(const VectorI & r)//subtracting to itself
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        return *this;
    }
    const VectorI & operator *=(const VectorI & r)//multiplying to itself
    {
        x *= r.x;
        y *= r.y;
        z *= r.z;
        return *this;
    }
    const VectorI & operator *=(int r) //multiplying by itself
    {
        x *= r;
        y *= r;
        z *= r;
        return *this;
    }

    friend constexpr int dot(const VectorI & a, const VectorI & b)//dot product
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    friend constexpr int absSquared(const VectorI & v)//length squared
    {
        return dot(v, v);
    }

    friend float abs(const VectorI & v)//length
    {
        return sqrt(absSquared(v));
    }

    friend constexpr VectorI cross(const VectorI & a, const VectorI & b)//cross product
    {
        return VectorI(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    friend ostream & operator <<(ostream & os, const VectorI & v)//prints vector
    {
        return os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
    }
};

struct VectorF //floating point vector
{
    float x, y, z;

    constexpr VectorF(float x, float y, float z)//constructor
        : x(x), y(y), z(z)
    {
    }

    constexpr VectorF(float v = 0)//constructor for the origin
        : x(v), y(v), z(v)
    {
    }

    constexpr VectorF(const VectorI & v)//converts integer vector to float vector
        : x(v.x), y(v.y), z(v.z)
    {
    }

    explicit operator VectorI() const //sets vector to lowest it can be
    {
        return VectorI(ifloor(x), ifloor(y), ifloor(z));
    }

    constexpr const VectorF operator -() const//negates a vector
    {
        return VectorF(-x, -y, -z);
    }

    constexpr const VectorF & operator +() const//leaves a vector positive
    {
        return *this;
    }

    constexpr const VectorF operator +(const VectorF & r) const//adds two vectors
    {
        return VectorF(x + r.x, y + r.y, z + r.z);
    }

    friend constexpr VectorF operator +(const VectorF & l, const VectorI & r)//adds int and float vectors
    {
        return l + (VectorF)r;
    }

    friend constexpr VectorF operator +(const VectorI & l, const VectorF & r)//adds int and float vectors
    {
        return (VectorF)l + r;
    }

    constexpr const VectorF operator -(const VectorF & r) const
    {
        return VectorF(x - r.x, y - r.y, z - r.z);
    }

    friend constexpr VectorF operator -(const VectorF & l, const VectorI & r)
    {
        return l - (VectorF)r;
    }

    friend constexpr VectorF operator -(const VectorI & l, const VectorF & r)
    {
        return (VectorF)l - r;
    }

    constexpr const VectorF operator *(const VectorF & r) const
    {
        return VectorF(x * r.x, y * r.y, z * r.z);
    }

    friend constexpr VectorF operator *(const VectorF & l, const VectorI & r)
    {
        return l * (VectorF)r;
    }

    friend constexpr VectorF operator *(const VectorI & l, const VectorF & r)
    {
        return (VectorF)l * r;
    }

    constexpr const VectorF operator /(const VectorF & r) const
    {
        return VectorF(x / r.x, y / r.y, z / r.z);
    }

    constexpr const VectorF operator *(float r) const
    {
        return VectorF(x * r, y * r, z * r);
    }

    friend constexpr VectorF operator *(float a, const VectorF & b)
    {
        return VectorF(a * b.x, a * b.y, a * b.z);
    }

    constexpr const VectorF operator /(float r) const
    {
        return VectorF(x / r, y / r, z / r);
    }

    constexpr bool operator ==(const VectorF & r) const
    {
        return x == r.x && y == r.y && z == r.z;
    }

    constexpr bool operator !=(const VectorF & r) const
    {
        return x != r.x || y != r.y || z != r.z;
    }

    const VectorF & operator +=(const VectorF & r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
        return *this;
    }
    const VectorF & operator -=(const VectorF & r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        return *this;
    }
    const VectorF & operator *=(const VectorF & r)
    {
        x *= r.x;
        y *= r.y;
        z *= r.z;
        return *this;
    }
    const VectorF & operator /=(const VectorF & r)
    {
        x /= r.x;
        y /= r.y;
        z /= r.z;
        return *this;
    }
    const VectorF & operator *=(float r)
    {
        x *= r;
        y *= r;
        z *= r;
        return *this;
    }
    const VectorF & operator /=(float r)
    {
        x /= r;
        y /= r;
        z /= r;
        return *this;
    }

    friend constexpr float dot(const VectorF & a, const VectorF & b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    friend constexpr float dot(const VectorI & a, const VectorF & b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    friend constexpr float dot(const VectorF & a, const VectorI & b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    friend constexpr float absSquared(const VectorF & v)
    {
        return dot(v, v);
    }

    friend float abs(const VectorF & v)
    {
        return sqrt(absSquared(v));
    }

    friend const VectorF normalizeNoThrow(const VectorF & v)
    {
        float r = abs(v);
        if(r == 0)
        {
            r = 1;
        }
        return v / r;
    }

    friend const VectorF normalize(const VectorF v)
    {
        float r = abs(v);
        if(v == 0)
        {
            throw domain_error("can't normalize <0, 0, 0>");
        }
        return v / r;
    }

    const VectorF static normalize(float x, float y, float z)
    {
        VectorF v(x, y, z);
        float r = abs(v);
        if(v == 0)
        {
            throw domain_error("can't normalize <0, 0, 0>");
        }
        return v / r;
    }

    float phi() const
    {
        float r = abs(*this);
        if(r == 0)
        {
            return 0;
        }
        float v = y / r;
        v = limit(v, -1.0f, 1.0f);
        return asin(v);
    }

    float theta() const
    {
        return atan2(x, z);
    }

    float rSpherical() const
    {
        return abs(*this);
    }

    friend constexpr VectorF cross(const VectorF & a, const VectorF & b)
    {
        return VectorF(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    template<typename RE>
    static VectorF random(RE & re)
    {
        VectorF retval;
        do
        {
            retval.x = uniform_real_distribution<float>(-1, 1)(re);
            retval.y = uniform_real_distribution<float>(-1, 1)(re);
            retval.z = uniform_real_distribution<float>(-1, 1)(re);
        }
        while(absSquared(retval) > 1 || absSquared(retval) < eps);
        return retval;
    }

    friend ostream & operator <<(ostream & os, const VectorF & v)
    {
        return os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
    }
};

constexpr inline bool operator ==(const VectorF & a, const VectorI & b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

constexpr inline bool operator ==(const VectorI & a, const VectorF & b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

constexpr inline bool operator !=(const VectorF & a, const VectorI & b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

constexpr inline bool operator !=(const VectorI & a, const VectorF & b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

inline VectorF normalize(const VectorI & v)
{
    return normalize((VectorF)v);
}

inline VectorF normalizeNoThrow(const VectorI & v)
{
    return normalizeNoThrow((VectorF)v);
}

constexpr VectorF gravityVector = VectorF(0, -9.8, 0);

inline float findIntersectionPoint(VectorF a, VectorF b, VectorF normal, float d)
{
    return (dot(a, normal) + d) / dot(a - b, normal);
}

#endif // VECTOR_H
