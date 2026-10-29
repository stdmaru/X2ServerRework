// Formulas follow D3DX 9 (row vectors, v * M). Cross-checked against the Wine d3dx9 implementation.
#include "x2/platform/d3dx_math.h"

D3DXMATRIX& D3DXMATRIX::operator*=(const D3DXMATRIX& mat)
{
    D3DXMatrixMultiply(this, this, &mat);
    return *this;
}

D3DXMATRIX& D3DXMATRIX::operator+=(const D3DXMATRIX& mat)
{
    for (int i = 0; i < 16; ++i)
        (&_11)[i] += (&mat._11)[i];
    return *this;
}

D3DXMATRIX& D3DXMATRIX::operator-=(const D3DXMATRIX& mat)
{
    for (int i = 0; i < 16; ++i)
        (&_11)[i] -= (&mat._11)[i];
    return *this;
}

D3DXMATRIX& D3DXMATRIX::operator*=(float f)
{
    for (int i = 0; i < 16; ++i)
        (&_11)[i] *= f;
    return *this;
}

bool D3DXMATRIX::operator==(const D3DXMATRIX& mat) const
{
    for (int i = 0; i < 16; ++i)
        if ((&_11)[i] != (&mat._11)[i])
            return false;
    return true;
}

D3DXVECTOR2* D3DXVec2Normalize(D3DXVECTOR2* out, const D3DXVECTOR2* v)
{
    const float len = D3DXVec2Length(v);
    *out = len == 0.0f ? D3DXVECTOR2(0.0f, 0.0f) : D3DXVECTOR2(v->x / len, v->y / len);
    return out;
}

D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* out, const D3DXVECTOR3* v)
{
    const float len = D3DXVec3Length(v);
    *out = len == 0.0f ? D3DXVECTOR3(0.0f, 0.0f, 0.0f) : D3DXVECTOR3(v->x / len, v->y / len, v->z / len);
    return out;
}

D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* out, const D3DXVECTOR3* v, const D3DXMATRIX* m)
{
    const D3DXVECTOR4 r(m->_11 * v->x + m->_21 * v->y + m->_31 * v->z + m->_41,
                        m->_12 * v->x + m->_22 * v->y + m->_32 * v->z + m->_42,
                        m->_13 * v->x + m->_23 * v->y + m->_33 * v->z + m->_43,
                        m->_14 * v->x + m->_24 * v->y + m->_34 * v->z + m->_44);
    *out = r;
    return out;
}

D3DXVECTOR3* D3DXVec3TransformCoord(D3DXVECTOR3* out, const D3DXVECTOR3* v, const D3DXMATRIX* m)
{
    const float w = m->_14 * v->x + m->_24 * v->y + m->_34 * v->z + m->_44;
    const D3DXVECTOR3 r((m->_11 * v->x + m->_21 * v->y + m->_31 * v->z + m->_41) / w,
                        (m->_12 * v->x + m->_22 * v->y + m->_32 * v->z + m->_42) / w,
                        (m->_13 * v->x + m->_23 * v->y + m->_33 * v->z + m->_43) / w);
    *out = r;
    return out;
}

D3DXVECTOR3* D3DXVec3TransformNormal(D3DXVECTOR3* out, const D3DXVECTOR3* v, const D3DXMATRIX* m)
{
    const D3DXVECTOR3 r(m->_11 * v->x + m->_21 * v->y + m->_31 * v->z,
                        m->_12 * v->x + m->_22 * v->y + m->_32 * v->z,
                        m->_13 * v->x + m->_23 * v->y + m->_33 * v->z);
    *out = r;
    return out;
}

D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* out)
{
    *out = D3DXMATRIX(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    return out;
}

BOOL D3DXMatrixIsIdentity(const D3DXMATRIX* m)
{
    D3DXMATRIX id;
    D3DXMatrixIdentity(&id);
    return *m == id ? TRUE : FALSE;
}

D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* out, const D3DXMATRIX* a, const D3DXMATRIX* b)
{
    D3DXMATRIX r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r.m[i][j] = a->m[i][0] * b->m[0][j] + a->m[i][1] * b->m[1][j] + a->m[i][2] * b->m[2][j] + a->m[i][3] * b->m[3][j];
    *out = r; // `out` may alias `a` or `b`
    return out;
}

D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* out, const D3DXMATRIX* m)
{
    D3DXMATRIX r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r.m[i][j] = m->m[j][i];
    *out = r;
    return out;
}

