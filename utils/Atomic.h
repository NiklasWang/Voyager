#ifndef __UTILS_ATOMIC_H
#define __UTILS_ATOMIC_H

#include <stdint.h>
#include <sys/types.h>
#include <atomic>

#ifndef UTILS_ATOMIC_INLINE
#define UTILS_ATOMIC_INLINE static inline
#endif

/*
 * A handful of basic atomic operations.
 * THESE ARE HERE FOR LEGACY REASONS ONLY.  AVOID.
 *
 * PREFERRED ALTERNATIVES:
 * - Use C++/C/pthread locks/mutexes whenever there is not a
 *   convincing reason to do otherwise.  Note that very clever and
 *   complicated, but correct, lock-free code is often slower than
 *   using locks, especially where nontrivial data structures
 *   are involved.
 * - C11 stdatomic.h.
 * - Where supported, C++11 std::atomic<T> .
 *
 * PLEASE STOP READING HERE UNLESS YOU ARE TRYING TO UNDERSTAND
 * OR UPDATE OLD CODE.
 *
 * The "acquire" and "release" terms can be defined intuitively in terms
 * of the placement of memory barriers in a simple lock implementation:
 *   - wait until compare-and-swap(lock-is-free --> lock-is-held) succeeds
 *   - barrier
 *   - [do work]
 *   - barrier
 *   - store(lock-is-free)
 * In very crude terms, the initial (acquire) barrier prevents any of the
 * "work" from happening before the lock is held, and the later (release)
 * barrier ensures that all of the work happens before the lock is released.
 * (Think of cached writes, cache read-ahead, and instruction reordering
 * around the CAS and store instructions.)
 *
 * The barriers must apply to both the compiler and the CPU.  Note it is
 * legal for instructions that occur before an "acquire" barrier to be
 * moved down below it, and for instructions that occur after a "release"
 * barrier to be moved up above it.
 *
 * The ARM-driven implementation we use here is short on subtlety,
 * and actually requests a full barrier from the compiler and the CPU.
 * The only difference between acquire and release is in whether they
 * are issued before or after the atomic operation with which they
 * are associated.  To ease the transition to C/C++ atomic intrinsics,
 * you should not rely on this, and instead assume that only the minimal
 * acquire/release protection is provided.
 *
 * NOTE: all int32_t* values are expected to be aligned on 32-bit boundaries.
 * If they are not, atomicity is not guaranteed.
 */

/*
 * Basic arithmetic and bitwise operations.  These all provide a
 * barrier with "release" ordering, and return the previous value.
 *
 * These have the same characteristics (e.g. what happens on overflow)
 * as the equivalent non-atomic C operations.
 */
UTILS_ATOMIC_INLINE
int32_t utils_atomic_inc(volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
        /* Int32_t, if it exists, is the same as int_least32_t. */
    return atomic_fetch_add_explicit(a, 1, std::memory_order_release);
}

UTILS_ATOMIC_INLINE
int32_t utils_atomic_dec(volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    return atomic_fetch_sub_explicit(a, 1, std::memory_order_release);
}

UTILS_ATOMIC_INLINE
int32_t utils_atomic_add(int32_t value, volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    return atomic_fetch_add_explicit(a, value, std::memory_order_release);
}

UTILS_ATOMIC_INLINE
int32_t utils_atomic_and(int32_t value, volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    return atomic_fetch_and_explicit(a, value, std::memory_order_release);
}

UTILS_ATOMIC_INLINE
int32_t utils_atomic_or(int32_t value, volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    return atomic_fetch_or_explicit(a, value, std::memory_order_release);
}

/*
 * Perform an atomic load with "acquire" or "release" ordering.
 *
 * Note that the notion of a "release" ordering for a load does not
 * really fit into the C11 or C++11 memory model.  The extra ordering
 * is normally observable only by code using std::memory_order_relaxed
 * atomics, or data races.  In the rare cases in which such ordering
 * is called for, use std::memory_order_relaxed atomics and a leading
 * std::atomic_thread_fence (typically with std::memory_order_acquire,
 * not std::memory_order_release!) instead.  If you do not understand
 * this comment, you are in the vast majority, and should not be
 * using release loads or replacing them with anything other than
 * locks or default sequentially consistent atomics.
 */
UTILS_ATOMIC_INLINE
int32_t utils_atomic_acquire_load(volatile const int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    return atomic_load_explicit(a, std::memory_order_acquire);
}

UTILS_ATOMIC_INLINE
int32_t utils_atomic_release_load(volatile const int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    std::atomic_thread_fence(std::memory_order_seq_cst);
    /* Any reasonable clients of this interface would probably prefer   */
    /* something weaker.  But some remaining clients seem to be         */
    /* abusing this API in strange ways, e.g. by using it as a fence.   */
    /* Thus we are conservative until we can get rid of remaining       */
    /* clients (and this function).                                     */
    return atomic_load_explicit(a, std::memory_order_relaxed);
}

/*
 * Perform an atomic store with "acquire" or "release" ordering.
 *
 * Note that the notion of an "acquire" ordering for a store does not
 * really fit into the C11 or C++11 memory model.  The extra ordering
 * is normally observable only by code using std::memory_order_relaxed
 * atomics, or data races.  In the rare cases in which such ordering
 * is called for, use std::memory_order_relaxed atomics and a trailing
 * std::atomic_thread_fence (typically with std::memory_order_release,
 * not std::memory_order_acquire!) instead.
 */
UTILS_ATOMIC_INLINE
void utils_atomic_acquire_store(int32_t value, volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    atomic_store_explicit(a, value, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_seq_cst);
    /* Again overly conservative to accomodate weird clients.   */
}

UTILS_ATOMIC_INLINE
void utils_atomic_release_store(int32_t value, volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    atomic_store_explicit(a, value, std::memory_order_release);
}

/*
 * Compare-and-set operation with "acquire" or "release" ordering.
 *
 * This returns zero if the new value was successfully stored, which will
 * only happen when *addr == oldvalue.
 *
 * (The return value is inverted from implementations on other platforms,
 * but matches the ARM ldrex/strex result.)
 *
 * Implementations that use the release CAS in a loop may be less efficient
 * than possible, because we re-issue the memory barrier on each iteration.
 */
UTILS_ATOMIC_INLINE
int utils_atomic_acquire_cas(int32_t oldvalue, int32_t newvalue,
                           volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    return (int)(!atomic_compare_exchange_strong_explicit(
                                          a, &oldvalue, newvalue,
                                          std::memory_order_acquire,
                                          std::memory_order_acquire));
}

UTILS_ATOMIC_INLINE
int utils_atomic_release_cas(int32_t oldvalue, int32_t newvalue,
                               volatile int32_t* addr)
{
    volatile std::atomic_int_least32_t* a = (volatile std::atomic_int_least32_t*)addr;
    return (int)(!atomic_compare_exchange_strong_explicit(
                                          a, &oldvalue, newvalue,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));
}

/*
 * Fence primitives.
 */
UTILS_ATOMIC_INLINE
void utils_compiler_barrier(void)
{
    __asm__ __volatile__ ("" : : : "memory");
    /* Could probably also be:                          */
    /* atomic_signal_fence(std::memory_order_seq_cst);       */
}

UTILS_ATOMIC_INLINE
void utils_memory_barrier(void)
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

/*
 * Aliases for code using an older version of this header.  These are now
 * deprecated and should not be used.  The definitions will be removed
 * in a future release.
 */
#define utils_atomic_write utils_atomic_release_store
#define utils_atomic_cmpxchg utils_atomic_release_cas

#endif

