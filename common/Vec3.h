#pragma once
#include <iostream>
#include <cmath>

struct Vec3
{
  double x, y, z;

  // constructor
  Vec3() = default;
  constexpr Vec3(const double _x, const double _y, const double _z) : x(_x), y(_y), z(_z) {}
  constexpr Vec3(const Vec3 &v) : x(v.x), y(v.y), z(v.z) {}

  // operator
  constexpr Vec3 operator+() const
  {
    return *this;
  }

  constexpr Vec3 operator-() const
  {
    return {-x, -y, -z};
  }

  constexpr Vec3 operator+(const Vec3 &other) const
  {
    return {x + other.x, y + other.y, z + other.z};
  }

  constexpr Vec3 operator-(const Vec3 &other) const
  {
    return {x - other.x, y - other.y, z - other.z};
  }

  constexpr Vec3 operator*(double s) const
  {
    return {x * s, y * s, z * s};
  }

  constexpr Vec3 operator/(double s) const
  {
    return {x / s, y / s, z / s};
  }
  constexpr bool operator==(const Vec3 &other) const
  {
    return (x == other.x) && (y == other.y) && (z == other.z);
  }
  constexpr bool operator!=(const Vec3 &other) const
  {
    return !(*this == other);
  }

  // assignment operators
  Vec3 &operator=(const Vec3 &other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
  }
  Vec3 &operator+=(const Vec3 &other)
  {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }
  Vec3 &operator-=(const Vec3 &other)
  {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }
  Vec3 &operator*=(const double s)
  {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }
  Vec3 &operator/=(const double s)
  {
    x /= s;
    y /= s;
    z /= s;
    return *this;
  }

  // special functions
  constexpr double dot(const Vec3 &other) const
  {
    return x * other.x + y * other.y + z * other.z;
  }
  constexpr Vec3 cross(const Vec3 &other) const
  {
    return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
  }
  constexpr double lengthSquare() const
  {
    return x * x + y * y + z * z;
  }
  double length() const
  {
    return std::sqrt(lengthSquare());
  }

  double distanceFrom(const Vec3 &other) const
  {
    return (*this - other).length();
  }
  constexpr bool isZero() const
  {
    return x == 0.0 && y == 0.0 && z == 0.0;
  }

  Vec3 normalized() const
  {
    if (x != 0 || y != 0 || z != 0)
    {
      return *this / length();
    }
    return *this;
  }

  void normalize() // normalize a Vec3 in place
  {
    if (x != 0 || y != 0 || z != 0)
    {
      *this /= length();
    }
  }

  // friends
  friend constexpr Vec3 operator*(const double s, const Vec3 &a) // scalar * v1
  {
    return a * s;
  }
  friend constexpr Vec3 prod(const Vec3 &a, const Vec3 &b) // term by term *
  {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
  }
  friend constexpr double dot(const Vec3 &a, const Vec3 &b)
  {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }
  friend constexpr Vec3 cross(const Vec3 &a, const Vec3 &b)
  {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
  }
  friend constexpr double distanceSquare(const Vec3 &a, const Vec3 &b)
  {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    return dx * dx + dy * dy + dz * dz;
  }
  friend double distance(const Vec3 &a, const Vec3 &b)
  {
    return std::sqrt(distanceSquare(a, b));
  }
};

// stream
template <class Char>
inline std::basic_ostream<Char> &operator<<(std::basic_ostream<Char> &os, const Vec3 &v)
{
  return os << Char('(') << v.x << Char(',') << v.y << Char(',') << v.z << Char(')');
}

template <class Char>
inline std::basic_istream<Char> &operator>>(std::basic_istream<Char> &is, Vec3 &v)
{
  return is >> v.x >> v.y >> v.z;
}
