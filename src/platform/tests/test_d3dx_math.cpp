// Expected values are derived by hand from the D3DX conventions (row vectors, v * M, left-handed).
#include "x2/platform/d3dx_math.h"

#include <doctest/doctest.h>

namespace {

bool Near(const D3DXVECTOR3& a, const D3DXVECTOR3& b, float eps = 1e-5f)
{
    return std::fabs(a.x - b.x) < eps && std::fabs(a.y - b.y) < eps && std::fabs(a.z - b.z) < eps;
}

bool Near(const D3DXMATRIX& a, const D3DXMATRIX& b, float eps = 1e-5f)
{
    for (int i = 0; i < 16; ++i)
        if (std::fabs((&a._11)[i] - (&b._11)[i]) >= eps)
            return false;
    return true;
}

// same shape as the original call sites: the address of a temporary is passed to a D3DX function
float LengthOfTemporary()
{
    return D3DXVec3Length(&D3DXVECTOR3(3.0f, 4.0f, 0.0f));
}

} // namespace

TEST_CASE("D3DX layout")
{
    D3DXMATRIX m;
    D3DXMatrixIdentity(&m);
    CHECK(&m._21 == &m.m[1][0]);
    CHECK(m(3, 3) == 1.0f);
    D3DXVECTOR3 v(1, 2, 3);
    const float* p = v;
    CHECK(p[2] == 3.0f);
    CHECK(&v == static_cast<D3DXVECTOR3*>(&v)); // built-in & on lvalues
}

TEST_CASE("address of a temporary (MSVC extension) works")
{
    CHECK(LengthOfTemporary() == doctest::Approx(5.0f));
}

TEST_CASE("vector operations")
{
    const D3DXVECTOR3 a(1, 0, 0), b(0, 1, 0);
    D3DXVECTOR3 c;
    D3DXVec3Cross(&c, &a, &b);
    CHECK(c == D3DXVECTOR3(0, 0, 1));
    CHECK(D3DXVec3Dot(&a, &b) == 0.0f);
    CHECK((a + b) * 2.0f == D3DXVECTOR3(2, 2, 0));
    CHECK(2.0f * a - b == D3DXVECTOR3(2, -1, 0));

    D3DXVECTOR3 n;
    D3DXVec3Normalize(&n, &D3DXVECTOR3(0, 3, 4));
    CHECK(Near(n, D3DXVECTOR3(0, 0.6f, 0.8f)));
    D3DXVec3Normalize(&n, &D3DXVECTOR3(0, 0, 0));
    CHECK(n == D3DXVECTOR3(0, 0, 0)); // zero stays zero, no NaN
    CHECK(D3DXToDegree(D3DX_PI) == doctest::Approx(180.0f));
}

TEST_CASE("rotations follow the D3DX convention")
{
    D3DXMATRIX ry, axis;
    D3DXMatrixRotationY(&ry, D3DX_PI / 2);
    D3DXMatrixRotationAxis(&axis, &D3DXVECTOR3(0, 2, 0), D3DX_PI / 2); // axis is normalized internally
    CHECK(Near(ry, axis));

    D3DXVECTOR3 out;
    D3DXVec3TransformCoord(&out, &D3DXVECTOR3(1, 0, 0), &ry);
    CHECK(Near(out, D3DXVECTOR3(0, 0, -1))); // left-handed: +X goes to -Z around +Y

    D3DXMATRIX rz;
    D3DXMatrixRotationZ(&rz, D3DX_PI / 2);
    D3DXVec3TransformCoord(&out, &D3DXVECTOR3(1, 0, 0), &rz);
    CHECK(Near(out, D3DXVECTOR3(0, 1, 0)));
}

TEST_CASE("translation, multiply order and TransformCoord")
{
    D3DXMATRIX t, s, ts;
    D3DXMatrixTranslation(&t, 10, 0, 0);
    D3DXMatrixScaling(&s, 2, 2, 2);
    D3DXMatrixMultiply(&ts, &s, &t); // scale, then translate (row vectors)
    D3DXVECTOR3 out;
    D3DXVec3TransformCoord(&out, &D3DXVECTOR3(1, 1, 1), &ts);
    CHECK(Near(out, D3DXVECTOR3(12, 2, 2)));
    CHECK(Near(s * t, ts));

    D3DXVec3TransformNormal(&out, &D3DXVECTOR3(1, 0, 0), &ts);
    CHECK(Near(out, D3DXVECTOR3(2, 0, 0))); // no translation for normals
}

TEST_CASE("inverse and LookAtLH")
{
    D3DXMATRIX view, inv, prod, id;
    D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0, 0, -5), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
    D3DXVECTOR3 out;
    D3DXVec3TransformCoord(&out, &D3DXVECTOR3(0, 0, 0), &view);
    CHECK(Near(out, D3DXVECTOR3(0, 0, 5))); // origin is 5 units in front of the camera

    float det = 0;
    REQUIRE(D3DXMatrixInverse(&inv, &det, &view) == &inv);
    CHECK(det == doctest::Approx(1.0f));
    D3DXMatrixMultiply(&prod, &view, &inv);
    D3DXMatrixIdentity(&id);
    CHECK(Near(prod, id));

    D3DXMATRIX singular(1, 2, 3, 4, 2, 4, 6, 8, 0, 0, 1, 0, 0, 0, 0, 1);
    D3DXMATRIX untouched = id;
    CHECK(D3DXMatrixInverse(&untouched, nullptr, &singular) == nullptr);
    CHECK(untouched == id);
}

TEST_CASE("D3DXCOLOR conversion clamps and rounds like D3DX")
{
    const D3DXCOLOR c(1.0f, 0.5f, 0.0f, 2.0f);
    CHECK(static_cast<DWORD>(c) == 0xFFFF8000u);
    const D3DXCOLOR back(0x80FF0000u);
    CHECK(back.r == 1.0f);
    CHECK(back.a == doctest::Approx(128.0f / 255.0f));
}