namespace {

// cofactor expansion; same results as D3DX up to float rounding
void Cofactors(const D3DXMATRIX& m, float inv[16], float& det)
{
    const float* a = &m._11;
    inv[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] + a[9] * a[7] * a[14] + a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
    inv[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] - a[8] * a[7] * a[14] - a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
    inv[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] + a[8] * a[7] * a[13] + a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
    inv[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] - a[8] * a[6] * a[13] - a[12] * a[5] * a[10] + a[12] * a[6] * a[9];
    inv[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] - a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
    inv[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
    inv[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
    inv[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];
    inv[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] + a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
    inv[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
    inv[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
    inv[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];
    inv[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
    inv[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] + a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
    inv[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] - a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
    inv[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] + a[8] * a[1] * a[6] - a[8] * a[2] * a[5];
    det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];
}

} // namespace

float D3DXMatrixDeterminant(const D3DXMATRIX* m)
{
    float inv[16], det;
    Cofactors(*m, inv, det);
    return det;
}

D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* out, float* determinant, const D3DXMATRIX* m)
{
    float inv[16], det;
    Cofactors(*m, inv, det);
    if (determinant)
        *determinant = det;
    if (det == 0.0f)
        return nullptr;
    const float s = 1.0f / det;
    for (int i = 0; i < 16; ++i)
        (&out->_11)[i] = inv[i] * s;
    return out;
}

D3DXMATRIX* D3DXMatrixTranslation(D3DXMATRIX* out, float x, float y, float z)
{
    *out = D3DXMATRIX(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1);
    return out;
}

D3DXMATRIX* D3DXMatrixScaling(D3DXMATRIX* out, float sx, float sy, float sz)
{
    *out = D3DXMATRIX(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
    return out;
}

D3DXMATRIX* D3DXMatrixRotationX(D3DXMATRIX* out, float angle)
{
    const float c = std::cos(angle), s = std::sin(angle);
    *out = D3DXMATRIX(1, 0, 0, 0, 0, c, s, 0, 0, -s, c, 0, 0, 0, 0, 1);
    return out;
}

D3DXMATRIX* D3DXMatrixRotationY(D3DXMATRIX* out, float angle)
{
    const float c = std::cos(angle), s = std::sin(angle);
    *out = D3DXMATRIX(c, 0, -s, 0, 0, 1, 0, 0, s, 0, c, 0, 0, 0, 0, 1);
    return out;
}

D3DXMATRIX* D3DXMatrixRotationZ(D3DXMATRIX* out, float angle)
{
    const float c = std::cos(angle), s = std::sin(angle);
    *out = D3DXMATRIX(c, s, 0, 0, -s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    return out;
}

D3DXMATRIX* D3DXMatrixRotationAxis(D3DXMATRIX* out, const D3DXVECTOR3* axis, float angle)
{
    D3DXVECTOR3 n;
    D3DXVec3Normalize(&n, axis);
    const float s = std::sin(angle), c = std::cos(angle), d = 1.0f - c;
    *out = D3DXMATRIX(d * n.x * n.x + c,       d * n.y * n.x + s * n.z, d * n.z * n.x - s * n.y, 0,
                      d * n.x * n.y - s * n.z, d * n.y * n.y + c,       d * n.z * n.y + s * n.x, 0,
                      d * n.x * n.z + s * n.y, d * n.y * n.z - s * n.x, d * n.z * n.z + c,       0,
                      0, 0, 0, 1);
    return out;
}

D3DXMATRIX* D3DXMatrixRotationYawPitchRoll(D3DXMATRIX* out, float yaw, float pitch, float roll)
{
    // roll (Z), then pitch (X), then yaw (Y)
    D3DXMATRIX z, x, y;
    D3DXMatrixRotationZ(&z, roll);
    D3DXMatrixRotationX(&x, pitch);
    D3DXMatrixRotationY(&y, yaw);
    D3DXMatrixMultiply(out, &z, &x);
    D3DXMatrixMultiply(out, out, &y);
    return out;
}

D3DXMATRIX* D3DXMatrixLookAtLH(D3DXMATRIX* out, const D3DXVECTOR3* eye, const D3DXVECTOR3* at, const D3DXVECTOR3* up)
{
    D3DXVECTOR3 zaxis = *at - *eye, xaxis, yaxis;
    D3DXVec3Normalize(&zaxis, &zaxis);
    D3DXVec3Cross(&xaxis, up, &zaxis);
    D3DXVec3Normalize(&xaxis, &xaxis);
    D3DXVec3Cross(&yaxis, &zaxis, &xaxis);
    *out = D3DXMATRIX(xaxis.x, yaxis.x, zaxis.x, 0,
                      xaxis.y, yaxis.y, zaxis.y, 0,
                      xaxis.z, yaxis.z, zaxis.z, 0,
                      -D3DXVec3Dot(&xaxis, eye), -D3DXVec3Dot(&yaxis, eye), -D3DXVec3Dot(&zaxis, eye), 1);
    return out;
}
