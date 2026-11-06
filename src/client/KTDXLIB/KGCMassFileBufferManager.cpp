#include "KGCMassFileBufferManager.h"

#include <bit>

KGCMassFileBuffer::KGCMassFileBuffer(int iSize)
{
    if (iSize > 0)
    {
        m_pData = new char[static_cast<std::size_t>(iSize)];
        m_iMemorySize = iSize;
    }
}

KGCMassFileBuffer::~KGCMassFileBuffer()
{
    delete[] m_pData;
}

KGCMassFileBufferManager::Pool::~Pool()
{
    for (Bucket& bucket : m_aBuckets)
        for (KGCMassFileBuffer* pBuffer : bucket.m_free)
            delete pBuffer;
}

void KGCMassFileBufferManager::Pool::Return(KGCMassFileBuffer* pBuffer, unsigned uBucket)
{
    Bucket& bucket = m_aBuckets[uBucket];
    std::lock_guard lock(bucket.m_mutex);
    bucket.m_free.push_back(pBuffer);
}

KGCMassFileBufferManager::KGCMassFileBufferManager() : m_pPool(std::make_shared<Pool>())
{
}

KGCMassFileBufferManager::~KGCMassFileBufferManager() = default;

KGCMassFileBufferPtr KGCMassFileBufferManager::GetBuffer(int iSize)
{
    if (iSize <= 0)
        return std::make_shared<KGCMassFileBuffer>(0);

    // Bucket k holds blocks of 2^k bytes.
    const unsigned uBucket = static_cast<unsigned>(std::bit_width(static_cast<unsigned>(iSize) - 1u));
    if (uBucket >= BUCKET_NUMS)
        return std::make_shared<KGCMassFileBuffer>(iSize);

    KGCMassFileBuffer* pBuffer = nullptr;
    {
        Pool::Bucket& bucket = m_pPool->m_aBuckets[uBucket];
        std::lock_guard lock(bucket.m_mutex);
        if (!bucket.m_free.empty())
        {
            pBuffer = bucket.m_free.back();
            bucket.m_free.pop_back();
        }
    }
    if (pBuffer == nullptr)
        pBuffer = new KGCMassFileBuffer(1 << uBucket);

    std::weak_ptr<Pool> wpPool = m_pPool;
    return KGCMassFileBufferPtr(pBuffer, [wpPool, uBucket](KGCMassFileBuffer* p) {
        if (const std::shared_ptr<Pool> pPool = wpPool.lock())
            pPool->Return(p, uBucket);
        else
            delete p;
    });
}

void KGCMassFileBufferManager::ReleaseAllBuffers()
{
    for (Pool::Bucket& bucket : m_pPool->m_aBuckets)
    {
        std::vector<KGCMassFileBuffer*> free;
        {
            std::lock_guard lock(bucket.m_mutex);
            free.swap(bucket.m_free);
        }
        for (KGCMassFileBuffer* pBuffer : free)
            delete pBuffer;
    }
}

std::size_t KGCMassFileBufferManager::GetPooledCount() const
{
    std::size_t count = 0;
    for (const Pool::Bucket& bucket : m_pPool->m_aBuckets)
    {
        std::lock_guard lock(bucket.m_mutex);
        count += bucket.m_free.size();
    }
    return count;
}
