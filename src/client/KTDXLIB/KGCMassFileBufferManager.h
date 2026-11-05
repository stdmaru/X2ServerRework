// Reusable memory blocks for the data KGCMassFileManager returns (jintaeks, 2008-10-16: background loading), and for
// a few engine scratch buffers (KTDXAnimationController). docs/porting/client-ktdxlib.md#mass-files
//
// GetBuffer(size) hands out a block of at least `size` bytes from a bucket of power-of-two sizes; when the last
// KGCMassFileBufferPtr to it goes away the block goes back to its bucket for the next GetBuffer of that size class.
// Blocks over 4 MB (23 buckets: 1 B .. 4 MB) are not pooled. Thread-safe.
#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>

class KGCMassFileBuffer
{
public:
    explicit KGCMassFileBuffer(int iSize);
    ~KGCMassFileBuffer();
    KGCMassFileBuffer(const KGCMassFileBuffer&) = delete;
    KGCMassFileBuffer& operator=(const KGCMassFileBuffer&) = delete;

    char* GetBuffer() { return m_pData; }
    int GetMemorySize() const { return m_iMemorySize; }

private:
    char* m_pData = nullptr;
    int m_iMemorySize = 0;
};

// Shared by every holder of the block (the info returned by LoadDataFile, a sound device keeping its data, ...).
using KGCMassFileBufferPtr = std::shared_ptr<KGCMassFileBuffer>;

class KGCMassFileBufferManager
{
public:
    KGCMassFileBufferManager();
    ~KGCMassFileBufferManager();

    // A block of at least iSize bytes (iSize <= 0: an empty block).
    KGCMassFileBufferPtr GetBuffer(int iSize);

    // Frees the blocks waiting in the buckets; blocks still held are freed when released.
    void ReleaseAllBuffers();

    // Blocks waiting in the buckets (tests).
    std::size_t GetPooledCount() const;

private:
    static constexpr unsigned BUCKET_NUMS = 23;

    struct Pool
    {
        struct Bucket
        {
            mutable std::mutex m_mutex;
            std::vector<KGCMassFileBuffer*> m_free;
        };
        std::array<Bucket, BUCKET_NUMS> m_aBuckets;

        ~Pool();
        void Return(KGCMassFileBuffer* pBuffer, unsigned uBucket);
    };

    // The deleters hold it weakly: a block released after the manager is gone is simply freed.
    std::shared_ptr<Pool> m_pPool;
};
