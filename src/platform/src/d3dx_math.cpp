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

D3DXMATRIX* D3DXMatrixPerspectiveFovLH(D3DXMATRIX* out, float fovy, float aspect, float zn, float zf)
{
    D3DXMatrixIdentity(out);
    out->m[0][0] = 1.0f / (aspect * std::tan(fovy / 2.0f));
    out->m[1][1] = 1.0f / std::tan(fovy / 2.0f);
    out->m[2][2] = zf / (zf - zn);
    out->m[2][3] = 1.0f;
    out->m[3][2] = (zf * zn) / (zn - zf);
    out->m[3][3] = 0.0f;
    return out;
}

D3DXMATRIX* D3DXMatrixOrthoLH(D3DXMATRIX* out, float w, float h, float zn, float zf)
{
    D3DXMatrixIdentity(out);
    out->m[0][0] = 2.0f / w;
    out->m[1][1] = 2.0f / h;
    out->m[2][2] = 1.0f / (zf - zn);
    out->m[3][2] = zn / (zn - zf);
    return out;
}

D3DXMATRIX* D3DXMatrixRotationQuaternion(D3DXMATRIX* out, const D3DXQUATERNION* q)
{
    D3DXMatrixIdentity(out);
    out->m[0][0] = 1.0f - 2.0f * (q->y * q->y + q->z * q->z);
    out->m[0][1] = 2.0f * (q->x * q->y + q->z * q->w);
    out->m[0][2] = 2.0f * (q->x * q->z - q->y * q->w);
    out->m[1][0] = 2.0f * (q->x * q->y - q->z * q->w);
    out->m[1][1] = 1.0f - 2.0f * (q->x * q->x + q->z * q->z);
    out->m[1][2] = 2.0f * (q->y * q->z + q->x * q->w);
    out->m[2][0] = 2.0f * (q->x * q->z + q->y * q->w);
    out->m[2][1] = 2.0f * (q->y * q->z - q->x * q->w);
    out->m[2][2] = 1.0f - 2.0f * (q->x * q->x + q->y * q->y);
    return out;
}

D3DXMATRIX* D3DXMatrixTransformation(D3DXMATRIX* out, const D3DXVECTOR3* scalingCenter,
                                     const D3DXQUATERNION* scalingRotation, const D3DXVECTOR3* scaling,
                                     const D3DXVECTOR3* rotationCenter, const D3DXQUATERNION* rotation,
                                     const D3DXVECTOR3* translation)
{
    const D3DXVECTOR3 zero(0.0f, 0.0f, 0.0f);
    D3DXMATRIX m1, t;

    if (scaling)
    {
        const D3DXVECTOR3 sc = scalingCenter ? *scalingCenter : zero;
        D3DXMatrixTranslation(&m1, -sc.x, -sc.y, -sc.z);
        if (scalingRotation)
        {
            const D3DXQUATERNION inv(-scalingRotation->x, -scalingRotation->y, -scalingRotation->z, scalingRotation->w);
            D3DXMatrixRotationQuaternion(&t, &inv);
            D3DXMatrixMultiply(&m1, &m1, &t);
        }
        D3DXMatrixScaling(&t, scaling->x, scaling->y, scaling->z);
        D3DXMatrixMultiply(&m1, &m1, &t);
        if (scalingRotation)
        {
            D3DXMatrixRotationQuaternion(&t, scalingRotation);
            D3DXMatrixMultiply(&m1, &m1, &t);
        }
        D3DXMatrixTranslation(&t, sc.x, sc.y, sc.z);
        D3DXMatrixMultiply(&m1, &m1, &t);
    }
    else
        D3DXMatrixIdentity(&m1);

    if (rotation)
    {
        const D3DXVECTOR3 rc = rotationCenter ? *rotationCenter : zero;
        D3DXMatrixTranslation(&t, -rc.x, -rc.y, -rc.z);
        D3DXMatrixMultiply(&m1, &m1, &t);
        D3DXMatrixRotationQuaternion(&t, rotation);
        D3DXMatrixMultiply(&m1, &m1, &t);
        D3DXMatrixTranslation(&t, rc.x, rc.y, rc.z);
        D3DXMatrixMultiply(&m1, &m1, &t);
    }

    if (translation)
    {
        D3DXMatrixTranslation(&t, translation->x, translation->y, translation->z);
        D3DXMatrixMultiply(out, &m1, &t);
    }
    else
        *out = m1;
    return out;
}

