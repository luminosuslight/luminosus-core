#ifndef HSV_ARRAY
#define HSV_ARRAY

#include <QVector>
#include <QDataStream>

// ------------------------ Basic Structs ---------------------

struct RGB;

struct HSV {
    HSV() : h(0), s(0), v(0) {}
    HSV(double x, double y, double z) : h(x), s(y), v(z) {}
    HSV(const RGB& rgb);

    bool operator==(const HSV& other) const {
        return h == other.h && s == other.s && v == other.v;
    }

    double h;
    double s;
    double v;
};

QDataStream& operator<<(QDataStream& out, const HSV& col);
QDataStream& operator>>(QDataStream& in, HSV& col);

// RGB is a windows macro, undefine it first:
#ifdef RGB
#undef RGB
#endif

struct RGB {
    RGB() : r(0), g(0), b(0) {}
    RGB(double x, double y, double z) : r(x), g(y), b(z) {}
    RGB(const HSV& hsv);

    bool operator==(const RGB& other) const {
        return r == other.r && g == other.g && b == other.b;
    }

    RGB operator*(double v) const {
        return RGB(r * v, g * v, b * v);
    }
    void operator*=(double v) {
        r *= v;
        g *= v;
        b *= v;
    }

    RGB operator+(const RGB& o) const {
        return RGB(r + o.r, g + o.g, b + o.b);
    }
    void operator+=(double v) {
        r *= v;
        g *= v;
        b *= v;
    }
    void operator+=(const RGB& o) {
        r += o.r;
        g += o.g;
        b += o.b;
    }
    void mixHtp(const RGB& o) {
        r = qMax(r, o.r);
        g = qMax(g, o.g);
        b = qMax(b, o.b);
    }
    double max() const {
        return qMax(r, qMax(g, b));
    }

    double r;
    double g;
    double b;
};

struct Size {
    Size() : width(1), height(1) {}
    Size(int w, int h) : width(w), height(h) {}
    bool operator==(const Size& other) const {
        return width == other.width && height == other.height;
    }
    int pixels() const { return width * height; }
    int width;
    int height;
};


// ----------------------- HSV ------------------------

class HsvMatrix {

public:
    HsvMatrix();  // default
    HsvMatrix(int width, int height);  // with initial size
    HsvMatrix(const HsvMatrix& other) = default;  // copy
    HsvMatrix(HsvMatrix&& other) = default;  // move
    HsvMatrix& operator=(const HsvMatrix& other) & = default; // copy assignment operator
    HsvMatrix& operator=(HsvMatrix&&) & = default;  // move assignment operator
    ~HsvMatrix() = default;  // destructor

    friend QDataStream& operator<<(QDataStream&, const HsvMatrix&);
    friend QDataStream& operator>>(QDataStream&, HsvMatrix&);

    // ---- Size:

    Size size() const { return Size(m_width, m_height); }
    int width() const { return m_width; }
    int height() const { return m_height; }
    bool hasSameSizeAs(const HsvMatrix& other) const;
    bool isSmallerThan(const HsvMatrix& other) const;

    void rescale(int width, int height);
    void rescale(const Size& s);
    void expandTo(int width, int height);
    void expandTo(const Size& s);

    // ---- Getter + Setter:

    HSV& at(int x, int y) { return m_data[abs(x % m_width)][abs(y % m_height)]; }
    const HSV& at(int x, int y) const { return m_data[abs(x % m_width)][abs(y % m_height)]; }

    void setFrom(const HsvMatrix& other);

//    void setAll(const HSV& col);
//    void setAll(double h, double s, double v);
//    void setHue(double val);
//    void setSat(double val);
//    void setVal(double val);

//    // ---- operators:

//    void operator*=(double val);

    void fadeTo(const HsvMatrix& other, double pos);


protected:
    QVector< QVector< HSV > > m_data;
    int m_width;
    int m_height;
};

QDataStream& operator<<(QDataStream& out, const HsvMatrix& matrix);
QDataStream& operator>>(QDataStream& in, HsvMatrix& matrix);


// ----------------------- RGB ------------------------

class RgbMatrix {

public:
    RgbMatrix();  // default
    RgbMatrix(int width, int height);  // with initial size
    RgbMatrix(const RgbMatrix& other) = default;  // copy
    RgbMatrix(RgbMatrix&& other) = default;  // move
    RgbMatrix& operator=(const RgbMatrix& other) & = default; // copy assignment operator
    RgbMatrix& operator=(RgbMatrix&&) & = default;  // move assignment operator
    ~RgbMatrix() = default;  // destructor

    // ---- Size:

    Size size() const { return Size(m_width, m_height); }
    int width() const { return m_width; }
    int height() const { return m_height; }
    bool hasSameSizeAs(const RgbMatrix& other) const;
    bool isSmallerThan(const RgbMatrix& other) const;

    void rescale(int width, int height);
    void rescale(const Size& s);
    void expandTo(int width, int height);
    void expandTo(const Size& s);

    // ---- Getter + Setter:

    RGB& at(int x, int y) { return m_data[abs(x % m_width)][abs(y % m_height)]; }
    const RGB& at(int x, int y) const { return m_data[abs(x % m_width)][abs(y % m_height)]; }

    void setFrom(const RgbMatrix& other);
    void addHtp(const RgbMatrix& other);

//    void setAll(const RGB& col);
//    void setAll(double r, double g, double b);

//    // ---- operators:

//    void operator*=(double val);


protected:
    QVector< QVector< RGB > > m_data;
    int m_width;
    int m_height;
};



#endif // HSV_ARRAY

