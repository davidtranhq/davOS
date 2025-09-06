#pragma once

#include <cstdint>

#ifndef ARCH_x86_64
#error "This header is intended for the x86_64 architecture only."
#endif

struct CpuContext {
    // We only need to save the callee-saved registers. On x86_64, System V ABI,
    // this is r15, r14, r13, r12, rbx, rbp.
    //
    // The other registers are caller-saved, i.e. expected to be clobbered.
    uint64_t r15, r14, r13, r12, rbx, rbp;

    uint64_t rsp;
    uint64_t rip;
    uint64_t rflags;
};


/**
 * @brief Saves the full x86_64 CPU context into the provided buffer.
 *
 * @param
 *   Pointer to a pre-allocated buffer, where the CPU context will be saved.
 *
 * @returns 0 on the first call, 1 when context is restored into this function.
 *
 * The instruction pointer that is saved points inside this function, so this
 * function will effectively return twice (once on the first call, another
 * when the thread context is restored).
 */
extern "C"
int save_cpu_context(CpuContext*);

/**
 * @brief Restores a previously saved x86_64 CPU context from the provided buffer.
 *
 * @param
 *   Pointer to a buffer previously initialized by save_cpu_context().
 *
 * Upon calling this function, control resumes at the instruction pointer
 * saved in the passed-in context.
 */
extern "C"
[[ noreturn ]]
int restore_cpu_context(CpuContext*);
