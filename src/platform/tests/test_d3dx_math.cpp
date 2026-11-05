// Expected values are derived by hand from the D3DX conventions (row vectors, v * M, left-handed).
#include "x2/platform/d3dx_math.h"

#include <doctest/doctest.h>

#include <cstddef>
#include <cstdint>

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

bool Near(const D3DXVECTOR4& a, const D3DXVECTOR4& b, float eps = 1e-5f)
{
    return std::fabs(a.x - b.x) < eps && std::fabs(a.y - b.y) < eps && std::fabs(a.z - b.z) < eps &&
           std::fabs(a.w - b.w) < eps;
}

bool Near(const D3DXQUATERNION& a, const D3DXQUATERNION& b, float eps = 1e-5f)
{
    return Near(D3DXVECTOR4(a.x, a.y, a.z, a.w), D3DXVECTOR4(b.x, b.y, b.z, b.w), eps);
}

D3DXQUATERNION AboutAxis(float x, float y, float z, float angle)
{
    const float s = std::sin(angle / 2.0f);
    return D3DXQUATERNION(x * s, y * s, z * s, std::cos(angle / 2.0f));
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

TEST_CASE("D3DXCOLOR members and d3d9types color macros")
{
    const D3DXCOLOR back(0x80FF0000u);
    CHECK(back.a == (1.0f / 255.0f) * 128.0f); // D3DX multiplies by 1/255
    CHECK(-back == D3DXCOLOR(-1.0f, 0.0f, 0.0f, -back.a));
    CHECK(D3DXCOLOR(1.0f, 0.5f, 0.0f, 2.0f) / 2.0f == D3DXCOLOR(0.5f, 0.25f, 0.0f, 1.0f));

    D3DCOLORVALUE value{ 0.25f, 0.5f, 0.75f, 1.0f };
    const D3DXCOLOR fromValue(value);
    CHECK(fromValue == D3DXCOLOR(0.25f, 0.5f, 0.75f, 1.0f));
    D3DXCOLOR c(0.0f, 0.0f, 0.0f, 0.0f);
    D3DCOLORVALUE& ref = c; // material colors are assigned through this conversion
    ref.g = 1.0f;
    CHECK(c.g == 1.0f);
    value = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
    CHECK(value.r == 1.0f);

    CHECK(D3DCOLOR_ARGB(0x80, 1, 2, 3) == 0x80010203u);
    CHECK(D3DCOLOR_RGBA(1, 2, 3, 0x80) == 0x80010203u);
    CHECK(D3DCOLOR_XRGB(1, 2, 3) == 0xFF010203u);
    CHECK(D3DCOLOR_COLORVALUE(1.0f, 0.5f, 0.0f, 1.0f) == 0xFFFF7F00u); // truncates: 0.5 * 255 -> 0x7F
}

TEST_CASE("D3DXMatrixPerspectiveFovLH and D3DXMatrixOrthoLH")
{
    D3DXMATRIX m;
    D3DXMatrixPerspectiveFovLH(&m, D3DX_PI / 2.0f, 2.0f, 1.0f, 11.0f);
    CHECK(Near(m, D3DXMATRIX(0.5f, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1.1f, 1, 0, 0, -1.1f, 0)));
    D3DXVECTOR3 out;
    D3DXVec3TransformCoord(&out, &D3DXVECTOR3(0, 0, 1), &m);
    CHECK(out.z == doctest::Approx(0.0f)); // near plane -> 0
    D3DXVec3TransformCoord(&out, &D3DXVECTOR3(0, 0, 11), &m);
    CHECK(out.z == doctest::Approx(1.0f)); // far plane -> 1

    D3DXMatrixOrthoLH(&m, 4.0f, 2.0f, 1.0f, 3.0f);
    CHECK(Near(m, D3DXMATRIX(0.5f, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.5f, 0, 0, 0, -0.5f, 1)));
}

TEST_CASE("D3DXVec3Project")
{
    const D3DVIEWPORT9 vp{ 0, 0, 800, 600, 0.0f, 1.0f };
    D3DXMATRIX proj, id;
    D3DXMatrixOrthoLH(&proj, 800.0f, 600.0f, 0.0f, 1.0f);
    D3DXMatrixIdentity(&id);
    D3DXVECTOR3 out;
    D3DXVec3Project(&out, &D3DXVECTOR3(200, 150, 0.5f), &vp, &proj, &id, &id);
    CHECK(Near(out, D3DXVECTOR3(600, 150, 0.5f)));
    D3DXVec3Project(&out, &D3DXVECTOR3(200, 150, 0.5f), &vp, &proj, nullptr, nullptr); // null matrices are skipped
    CHECK(Near(out, D3DXVECTOR3(600, 150, 0.5f)));

    const D3DVIEWPORT9 offset{ 100, 50, 200, 100, 0.5f, 1.0f };
    D3DXVec3Project(&out, &D3DXVECTOR3(0, 0, 0), &offset, nullptr, nullptr, nullptr);
    CHECK(Near(out, D3DXVECTOR3(200, 100, 0.5f))); // center of the viewport, MinZ
    D3DXVec3Project(&out, &D3DXVECTOR3(0.5f, 0.5f, 0.5f), nullptr, nullptr, nullptr, nullptr);
    CHECK(Near(out, D3DXVECTOR3(0.5f, 0.5f, 0.5f)));
}

TEST_CASE("D3DXVec3CatmullRom, D3DXVec2Transform, D3DXVec4Normalize")
{
    const D3DXVECTOR3 p0(0, 0, 0), p1(1, 1, 0), p2(2, 4, 0), p3(3, 9, 0); // (i, i^2)
    D3DXVECTOR3 out;
    D3DXVec3CatmullRom(&out, &p0, &p1, &p2, &p3, 0.0f);
    CHECK(Near(out, p1));
    D3DXVec3CatmullRom(&out, &p0, &p1, &p2, &p3, 1.0f);
    CHECK(Near(out, p2));
    D3DXVec3CatmullRom(&out, &p0, &p1, &p2, &p3, 0.5f);
    CHECK(Near(out, D3DXVECTOR3(1.5f, 2.25f, 0))); // Catmull-Rom reproduces quadratics

    D3DXMATRIX m;
    D3DXMatrixTranslation(&m, 10, 20, 30);
    D3DXVECTOR4 v4;
    D3DXVec2Transform(&v4, &D3DXVECTOR2(1, 2), &m);
    CHECK(Near(v4, D3DXVECTOR4(11, 22, 30, 1)));

    D3DXVec4Normalize(&v4, &D3DXVECTOR4(1, 2, 2, 4));
    CHECK(Near(v4, D3DXVECTOR4(0.2f, 0.4f, 0.4f, 0.8f)));
    D3DXVec4Normalize(&v4, &D3DXVECTOR4(0, 0, 0, 0));
    CHECK(std::isnan(v4.x)); // D3DX does not check the length
}

TEST_CASE("D3DXQUATERNION functions")
{
    D3DX_ALIGN16 D3DXQUATERNION q, q2;
    CHECK(reinterpret_cast<std::uintptr_t>(&q) % 16 == 0);
    CHECK(reinterpret_cast<std::uintptr_t>(&q2) % 16 == 0);

    D3DXQuaternionNormalize(&q, &D3DXQUATERNION(0, 0, 3, 4));
    CHECK(Near(q, D3DXQUATERNION(0, 0, 0.6f, 0.8f)));
    D3DXQuaternionNormalize(&q2, &D3DXQUATERNION(0, 0, 0, 0));
    CHECK(std::isnan(q2.w));
    D3DXQuaternionConjugate(&q2, &D3DXQUATERNION(1, 2, 3, 4));
    CHECK(q2 == D3DXQUATERNION(-1, -2, -3, 4));
    CHECK(-q2 == D3DXQUATERNION(1, 2, 3, -4));

    // matches the rotation matrices
    const D3DXQUATERNION z90 = AboutAxis(0, 0, 1, D3DX_PI / 2), x90 = AboutAxis(1, 0, 0, D3DX_PI / 2);
    D3DXMATRIX mq, mz, mx;
    D3DXMatrixRotationQuaternion(&mq, &z90);
    D3DXMatrixRotationZ(&mz, D3DX_PI / 2);
    CHECK(Near(mq, mz));

    // Multiply(q1, q2) is q1 followed by q2, like the matrix product M1 * M2
    D3DXQUATERNION prod;
    D3DXQuaternionMultiply(&prod, &x90, &z90);
    D3DXMatrixRotationQuaternion(&mq, &prod);
    D3DXMatrixRotationX(&mx, D3DX_PI / 2);
    CHECK(Near(mq, mx * mz));
    CHECK(Near(x90 * z90, prod));
    D3DXQuaternionMultiply(&prod, &z90, &z90);
    CHECK(Near(prod, D3DXQUATERNION(0, 0, 1, 0)));
    q = x90;
    q *= z90; // aliasing through operator*=
    CHECK(Near(q, x90 * z90));

    const D3DXQUATERNION id(0, 0, 0, 1), z180(0, 0, 1, 0);
    D3DXQuaternionSlerp(&q, &id, &z180, 0.5f);
    CHECK(Near(q, z90));
    D3DXQuaternionSlerp(&q, &id, &z180, 0.0f);
    CHECK(Near(q, id));
    D3DXQuaternionSlerp(&q, &id, &D3DXQUATERNION(0, 0, 0, -1), 0.5f); // same rotation, opposite sign: shortest path
    CHECK(Near(q, id));
    CHECK(D3DXQuaternionDot(&z90, &z90) == doctest::Approx(1.0f));
    CHECK(D3DXQuaternionLength(&z90) == doctest::Approx(1.0f));
}

TEST_CASE("D3DXPLANE functions")
{
    D3DXPLANE p;
    D3DXPlaneFromPointNormal(&p, &D3DXVECTOR3(0, 5, 0), &D3DXVECTOR3(0, 1, 0));
    CHECK(p == D3DXPLANE(0, 1, 0, -5));
    CHECK(D3DXPlaneDotCoord(&p, &D3DXVECTOR3(3, 7, 4)) == 2.0f);
    CHECK(D3DXPlaneDotNormal(&p, &D3DXVECTOR3(3, 7, 4)) == 7.0f);

    D3DXPLANE n;
    D3DXPlaneNormalize(&n, &D3DXPLANE(0, 2, 0, -10));
    CHECK(n == D3DXPLANE(0, 1, 0, -5));
    D3DXPlaneNormalize(&n, &D3DXPLANE(0, 0, 0, 3));
    CHECK(n == D3DXPLANE(0, 0, 0, 0));

    D3DXVECTOR3 hit;
    REQUIRE(D3DXPlaneIntersectLine(&hit, &p, &D3DXVECTOR3(1, 0, 1), &D3DXVECTOR3(1, 10, 1)) == &hit);
    CHECK(Near(hit, D3DXVECTOR3(1, 5, 1)));
    REQUIRE(D3DXPlaneIntersectLine(&hit, &p, &D3DXVECTOR3(1, 0, 1), &D3DXVECTOR3(1, 1, 1)) == &hit);
    CHECK(Near(hit, D3DXVECTOR3(1, 5, 1))); // the line, not the segment
    CHECK(D3DXPlaneIntersectLine(&hit, &p, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(1, 0, 0)) == nullptr);
    CHECK(-p * 2.0f == D3DXPLANE(0, -2, 0, 10));
}

TEST_CASE("D3DXMatrixTransformation and D3DXMatrixTransformation2D")
{
    D3DXMATRIX m, id;
    D3DXMatrixIdentity(&id);
    D3DXMatrixTransformation(&m, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    CHECK(m == id);

    // scale x2 about (1, 1), rotate 90 degrees about the origin, translate by (10, 0)
    const D3DXVECTOR2 sc(1, 1), s(2, 2), t(10, 0);
    D3DXMatrixTransformation2D(&m, &sc, 0.0f, &s, nullptr, D3DX_PI / 2, &t);
    D3DXVECTOR4 out;
    D3DXVec2Transform(&out, &D3DXVECTOR2(2, 1), &m);
    CHECK(Near(out, D3DXVECTOR4(9, 3, 0, 1), 1e-4f));
    CHECK(m._33 == 1.0f); // z scaling is 1

    // the scaling rotation orients the scaling axes: x2 along x rotated by 90 degrees scales y
    const D3DXVECTOR2 sx(2, 1);
    D3DXMatrixTransformation2D(&m, nullptr, D3DX_PI / 2, &sx, nullptr, 0.0f, nullptr);
    D3DXVec2Transform(&out, &D3DXVECTOR2(0, 1), &m);
    CHECK(Near(out, D3DXVECTOR4(0, 2, 0, 1), 1e-5f));
    D3DXVec2Transform(&out, &D3DXVECTOR2(1, 0), &m);
    CHECK(Near(out, D3DXVECTOR4(1, 0, 0, 1), 1e-5f));

    // rotation about a center
    const D3DXVECTOR2 rc(1, 0);
    D3DXMatrixTransformation2D(&m, nullptr, 0.0f, nullptr, &rc, D3DX_PI, nullptr);
    D3DXVec2Transform(&out, &D3DXVECTOR2(0, 0), &m);
    CHECK(Near(out, D3DXVECTOR4(2, 0, 0, 1), 1e-5f));
}

TEST_CASE("D3DXComputeBoundingSphere with a vertex stride")
{
    struct Vertex
    {
        D3DXVECTOR3 pos;
        float u, v;
    };
    const Vertex vertices[] = { { { 1, 0, 0 }, 0, 0 }, { { -1, 0, 0 }, 0, 0 }, { { 0, 3, 0 }, 0, 0 }, { { 0, -3, 0 }, 0, 0 } };
    D3DXVECTOR3 center;
    float radius = -1;
    CHECK(D3DXComputeBoundingSphere(&vertices[0].pos, 4, sizeof(Vertex), &center, &radius) == D3D_OK);
    CHECK(Near(center, D3DXVECTOR3(0, 0, 0)));
    CHECK(radius == doctest::Approx(3.0f));
    CHECK(D3DXComputeBoundingSphere(nullptr, 4, sizeof(Vertex), &center, &radius) == D3DERR_INVALIDCALL);
}

TEST_CASE("D3DX animation keys")
{
    D3DXKEY_QUATERNION key{ 1.5f, D3DXQUATERNION(0, 0, 0, 1) };
    CHECK(offsetof(D3DXKEY_QUATERNION, Value) == 4);
    CHECK(key.Value.w == 1.0f);
}
