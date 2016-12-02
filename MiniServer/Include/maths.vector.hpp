#pragma once
#include <cmath>
using std::sqrtf;

class Vector3 
{
public:
    Vector3() 
    {}
    Vector3(float X, float Y, float Z) 
        : x(X)
        , y(Y)
        , z(Z) 
    {}
    float x, y, z;
    
    // Maths
    // Dot Product
    inline float dot(const Vector3& vec2)
    {
        return ((x*vec2.x) + (y*vec2.y) + (z*vec2.z));
    }

    // Cross Product
    inline Vector3 cross(const Vector3& vec2)
    {
        return Vector3(y*vec2.z - z*vec2.y,
                     -(x*vec2.z - z*vec2.x),
                       x*vec2.y - y*vec2.x);
    }

    inline void normalise()
    {
        float mag = magnitude();
        if (mag)
        {
            float multiplier = 1.0f / mag;
            x *= multiplier;
            y *= multiplier;
            z *= multiplier;
        }
    }

    inline float magnitude()
    {
        return sqrtf(magnitudeSqrd());
    }

    inline float magnitudeSqrd()
    {
        return (x*x + y*y + z*z);
    }
    
    inline bool equals(const Vector3& vec2)
    {
        return equals(vec2, 0.00001f);
    }

    inline bool equals(const Vector3& vec2, float epsilon)
    {
        return((fabs(x - vec2.x) < epsilon) &&
            (fabs(y - vec2.y) < epsilon) &&
            (fabs(z - vec2.x) < epsilon));
    }

    // Operator overloads
    inline Vector3& operator+=(const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    inline Vector3& operator-=(const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    inline Vector3& operator/=(const Vector3& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    inline Vector3& operator*=(const Vector3& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }


    inline Vector3 operator+(const Vector3& rhs)
    {
        return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    inline Vector3 operator-(const Vector3& rhs)
    {
        return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    inline Vector3 operator/(const Vector3& rhs)
    {
        return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
    }

    inline Vector3 operator*(const Vector3& rhs)
    {
        return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    inline Vector3 operator*(const float rhs)
    {
        return Vector3(x*rhs, y*rhs, z*rhs);
    }

    inline Vector3 operator/(const float rhs)
    {
        return Vector3(x / rhs, y / rhs, z / rhs);
    }
};

inline Vector3 operator*(int lhs, Vector3 rhs)
{
    return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

inline Vector3 operator/(int lhs, Vector3 rhs)
{
    return Vector3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

inline Vector3 operator*(float lhs, Vector3 rhs)
{
    return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

inline Vector3 operator/(float lhs, Vector3 rhs)
{
    return Vector3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

class Vector2
{
public:
    Vector2() 
    {}
    Vector2(float X, float Y) 
        : x(X)
        , y(Y) 
    {}
    float x, y;

    // Maths
    // Dot Product
    inline float dot(const Vector2& vec2)
    {
        return ((x*vec2.x) + (y*vec2.y));
    }
    //// Cross Product
    //Vector2 cross(const Vector2& vec2);
    inline void normalise()
    {
        float mag = magnitude();
        if (mag)
        {
            float multiplier = 1.0f / mag;
            x *= multiplier;
            y *= multiplier;
        }
    }

    inline float magnitude()
    {
        return sqrtf(magnitudeSqrd());
    }

    inline float magnitudeSqrd()
    {
        return (x*x + y*y);
    }
    
    inline bool equals(const Vector2& vec2)
    {
        return equals(vec2, 0.00001f);
    }

    inline bool equals(const Vector2& vec2, float epsilon)
    {
        return ((fabs(x - vec2.x) < epsilon) &&
                (fabs(y - vec2.y) < epsilon));
    }

    // Operator overloads
    inline Vector2& operator+=(const Vector2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    inline Vector2& operator-=(const Vector2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    inline Vector2& operator/=(const Vector2& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    inline Vector2& operator*=(const Vector2& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    inline Vector2 operator+(const Vector2& rhs)
    {
        return Vector2(x + rhs.x, y += rhs.y);
    }

    inline Vector2 operator-(const Vector2& rhs)
    {
        return Vector2(x - rhs.x, y - rhs.y);
    }

    inline Vector2 operator/(const Vector2& rhs)
    {
        return Vector2(x / rhs.x, y / rhs.y);
    }

    inline Vector2 operator*(const Vector2& rhs)
    {
        return Vector2(x * rhs.x, y * rhs.y);
    }
};

inline Vector2 operator*(int lhs, Vector2 rhs)
{
    return Vector2(lhs*rhs.x, lhs*rhs.y);
}

inline Vector2 operator/(int lhs, Vector2 rhs)
{
    return Vector2(lhs / rhs.x, lhs / rhs.y);
}

inline Vector2 operator*(float lhs, Vector2 rhs)
{
    return Vector2(lhs * rhs.x, lhs * rhs.y);
}

inline Vector2 operator/(float lhs, Vector2 rhs)
{
    return Vector2(lhs / rhs.x, lhs / rhs.y);
}
