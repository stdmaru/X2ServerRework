// Portable replacement for the D3DX 9 math library (d3dx9math.h), which the servers and the client use as
// their vector/matrix library. Same names, same memory layout and same formulas as D3DX (row vectors, v * M,
// left-handed helpers), so the original code compiles unchanged. Only the subset the servers and the client
// call; the types carry D3DX's full set of members. The few d3d9types.h definitions D3DX math depends on
// (D3DCOLORVALUE, D3DVIEWPORT9, D3DCOLOR_* macros, D3D_OK / D3DERR_INVALIDCALL) live here as well.
#pragma once

#include "x2/platform/types.h"

#include <cmath>

#define D3DX_PI    (3.141592654f)
#define D3DX_1BYPI (1.0f / D3DX_PI)
#define D3DXToRadian(degree) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree(radian) ((radian) * (180.0f / D3DX_PI))

// d3d9types.h constants that appear next to D3DX math in game code
#ifndef D3DFVF_XYZ
#define D3DFVF_XYZ     0x002
#define D3DFVF_DIFFUSE 0x040
#endif

#if !defined(_WIN32)
typedef DWORD D3DCOLOR;
#elif !defined(D3DCOLOR_DEFINED)
typedef DWORD D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

#ifndef D3DCOLOR_ARGB
#define D3DCOLOR_ARGB(a, r, g, b) \
    ((D3DCOLOR)((((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff)))
#define D3DCOLOR_RGBA(r, g, b, a) D3DCOLOR_ARGB(a, r, g, b)
#define D3DCOLOR_XRGB(r, g, b)    D3DCOLOR_ARGB(0xff, r, g, b)
#define D3DCOLOR_COLORVALUE(r, g, b, a) \
    D3DCOLOR_RGBA((DWORD)((r) * 255.f), (DWORD)((g) * 255.f), (DWORD)((b) * 255.f), (DWORD)((a) * 255.f))
#endif

#ifndef D3D_OK
#define D3D_OK              S_OK
#define D3DERR_INVALIDCALL  static_cast<HRESULT>(0x8876086CL) // MAKE_D3DHRESULT(2156)
#endif

// __declspec(align(16)) in D3DX
#define D3DX_ALIGN16 alignas(16)

struct D3DCOLORVALUE
{
    float r, g, b, a;
};

struct D3DVIEWPORT9
{
    DWORD X, Y, Width, Height;
    float MinZ, MaxZ;
};

// MSVC accepts `&D3DXVECTOR3(...)` (address of a temporary) as an extension, and the original passes such
// temporaries to D3DX functions. The rvalue-only operator& below gives the same result (the temporary lives
// until the end of the full expression); `&` on ordinary variables is still the built-in operator.
#define X2_D3DX_TEMP_ADDRESS(T)                              \
    const T* operator&() const&& { return this; }            \
    T* operator&() && { return this; }

struct D3DVECTOR
{
    float x, y, z;
};

struct D3DXVECTOR2
{
    float x, y;

    D3DXVECTOR2() = default;
    D3DXVECTOR2(float fx, float fy) : x(fx), y(fy) {}
    explicit D3DXVECTOR2(const float* p) : x(p[0]), y(p[1]) {}

    operator float*() { return &x; }
    operator const float*() const { return &x; }

    D3DXVECTOR2& operator+=(const D3DXVECTOR2& v) { x += v.x; y += v.y; return *this; }
    D3DXVECTOR2& operator-=(const D3DXVECTOR2& v) { x -= v.x; y -= v.y; return *this; }
    D3DXVECTOR2& operator*=(float f) { x *= f; y *= f; return *this; }
    D3DXVECTOR2& operator/=(float f) { const float inv = 1.0f / f; x *= inv; y *= inv; return *this; }

    D3DXVECTOR2 operator+() const { return *this; }
    D3DXVECTOR2 operator-() const { return D3DXVECTOR2(-x, -y); }
    D3DXVECTOR2 operator+(const D3DXVECTOR2& v) const { return D3DXVECTOR2(x + v.x, y + v.y); }
    D3DXVECTOR2 operator-(const D3DXVECTOR2& v) const { return D3DXVECTOR2(x - v.x, y - v.y); }
    D3DXVECTOR2 operator*(float f) const { return D3DXVECTOR2(x * f, y * f); }
    D3DXVECTOR2 operator/(float f) const { const float inv = 1.0f / f; return D3DXVECTOR2(x * inv, y * inv); }
    friend D3DXVECTOR2 operator*(float f, const D3DXVECTOR2& v) { return v * f; }

    bool operator==(const D3DXVECTOR2& v) const { return x == v.x && y == v.y; }
    bool operator!=(const D3DXVECTOR2& v) const { return !(*this == v); }

    X2_D3DX_TEMP_ADDRESS(D3DXVECTOR2)
};

struct D3DXVECTOR3 : public D3DVECTOR
{
    D3DXVECTOR3() = default;
    D3DXVECTOR3(float fx, float fy, float fz) : D3DVECTOR{ fx, fy, fz } {}
    D3DXVECTOR3(const D3DVECTOR& v) : D3DVECTOR(v) {}
    explicit D3DXVECTOR3(const float* p) : D3DVECTOR{ p[0], p[1], p[2] } {}

    operator float*() { return &x; }
    operator const float*() const { return &x; }

    D3DXVECTOR3& operator+=(const D3DXVECTOR3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    D3DXVECTOR3& operator-=(const D3DXVECTOR3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    D3DXVECTOR3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    D3DXVECTOR3& operator/=(float f) { const float inv = 1.0f / f; x *= inv; y *= inv; z *= inv; return *this; }

    D3DXVECTOR3 operator+() const { return *this; }
    D3DXVECTOR3 operator-() const { return D3DXVECTOR3(-x, -y, -z); }
    D3DXVECTOR3 operator+(const D3DXVECTOR3& v) const { return D3DXVECTOR3(x + v.x, y + v.y, z + v.z); }
    D3DXVECTOR3 operator-(const D3DXVECTOR3& v) const { return D3DXVECTOR3(x - v.x, y - v.y, z - v.z); }
    D3DXVECTOR3 operator*(float f) const { return D3DXVECTOR3(x * f, y * f, z * f); }
    D3DXVECTOR3 operator/(float f) const { const float inv = 1.0f / f; return D3DXVECTOR3(x * inv, y * inv, z * inv); }
    friend D3DXVECTOR3 operator*(float f, const D3DXVECTOR3& v) { return v * f; }

    bool operator==(const D3DXVECTOR3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const D3DXVECTOR3& v) const { return !(*this == v); }

    X2_D3DX_TEMP_ADDRESS(D3DXVECTOR3)
};

struct D3DXVECTOR4
{
    float x, y, z, w;

    D3DXVECTOR4() = default;
    D3DXVECTOR4(float fx, float fy, float fz, float fw) : x(fx), y(fy), z(fz), w(fw) {}
    D3DXVECTOR4(const D3DVECTOR& v, float fw) : x(v.x), y(v.y), z(v.z), w(fw) {}
    explicit D3DXVECTOR4(const float* p) : x(p[0]), y(p[1]), z(p[2]), w(p[3]) {}

    operator float*() { return &x; }
    operator const float*() const { return &x; }

    D3DXVECTOR4& operator+=(const D3DXVECTOR4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    D3DXVECTOR4& operator-=(const D3DXVECTOR4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    D3DXVECTOR4& operator*=(float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
    D3DXVECTOR4& operator/=(float f) { const float inv = 1.0f / f; return *this *= inv; }

    D3DXVECTOR4 operator+() const { return *this; }
    D3DXVECTOR4 operator-() const { return D3DXVECTOR4(-x, -y, -z, -w); }
    D3DXVECTOR4 operator+(const D3DXVECTOR4& v) const { return D3DXVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w); }
    D3DXVECTOR4 operator-(const D3DXVECTOR4& v) const { return D3DXVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w); }
    D3DXVECTOR4 operator*(float f) const { return D3DXVECTOR4(x * f, y * f, z * f, w * f); }
    D3DXVECTOR4 operator/(float f) const { const float inv = 1.0f / f; return *this * inv; }
    friend D3DXVECTOR4 operator*(float f, const D3DXVECTOR4& v) { return v * f; }

    bool operator==(const D3DXVECTOR4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    bool operator!=(const D3DXVECTOR4& v) const { return !(*this == v); }

    X2_D3DX_TEMP_ADDRESS(D3DXVECTOR4)
};

struct D3DMATRIX
{
    union
    {
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
};

struct D3DXMATRIX : public D3DMATRIX
{
    D3DXMATRIX() = default;
    D3DXMATRIX(const D3DMATRIX& mat) : D3DMATRIX(mat) {}
    explicit D3DXMATRIX(const float* p)
    {
        for (int i = 0; i < 16; ++i)
            (&_11)[i] = p[i];
    }
    D3DXMATRIX(float f11, float f12, float f13, float f14,
               float f21, float f22, float f23, float f24,
               float f31, float f32, float f33, float f34,
               float f41, float f42, float f43, float f44)
    {
        _11 = f11; _12 = f12; _13 = f13; _14 = f14;
        _21 = f21; _22 = f22; _23 = f23; _24 = f24;
        _31 = f31; _32 = f32; _33 = f33; _34 = f34;
        _41 = f41; _42 = f42; _43 = f43; _44 = f44;
    }

    float& operator()(UINT row, UINT col) { return m[row][col]; }
    float operator()(UINT row, UINT col) const { return m[row][col]; }

    operator float*() { return &_11; }
    operator const float*() const { return &_11; }

    D3DXMATRIX& operator*=(const D3DXMATRIX& mat);
    D3DXMATRIX& operator+=(const D3DXMATRIX& mat);
    D3DXMATRIX& operator-=(const D3DXMATRIX& mat);
    D3DXMATRIX& operator*=(float f);
    D3DXMATRIX& operator/=(float f) { return *this *= 1.0f / f; }

    D3DXMATRIX operator+() const { return *this; }
    D3DXMATRIX operator-() const { D3DXMATRIX r(*this); return r *= -1.0f; }
    D3DXMATRIX operator*(const D3DXMATRIX& mat) const { D3DXMATRIX r(*this); return r *= mat; }
    D3DXMATRIX operator+(const D3DXMATRIX& mat) const { D3DXMATRIX r(*this); return r += mat; }
    D3DXMATRIX operator-(const D3DXMATRIX& mat) const { D3DXMATRIX r(*this); return r -= mat; }
    D3DXMATRIX operator*(float f) const { D3DXMATRIX r(*this); return r *= f; }
    D3DXMATRIX operator/(float f) const { D3DXMATRIX r(*this); return r /= f; }
    friend D3DXMATRIX operator*(float f, const D3DXMATRIX& mat) { return mat * f; }

    bool operator==(const D3DXMATRIX& mat) const;
    bool operator!=(const D3DXMATRIX& mat) const { return !(*this == mat); }

    X2_D3DX_TEMP_ADDRESS(D3DXMATRIX)
};

struct D3DXCOLOR
{
    float r, g, b, a;

    D3DXCOLOR() = default;
    D3DXCOLOR(float fr, float fg, float fb, float fa) : r(fr), g(fg), b(fb), a(fa) {}
    D3DXCOLOR(DWORD argb)
    {
        const float f = 1.0f / 255.0f; // D3DX multiplies by 1/255 (not the same rounding as dividing)
        r = f * static_cast<float>(static_cast<unsigned char>(argb >> 16));
        g = f * static_cast<float>(static_cast<unsigned char>(argb >> 8));
        b = f * static_cast<float>(static_cast<unsigned char>(argb));
        a = f * static_cast<float>(static_cast<unsigned char>(argb >> 24));
    }
    explicit D3DXCOLOR(const float* p) : r(p[0]), g(p[1]), b(p[2]), a(p[3]) {}
    D3DXCOLOR(const D3DCOLORVALUE& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

    // same clamping and rounding as D3DX
    operator DWORD() const
    {
        auto c = [](float v) -> DWORD { return v >= 1.0f ? 0xFFu : v <= 0.0f ? 0x00u : static_cast<DWORD>(v * 255.0f + 0.5f); };
        return (c(a) << 24) | (c(r) << 16) | (c(g) << 8) | c(b);
    }
    operator float*() { return &r; }
    operator const float*() const { return &r; }
    // D3DXCOLOR and D3DCOLORVALUE share their layout; D3DX converts by reinterpreting
    operator D3DCOLORVALUE*() { return reinterpret_cast<D3DCOLORVALUE*>(&r); }
    operator const D3DCOLORVALUE*() const { return reinterpret_cast<const D3DCOLORVALUE*>(&r); }
    operator D3DCOLORVALUE&() { return *reinterpret_cast<D3DCOLORVALUE*>(&r); }
    operator const D3DCOLORVALUE&() const { return *reinterpret_cast<const D3DCOLORVALUE*>(&r); }

    D3DXCOLOR& operator+=(const D3DXCOLOR& c) { r += c.r; g += c.g; b += c.b; a += c.a; return *this; }
    D3DXCOLOR& operator-=(const D3DXCOLOR& c) { r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this; }
    D3DXCOLOR& operator*=(float f) { r *= f; g *= f; b *= f; a *= f; return *this; }
    D3DXCOLOR& operator/=(float f) { const float inv = 1.0f / f; return *this *= inv; }

    D3DXCOLOR operator+() const { return *this; }
    D3DXCOLOR operator-() const { return D3DXCOLOR(-r, -g, -b, -a); }
    D3DXCOLOR operator+(const D3DXCOLOR& c) const { return D3DXCOLOR(r + c.r, g + c.g, b + c.b, a + c.a); }
    D3DXCOLOR operator-(const D3DXCOLOR& c) const { return D3DXCOLOR(r - c.r, g - c.g, b - c.b, a - c.a); }
    D3DXCOLOR operator*(float f) const { return D3DXCOLOR(r * f, g * f, b * f, a * f); }
    D3DXCOLOR operator/(float f) const { const float inv = 1.0f / f; return *this * inv; }
    friend D3DXCOLOR operator*(float f, const D3DXCOLOR& c) { return c * f; }
    bool operator==(const D3DXCOLOR& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
    bool operator!=(const D3DXCOLOR& c) const { return !(*this == c); }

    X2_D3DX_TEMP_ADDRESS(D3DXCOLOR)
};

struct D3DXQUATERNION
{
    float x, y, z, w;

    D3DXQUATERNION() = default;
    D3DXQUATERNION(float fx, float fy, float fz, float fw) : x(fx), y(fy), z(fz), w(fw) {}
    explicit D3DXQUATERNION(const float* p) : x(p[0]), y(p[1]), z(p[2]), w(p[3]) {}

    operator float*() { return &x; }
    operator const float*() const { return &x; }

    D3DXQUATERNION& operator+=(const D3DXQUATERNION& q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
    D3DXQUATERNION& operator-=(const D3DXQUATERNION& q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
    D3DXQUATERNION& operator*=(const D3DXQUATERNION& q); // D3DXQuaternionMultiply(this, this, &q)
    D3DXQUATERNION& operator*=(float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
    D3DXQUATERNION& operator/=(float f) { const float inv = 1.0f / f; return *this *= inv; }

    D3DXQUATERNION operator+() const { return *this; }
    D3DXQUATERNION operator-() const { return D3DXQUATERNION(-x, -y, -z, -w); }
    D3DXQUATERNION operator+(const D3DXQUATERNION& q) const { return D3DXQUATERNION(x + q.x, y + q.y, z + q.z, w + q.w); }
    D3DXQUATERNION operator-(const D3DXQUATERNION& q) const { return D3DXQUATERNION(x - q.x, y - q.y, z - q.z, w - q.w); }
    D3DXQUATERNION operator*(const D3DXQUATERNION& q) const { D3DXQUATERNION r(*this); return r *= q; }
    D3DXQUATERNION operator*(float f) const { return D3DXQUATERNION(x * f, y * f, z * f, w * f); }
    D3DXQUATERNION operator/(float f) const { const float inv = 1.0f / f; return *this * inv; }
    friend D3DXQUATERNION operator*(float f, const D3DXQUATERNION& q) { return q * f; }

    bool operator==(const D3DXQUATERNION& q) const { return x == q.x && y == q.y && z == q.z && w == q.w; }
    bool operator!=(const D3DXQUATERNION& q) const { return !(*this == q); }

    X2_D3DX_TEMP_ADDRESS(D3DXQUATERNION)
};

// ax + by + cz + d = 0
struct D3DXPLANE
{
    float a, b, c, d;

    D3DXPLANE() = default;
    D3DXPLANE(float fa, float fb, float fc, float fd) : a(fa), b(fb), c(fc), d(fd) {}
    explicit D3DXPLANE(const float* p) : a(p[0]), b(p[1]), c(p[2]), d(p[3]) {}

    operator float*() { return &a; }
    operator const float*() const { return &a; }

    D3DXPLANE& operator*=(float f) { a *= f; b *= f; c *= f; d *= f; return *this; }
    D3DXPLANE& operator/=(float f) { const float inv = 1.0f / f; return *this *= inv; }

    D3DXPLANE operator+() const { return *this; }
    D3DXPLANE operator-() const { return D3DXPLANE(-a, -b, -c, -d); }
    D3DXPLANE operator*(float f) const { return D3DXPLANE(a * f, b * f, c * f, d * f); }
    D3DXPLANE operator/(float f) const { const float inv = 1.0f / f; return *this * inv; }
    friend D3DXPLANE operator*(float f, const D3DXPLANE& p) { return p * f; }

    bool operator==(const D3DXPLANE& p) const { return a == p.a && b == p.b && c == p.c && d == p.d; }
    bool operator!=(const D3DXPLANE& p) const { return !(*this == p); }

    X2_D3DX_TEMP_ADDRESS(D3DXPLANE)
};

// animation keys (d3dx9anim.h): plain data the client's animation code sorts and compresses
struct D3DXKEY_VECTOR3
{
    float Time;
    D3DXVECTOR3 Value;
};

struct D3DXKEY_QUATERNION
{
    float Time;
    D3DXQUATERNION Value;
};

static_assert(sizeof(D3DXVECTOR2) == 8 && sizeof(D3DXVECTOR3) == 12 && sizeof(D3DXVECTOR4) == 16, "D3DX layout");
static_assert(sizeof(D3DXMATRIX) == 64 && sizeof(D3DXCOLOR) == 16, "D3DX layout");
static_assert(sizeof(D3DXQUATERNION) == 16 && sizeof(D3DXPLANE) == 16 && sizeof(D3DCOLORVALUE) == 16, "D3DX layout");
static_assert(sizeof(D3DVIEWPORT9) == 24, "D3D layout");
static_assert(sizeof(D3DXKEY_VECTOR3) == 16 && sizeof(D3DXKEY_QUATERNION) == 20, "D3DX layout");

// --- vector functions (inline in d3dx9math.inl) --------------------------------------------------

inline float D3DXVec2Length(const D3DXVECTOR2* v) { return std::sqrt(v->x * v->x + v->y * v->y); }
inline float D3DXVec2LengthSq(const D3DXVECTOR2* v) { return v->x * v->x + v->y * v->y; }
inline float D3DXVec2Dot(const D3DXVECTOR2* a, const D3DXVECTOR2* b) { return a->x * b->x + a->y * b->y; }

inline float D3DXVec3Length(const D3DXVECTOR3* v) { return std::sqrt(v->x * v->x + v->y * v->y + v->z * v->z); }
inline float D3DXVec3LengthSq(const D3DXVECTOR3* v) { return v->x * v->x + v->y * v->y + v->z * v->z; }
inline float D3DXVec3Dot(const D3DXVECTOR3* a, const D3DXVECTOR3* b) { return a->x * b->x + a->y * b->y + a->z * b->z; }

inline D3DXVECTOR3* D3DXVec3Cross(D3DXVECTOR3* out, const D3DXVECTOR3* a, const D3DXVECTOR3* b)
{
    const D3DXVECTOR3 r(a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z, a->x * b->y - a->y * b->x);
    *out = r;
    return out;
}

inline D3DXVECTOR3* D3DXVec3Add(D3DXVECTOR3* out, const D3DXVECTOR3* a, const D3DXVECTOR3* b) { *out = *a + *b; return out; }
inline D3DXVECTOR3* D3DXVec3Subtract(D3DXVECTOR3* out, const D3DXVECTOR3* a, const D3DXVECTOR3* b) { *out = *a - *b; return out; }
inline D3DXVECTOR3* D3DXVec3Scale(D3DXVECTOR3* out, const D3DXVECTOR3* v, float s) { *out = *v * s; return out; }
inline D3DXVECTOR3* D3DXVec3Lerp(D3DXVECTOR3* out, const D3DXVECTOR3* a, const D3DXVECTOR3* b, float s)
{
    *out = D3DXVECTOR3(a->x + s * (b->x - a->x), a->y + s * (b->y - a->y), a->z + s * (b->z - a->z));
    return out;
}
inline D3DXVECTOR3* D3DXVec3Minimize(D3DXVECTOR3* out, const D3DXVECTOR3* a, const D3DXVECTOR3* b)
{
    *out = D3DXVECTOR3(a->x < b->x ? a->x : b->x, a->y < b->y ? a->y : b->y, a->z < b->z ? a->z : b->z);
    return out;
}
inline D3DXVECTOR3* D3DXVec3Maximize(D3DXVECTOR3* out, const D3DXVECTOR3* a, const D3DXVECTOR3* b)
{
    *out = D3DXVECTOR3(a->x > b->x ? a->x : b->x, a->y > b->y ? a->y : b->y, a->z > b->z ? a->z : b->z);
    return out;
}

inline float D3DXVec4Length(const D3DXVECTOR4* v) { return std::sqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w); }
inline float D3DXVec4LengthSq(const D3DXVECTOR4* v) { return v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w; }
inline float D3DXVec4Dot(const D3DXVECTOR4* a, const D3DXVECTOR4* b) { return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w; }

// a zero vector normalizes to zero (D3DX behaviour)
D3DXVECTOR2* D3DXVec2Normalize(D3DXVECTOR2* out, const D3DXVECTOR2* v);
D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* out, const D3DXVECTOR3* v);
// unlike Vec2/Vec3, D3DX divides by the length unchecked: a zero vector gives NaN
D3DXVECTOR4* D3DXVec4Normalize(D3DXVECTOR4* out, const D3DXVECTOR4* v);

D3DXVECTOR4* D3DXVec2Transform(D3DXVECTOR4* out, const D3DXVECTOR2* v, const D3DXMATRIX* m);
// Catmull-Rom spline through v1 (s = 0) and v2 (s = 1)
D3DXVECTOR3* D3DXVec3CatmullRom(D3DXVECTOR3* out, const D3DXVECTOR3* v0, const D3DXVECTOR3* v1,
                                const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, float s);
// object space -> screen space (world * view * projection, then the viewport); null matrices are skipped
D3DXVECTOR3* D3DXVec3Project(D3DXVECTOR3* out, const D3DXVECTOR3* v, const D3DVIEWPORT9* viewport,
                             const D3DXMATRIX* projection, const D3DXMATRIX* view, const D3DXMATRIX* world);

D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* out, const D3DXVECTOR3* v, const D3DXMATRIX* m);
D3DXVECTOR3* D3DXVec3TransformCoord(D3DXVECTOR3* out, const D3DXVECTOR3* v, const D3DXMATRIX* m);
D3DXVECTOR3* D3DXVec3TransformNormal(D3DXVECTOR3* out, const D3DXVECTOR3* v, const D3DXMATRIX* m);

// --- matrix functions ------------------------------------------------------------------------------

D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* out);
BOOL        D3DXMatrixIsIdentity(const D3DXMATRIX* m);
D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* out, const D3DXMATRIX* a, const D3DXMATRIX* b);
D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* out, const D3DXMATRIX* m);
float       D3DXMatrixDeterminant(const D3DXMATRIX* m);
// returns NULL (and leaves `out` untouched) when the matrix is singular, like D3DX
D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* out, float* determinant, const D3DXMATRIX* m);
D3DXMATRIX* D3DXMatrixTranslation(D3DXMATRIX* out, float x, float y, float z);
D3DXMATRIX* D3DXMatrixScaling(D3DXMATRIX* out, float sx, float sy, float sz);
D3DXMATRIX* D3DXMatrixRotationX(D3DXMATRIX* out, float angle);
D3DXMATRIX* D3DXMatrixRotationY(D3DXMATRIX* out, float angle);
D3DXMATRIX* D3DXMatrixRotationZ(D3DXMATRIX* out, float angle);
D3DXMATRIX* D3DXMatrixRotationAxis(D3DXMATRIX* out, const D3DXVECTOR3* axis, float angle);
D3DXMATRIX* D3DXMatrixRotationYawPitchRoll(D3DXMATRIX* out, float yaw, float pitch, float roll);
D3DXMATRIX* D3DXMatrixLookAtLH(D3DXMATRIX* out, const D3DXVECTOR3* eye, const D3DXVECTOR3* at, const D3DXVECTOR3* up);
D3DXMATRIX* D3DXMatrixPerspectiveFovLH(D3DXMATRIX* out, float fovy, float aspect, float zn, float zf);
D3DXMATRIX* D3DXMatrixOrthoLH(D3DXMATRIX* out, float w, float h, float zn, float zf);
D3DXMATRIX* D3DXMatrixRotationQuaternion(D3DXMATRIX* out, const D3DXQUATERNION* q);
// Msc^-1 * Msr^-1 * Ms * Msr * Msc * Mrc^-1 * Mr * Mrc * Mt; every argument after `out` may be null
D3DXMATRIX* D3DXMatrixTransformation(D3DXMATRIX* out, const D3DXVECTOR3* scalingCenter,
                                     const D3DXQUATERNION* scalingRotation, const D3DXVECTOR3* scaling,
                                     const D3DXVECTOR3* rotationCenter, const D3DXQUATERNION* rotation,
                                     const D3DXVECTOR3* translation);
// the same in the XY plane, rotations in radians about Z
D3DXMATRIX* D3DXMatrixTransformation2D(D3DXMATRIX* out, const D3DXVECTOR2* scalingCenter, float scalingRotation,
                                       const D3DXVECTOR2* scaling, const D3DXVECTOR2* rotationCenter, float rotation,
                                       const D3DXVECTOR2* translation);

// --- quaternion functions --------------------------------------------------------------------------

inline float D3DXQuaternionLength(const D3DXQUATERNION* q) { return std::sqrt(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w); }
inline float D3DXQuaternionLengthSq(const D3DXQUATERNION* q) { return q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w; }
inline float D3DXQuaternionDot(const D3DXQUATERNION* a, const D3DXQUATERNION* b) { return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w; }
inline D3DXQUATERNION* D3DXQuaternionIdentity(D3DXQUATERNION* out) { *out = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f); return out; }
inline D3DXQUATERNION* D3DXQuaternionConjugate(D3DXQUATERNION* out, const D3DXQUATERNION* q)
{
    *out = D3DXQUATERNION(-q->x, -q->y, -q->z, q->w);
    return out;
}

// divides by the length unchecked, like D3DX: a zero quaternion gives NaN
D3DXQUATERNION* D3DXQuaternionNormalize(D3DXQUATERNION* out, const D3DXQUATERNION* q);
// returns q1 * q2 in D3DX's order: the rotation q1 followed by q2 (Hamilton product q2 q1)
D3DXQUATERNION* D3DXQuaternionMultiply(D3DXQUATERNION* out, const D3DXQUATERNION* q1, const D3DXQUATERNION* q2);
// shortest path; falls back to linear interpolation when the quaternions are within 0.001 of each other
D3DXQUATERNION* D3DXQuaternionSlerp(D3DXQUATERNION* out, const D3DXQUATERNION* q1, const D3DXQUATERNION* q2, float t);

// --- plane functions -------------------------------------------------------------------------------

inline float D3DXPlaneDot(const D3DXPLANE* p, const D3DXVECTOR4* v) { return p->a * v->x + p->b * v->y + p->c * v->z + p->d * v->w; }
inline float D3DXPlaneDotCoord(const D3DXPLANE* p, const D3DXVECTOR3* v) { return p->a * v->x + p->b * v->y + p->c * v->z + p->d; }
inline float D3DXPlaneDotNormal(const D3DXPLANE* p, const D3DXVECTOR3* v) { return p->a * v->x + p->b * v->y + p->c * v->z; }

// a plane with a zero normal normalizes to zero
D3DXPLANE*   D3DXPlaneNormalize(D3DXPLANE* out, const D3DXPLANE* p);
D3DXPLANE*   D3DXPlaneFromPointNormal(D3DXPLANE* out, const D3DXVECTOR3* point, const D3DXVECTOR3* normal);
// intersection with the infinite line through v1 and v2 (not limited to the segment); NULL when parallel
D3DXVECTOR3* D3DXPlaneIntersectLine(D3DXVECTOR3* out, const D3DXPLANE* p, const D3DXVECTOR3* v1, const D3DXVECTOR3* v2);

// --- mesh math (d3dx9mesh.h) -----------------------------------------------------------------------

// centroid of the positions and the largest distance to it; `stride` is in bytes.
// D3DERR_INVALIDCALL on null pointers.
HRESULT D3DXComputeBoundingSphere(const D3DXVECTOR3* firstPosition, DWORD numVertices, DWORD stride,
                                  D3DXVECTOR3* center, float* radius);
