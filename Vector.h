class MathVector : public Printable {
  public:
    int x, y;
    MathVector(int, int);
    MathVector();

    MathVector& operator=(const MathVector& rhs) {
      x = rhs.x;
      y = rhs.y;
      return *this;
    }
    
    MathVector& operator=(int a) {
      x = a;
      y = a;
      return *this;
    }
    
    MathVector& operator+=(const MathVector& rhs) {
      x += rhs.x;
      y += rhs.y;
      return *this;
    }

    MathVector& operator-=(const MathVector& rhs) {
      x -= rhs.x;
      y -= rhs.y;
      return *this;
    }

    // not a dot product or a cross product, rather a scalar multiplication
    MathVector& operator*=(int a) {
      x *= a;
      y *= a;
      return *this;
    }

    // also not a dot product or a cross product, only member-wise scalar multiplication
    MathVector& operator*=(const MathVector& rhs) {
      x *= rhs.x;
      y *= rhs.y;
      return *this;
    }

    int lengthSquared() {
      return x * x + y * y;
    }

    bool isZero() {
      return x == 0 && y == 0;
    }

    size_t printTo(Print& p) const {
      size_t r = 0;

      r += p.print('(');
      r += p.print(x);
      r += p.print(", ");
      r += p.print(y);
      r += p.print(')');
      return r;
    }
};

MathVector::MathVector(int a, int b) : x(a), y(b) {}

MathVector::MathVector() : x(0), y(0) {}

inline MathVector operator+(MathVector lhs, const MathVector& rhs) {
  lhs += rhs;
  return lhs;
}

inline MathVector operator-(MathVector lhs, const MathVector& rhs) {
  lhs -= rhs;
  return lhs;
}

inline MathVector operator*(MathVector lhs, int a) {
  lhs *= a;
  return lhs;
}

inline MathVector operator*(MathVector lhs, const MathVector& rhs) {
  lhs *= rhs;
  return lhs;
}

inline bool operator==(const MathVector& lhs, const MathVector& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator<(const MathVector& lhs, const MathVector& rhs) {
  return (lhs.x * lhs.x + lhs.y * lhs.y) < (rhs.x * rhs.x + rhs.y * rhs.y);
}

inline bool operator>(const MathVector& lhs, const MathVector& rhs) {
  return rhs < lhs;
}
inline bool operator<=(const MathVector& lhs, const MathVector& rhs) {
  return !(lhs > rhs);
}
inline bool operator>=(const MathVector& lhs, const MathVector& rhs) {
  return !(rhs < lhs);
}