D3DXMATRIX* D3DXMatrixTransformation2D(D3DXMATRIX* out, const D3DXVECTOR2* scalingCenter, float scalingRotation,
                                       const D3DXVECTOR2* scaling, const D3DXVECTOR2* rotationCenter, float rotation,
                                       const D3DXVECTOR2* translation)
{
    auto to3 = [](const D3DXVECTOR2* v, float z) { return v ? D3DXVECTOR3(v->x, v->y, z) : D3DXVECTOR3(0, 0, 0); };
    const D3DXVECTOR3 sc = to3(scalingCenter, 0.0f), s = to3(scaling, 1.0f), rc = to3(rotationCenter, 0.0f),
                      t = to3(translation, 0.0f);
    // a zero angle is passed on as "no rotation", like D3DX
    const D3DXQUATERNION r(0.0f, 0.0f, std::sin(rotation / 2.0f), std::cos(rotation / 2.0f));
    const D3DXQUATERNION sr(0.0f, 0.0f, std::sin(scalingRotation / 2.0f), std::cos(scalingRotation / 2.0f));
    return D3DXMatrixTransformation(out, scalingCenter ? &sc : nullptr, scalingRotation != 0.0f ? &sr : nullptr,
                                    scaling ? &s : nullptr, rotationCenter ? &rc : nullptr,
                                    rotation != 0.0f ? &r : nullptr, translation ? &t : nullptr);
}

D3DXVECTOR4* D3DXVec4Normalize(D3DXVECTOR4* out, const D3DXVECTOR4* v)
{
    const float norm = D3DXVec4Length(v);
    *out = D3DXVECTOR4(v->x / norm, v->y / norm, v->z / norm, v->w / norm);
    return out;
}

D3DXVECTOR4* D3DXVec2Transform(D3DXVECTOR4* out, const D3DXVECTOR2* v, const D3DXMATRIX* m)
{
    const D3DXVECTOR4 r(m->_11 * v->x + m->_21 * v->y + m->_41,
                        m->_12 * v->x + m->_22 * v->y + m->_42,
                        m->_13 * v->x + m->_23 * v->y + m->_43,
                        m->_14 * v->x + m->_24 * v->y + m->_44);
    *out = r;
    return out;
}

D3DXVECTOR3* D3DXVec3CatmullRom(D3DXVECTOR3* out, const D3DXVECTOR3* v0, const D3DXVECTOR3* v1,
                                const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, float s)
{
    auto f = [s](float p0, float p1, float p2, float p3) {
        return 0.5f * (2.0f * p1 + (p2 - p0) * s + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * s * s +
                       (p3 - 3.0f * p2 + 3.0f * p1 - p0) * s * s * s);
    };
    *out = D3DXVECTOR3(f(v0->x, v1->x, v2->x, v3->x), f(v0->y, v1->y, v2->y, v3->y), f(v0->z, v1->z, v2->z, v3->z));
    return out;
}

D3DXVECTOR3* D3DXVec3Project(D3DXVECTOR3* out, const D3DXVECTOR3* v, const D3DVIEWPORT9* viewport,
                             const D3DXMATRIX* projection, const D3DXMATRIX* view, const D3DXMATRIX* world)
{
    D3DXMATRIX m;
    D3DXMatrixIdentity(&m);
    if (world)
        D3DXMatrixMultiply(&m, &m, world);
    if (view)
        D3DXMatrixMultiply(&m, &m, view);
    if (projection)
        D3DXMatrixMultiply(&m, &m, projection);
    D3DXVec3TransformCoord(out, v, &m);
    if (viewport)
    {
        out->x = static_cast<float>(viewport->X) + (1.0f + out->x) * static_cast<float>(viewport->Width) / 2.0f;
        out->y = static_cast<float>(viewport->Y) + (1.0f - out->y) * static_cast<float>(viewport->Height) / 2.0f;
        out->z = viewport->MinZ + out->z * (viewport->MaxZ - viewport->MinZ);
    }
    return out;
}

D3DXQUATERNION& D3DXQUATERNION::operator*=(const D3DXQUATERNION& q)
{
    D3DXQuaternionMultiply(this, this, &q);
    return *this;
}

