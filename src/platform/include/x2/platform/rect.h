// winuser.h's RECT helpers (SetRect, PtInRect, OffsetRect...), which the client's UI uses on its control rectangles.
// Native on Windows; elsewhere inline, with Win32's results (an empty rectangle has right <= left or bottom <= top).
#pragma once

#include "x2/platform/window_messages.h"

#if !defined(_WIN32)

#include <algorithm>

inline BOOL SetRect(RECT* rc, int left, int top, int right, int bottom)
{
    if (rc == nullptr) {
        return FALSE;
    }
    *rc = {left, top, right, bottom};
    return TRUE;
}

inline BOOL SetRectEmpty(RECT* rc) { return SetRect(rc, 0, 0, 0, 0); }

inline BOOL CopyRect(RECT* dst, const RECT* src)
{
    if (dst == nullptr || src == nullptr) {
        return FALSE;
    }
    *dst = *src;
    return TRUE;
}

inline BOOL IsRectEmpty(const RECT* rc) { return rc == nullptr || rc->right <= rc->left || rc->bottom <= rc->top; }

inline BOOL EqualRect(const RECT* a, const RECT* b)
{
    return a != nullptr && b != nullptr && a->left == b->left && a->top == b->top && a->right == b->right &&
           a->bottom == b->bottom;
}

// The right and bottom edges are outside.
inline BOOL PtInRect(const RECT* rc, POINT pt)
{
    return rc != nullptr && pt.x >= rc->left && pt.x < rc->right && pt.y >= rc->top && pt.y < rc->bottom;
}

inline BOOL OffsetRect(RECT* rc, int dx, int dy)
{
    if (rc == nullptr) {
        return FALSE;
    }
    rc->left += dx;
    rc->right += dx;
    rc->top += dy;
    rc->bottom += dy;
    return TRUE;
}

inline BOOL InflateRect(RECT* rc, int dx, int dy)
{
    if (rc == nullptr) {
        return FALSE;
    }
    rc->left -= dx;
    rc->right += dx;
    rc->top -= dy;
    rc->bottom += dy;
    return TRUE;
}

// FALSE and an empty dst when they do not overlap.
inline BOOL IntersectRect(RECT* dst, const RECT* a, const RECT* b)
{
    if (dst == nullptr || a == nullptr || b == nullptr) {
        return FALSE;
    }
    const RECT r = {std::max(a->left, b->left), std::max(a->top, b->top), std::min(a->right, b->right),
                    std::min(a->bottom, b->bottom)};
    if (IsRectEmpty(&r)) {
        SetRectEmpty(dst);
        return FALSE;
    }
    *dst = r;
    return TRUE;
}

// An empty rectangle does not count; FALSE and an empty dst when both are empty.
inline BOOL UnionRect(RECT* dst, const RECT* a, const RECT* b)
{
    if (dst == nullptr || a == nullptr || b == nullptr) {
        return FALSE;
    }
    const bool emptyA = IsRectEmpty(a);
    const bool emptyB = IsRectEmpty(b);
    if (emptyA && emptyB) {
        SetRectEmpty(dst);
        return FALSE;
    }
    if (emptyA || emptyB) {
        *dst = emptyA ? *b : *a;
        return TRUE;
    }
    *dst = {std::min(a->left, b->left), std::min(a->top, b->top), std::max(a->right, b->right),
            std::max(a->bottom, b->bottom)};
    return TRUE;
}

#endif // !_WIN32
