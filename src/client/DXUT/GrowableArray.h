#pragma once

// DXUT's CGrowableArray (DXUTmisc.h), which KTDXLIB's list box, combo box and IME edit box keep, on std::vector. Same
// methods and results: E_INVALIDARG for an index out of range, -1 from IndexOf when not found.

#include "x2/platform/types.h"

#include <cassert>
#include <vector>

template<typename TYPE>
class CGrowableArray {
public:
    const TYPE& operator[](int nIndex) const { return GetAt(nIndex); }
    TYPE& operator[](int nIndex) { return GetAt(nIndex); }

    // Grows with default-constructed elements, or shrinks.
    HRESULT SetSize(int nNewSize)
    {
        if (nNewSize < 0)
            return E_INVALIDARG;
        m_data.resize(static_cast<std::size_t>(nNewSize));
        return S_OK;
    }
    HRESULT Add(const TYPE& value)
    {
        m_data.push_back(value);
        return S_OK;
    }
    HRESULT Insert(int nIndex, const TYPE& value)
    {
        if (nIndex < 0 || nIndex > GetSize())
            return E_INVALIDARG;
        m_data.insert(m_data.begin() + nIndex, value);
        return S_OK;
    }
    HRESULT SetAt(int nIndex, const TYPE& value)
    {
        if (nIndex < 0 || nIndex >= GetSize())
            return E_INVALIDARG;
        m_data[static_cast<std::size_t>(nIndex)] = value;
        return S_OK;
    }
    TYPE& GetAt(int nIndex)
    {
        assert(nIndex >= 0 && nIndex < GetSize());
        return m_data[static_cast<std::size_t>(nIndex)];
    }
    const TYPE& GetAt(int nIndex) const
    {
        assert(nIndex >= 0 && nIndex < GetSize());
        return m_data[static_cast<std::size_t>(nIndex)];
    }
    int GetSize() const { return static_cast<int>(m_data.size()); }
    TYPE* GetData() { return m_data.empty() ? nullptr : m_data.data(); }
    bool Contains(const TYPE& value) { return IndexOf(value) != -1; }

    int IndexOf(const TYPE& value) { return GetSize() > 0 ? IndexOf(value, 0, GetSize()) : -1; }
    int IndexOf(const TYPE& value, int iStart) { return IndexOf(value, iStart, GetSize() - iStart); }
    int IndexOf(const TYPE& value, int nIndex, int nNumElements)
    {
        if (nIndex < 0 || nNumElements < 0 || nIndex + nNumElements > GetSize())
            return -1;
        for (int i = nIndex; i < nIndex + nNumElements; ++i)
            if (value == m_data[static_cast<std::size_t>(i)])
                return i;
        return -1;
    }

    int LastIndexOf(const TYPE& value) { return GetSize() > 0 ? LastIndexOf(value, GetSize() - 1, GetSize()) : -1; }
    int LastIndexOf(const TYPE& value, int nIndex) { return LastIndexOf(value, nIndex, nIndex + 1); }
    int LastIndexOf(const TYPE& value, int nIndex, int nNumElements)
    {
        if (nIndex < 0 || nNumElements < 0 || nIndex >= GetSize() || nIndex - nNumElements + 1 < 0)
            return -1;
        for (int i = nIndex; i > nIndex - nNumElements; --i)
            if (value == m_data[static_cast<std::size_t>(i)])
                return i;
        return -1;
    }

    HRESULT Remove(int nIndex)
    {
        if (nIndex < 0 || nIndex >= GetSize())
            return E_INVALIDARG;
        m_data.erase(m_data.begin() + nIndex);
        return S_OK;
    }
    void RemoveAll() { m_data.clear(); }

private:
    std::vector<TYPE> m_data;
};
