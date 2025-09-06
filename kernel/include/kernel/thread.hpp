#pragma once

#include <cstdint>
#include <kernel/CpuContext.hpp>
#include <kernel/IDTStructure.h>

namespace thread {
struct Thread {
    enum class State {
        uninitialized,
        ready,
        running,
        blocked,
        terminated,
    };
    CpuContext context;
    void (*entryPoint)(void* arg) = nullptr;
    void* entryPointArg = nullptr;
    State state = State::uninitialized;
    // Pointer to the base of the thread-local stack allocated when the thread is created.
    // The RSP register in the context will point to the top of this stack.
    void* stackBase = nullptr;
};
using ThreadId = int64_t;
enum class ThreadError {
    success = 0,
    maxThreadsReached,
    invalidThreadId,
    doubleDestroy,
    stackAllocationFailed
};

/**
 * @brief Switch to the next thread in the ready state.
 * If no threads are ready, the current thread will continue running.
 */
void yield();

/**
 * @brief Create a new thread with the specified entry point function and argument.
 * @param func The function to run in the new thread.
 * @param arg The argument to pass to the function (default is nullptr).
 * @return An Expected containing the ThreadId of the created thread or an error if creation failed.
 */
ThreadId create_thread(void (*func)(void*), void* arg = nullptr);

/**
 * @brief Destroy a thread by its ThreadId. Cleans up resources associated with the thread.
 * This function is called automatically when a thread finishes execution.
 * @param threadId The ID of the thread to destroy.
 * @return A ThreadError indicating the result of the operation.
 */
ThreadError destroy_thread(ThreadId);


/**
 * @brief Switch to the specified thread by its ThreadId.
 * Saves the current thread's context and restores the context of the specified thread.
 * If the operation is successful, the CPU immediately jumps to the new thread and
 * does not return from this function.
 *
 * @param nextThreadId The ID of the thread to switch to.
 * @return A ThreadError indicating the result of the operation if there was failiure.
 */
ThreadError switch_to(ThreadId);

/**
 * @brief Get the ThreadId of the currently running thread.
 */
ThreadId get_current_thread_id();

} // namespace thread