D3DXQUATERNION* D3DXQuaternionNormalize(D3DXQUATERNION* out, const D3DXQUATERNION* q)
{
    const float norm = D3DXQuaternionLength(q);
    *out = D3DXQUATERNION(q->x / norm, q->y / norm, q->z / norm, q->w / norm);
    return out;
}

D3DXQUATERNION* D3DXQuaternionMultiply(D3DXQUATERNION* out, const D3DXQUATERNION* q1, const D3DXQUATERNION* q2)
{
    const D3DXQUATERNION r(q2->w * q1->x + q2->x * q1->w + q2->y * q1->z - q2->z * q1->y,
                           q2->w * q1->y - q2->x * q1->z + q2->y * q1->w + q2->z * q1->x,
                           q2->w * q1->z + q2->x * q1->y - q2->y * q1->x + q2->z * q1->w,
                           q2->w * q1->w - q2->x * q1->x - q2->y * q1->y - q2->z * q1->z);
    *out = r; // `out` may alias `q1` or `q2`
    return out;
}

D3DXQUATERNION* D3DXQuaternionSlerp(D3DXQUATERNION* out, const D3DXQUATERNION* q1, const D3DXQUATERNION* q2, float t)
{
    float temp = 1.0f - t;
    float dot = D3DXQuaternionDot(q1, q2);
    if (dot < 0.0f)
    {
        t = -t;
        dot = -dot;
    }
    if (1.0f - dot > 0.001f)
    {
        const float theta = std::acos(dot);
        temp = std::sin(theta * temp) / std::sin(theta);
        t = std::sin(theta * t) / std::sin(theta);
    }
    const D3DXQUATERNION r(temp * q1->x + t * q2->x, temp * q1->y + t * q2->y, temp * q1->z + t * q2->z,
                           temp * q1->w + t * q2->w);
    *out = r;
    return out;
}

D3DXPLANE* D3DXPlaneNormalize(D3DXPLANE* out, const D3DXPLANE* p)
{
    const float norm = std::sqrt(p->a * p->a + p->b * p->b + p->c * p->c);
    *out = norm != 0.0f ? D3DXPLANE(p->a / norm, p->b / norm, p->c / norm, p->d / norm) : D3DXPLANE(0, 0, 0, 0);
    return out;
}

D3DXPLANE* D3DXPlaneFromPointNormal(D3DXPLANE* out, const D3DXVECTOR3* point, const D3DXVECTOR3* normal)
{
    *out = D3DXPLANE(normal->x, normal->y, normal->z, -D3DXVec3Dot(point, normal));
    return out;
}

D3DXVECTOR3* D3DXPlaneIntersectLine(D3DXVECTOR3* out, const D3DXPLANE* p, const D3DXVECTOR3* v1, const D3DXVECTOR3* v2)
{
    const D3DXVECTOR3 normal(p->a, p->b, p->c);
    const D3DXVECTOR3 direction = *v2 - *v1;
    const float dot = D3DXVec3Dot(&normal, &direction);
    if (dot == 0.0f)
        return nullptr;
    const float temp = (p->d + D3DXVec3Dot(&normal, v1)) / dot;
    *out = *v1 - temp * direction;
    return out;
}

HRESULT D3DXComputeBoundingSphere(const D3DXVECTOR3* firstPosition, DWORD numVertices, DWORD stride,
                                  D3DXVECTOR3* center, float* radius)
{
    if (!firstPosition || !center || !radius)
        return D3DERR_INVALIDCALL;
    auto at = [&](DWORD i) {
        return reinterpret_cast<const D3DXVECTOR3*>(reinterpret_cast<const char*>(firstPosition) + std::size_t(stride) * i);
    };
    D3DXVECTOR3 sum(0.0f, 0.0f, 0.0f);
    *radius = 0.0f;
    for (DWORD i = 0; i < numVertices; ++i)
        sum += *at(i);
    *center = sum * (1.0f / static_cast<float>(numVertices));
    for (DWORD i = 0; i < numVertices; ++i)
    {
        const D3DXVECTOR3 d = *at(i) - *center;
        const float len = D3DXVec3Length(&d);
        if (len > *radius)
            *radius = len;
    }
    return D3D_OK;
}
