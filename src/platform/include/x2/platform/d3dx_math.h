// Portable replacement for the D3DX 9 math library (d3dx9math.h), which the servers and the client use as
// their vector/matrix library. Same names, same memory layout and same formulas as D3DX (row vectors, v * M,
// left-handed helpers), so the original code compiles unchanged. Only the subset used so far: grow it as
// modules are ported (the client will need much more).
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
        : r(((argb >> 16) & 0xFF) / 255.0f), g(((argb >> 8) & 0xFF) / 255.0f),
          b((argb & 0xFF) / 255.0f), a(((argb >> 24) & 0xFF) / 255.0f) {}

    // same clamping and rounding as D3DX
    operator DWORD() const
    {
        auto c = [](float v) -> DWORD { return v >= 1.0f ? 0xFFu : v <= 0.0f ? 0x00u : static_cast<DWORD>(v * 255.0f + 0.5f); };
        return (c(a) << 24) | (c(r) << 16) | (c(g) << 8) | c(b);
    }
    operator float*() { return &r; }
    operator const float*() const { return &r; }

    D3DXCOLOR& operator+=(const D3DXCOLOR& c) { r += c.r; g += c.g; b += c.b; a += c.a; return *this; }
    D3DXCOLOR& operator-=(const D3DXCOLOR& c) { r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this; }
    D3DXCOLOR& operator*=(float f) { r *= f; g *= f; b *= f; a *= f; return *this; }
    D3DXCOLOR operator+(const D3DXCOLOR& c) const { return D3DXCOLOR(r + c.r, g + c.g, b + c.b, a + c.a); }
    D3DXCOLOR operator-(const D3DXCOLOR& c) const { return D3DXCOLOR(r - c.r, g - c.g, b - c.b, a - c.a); }
    D3DXCOLOR operator*(float f) const { return D3DXCOLOR(r * f, g * f, b * f, a * f); }
    friend D3DXCOLOR operator*(float f, const D3DXCOLOR& c) { return c * f; }
    bool operator==(const D3DXCOLOR& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
    bool operator!=(const D3DXCOLOR& c) const { return !(*this == c); }

    X2_D3DX_TEMP_ADDRESS(D3DXCOLOR)
};

static_assert(sizeof(D3DXVECTOR2) == 8 && sizeof(D3DXVECTOR3) == 12 && sizeof(D3DXVECTOR4) == 16, "D3DX layout");
static_assert(sizeof(D3DXMATRIX) == 64 && sizeof(D3DXCOLOR) == 16, "D3DX layout");

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

// a zero vector normalizes to zero (D3DX behaviour)
D3DXVECTOR2* D3DXVec2Normalize(D3DXVECTOR2* out, const D3DXVECTOR2* v);
D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* out, const D3DXVECTOR3* v);

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
