#include <kernel/thread.hpp>
#include <kernel/vmm.h>
#include <kpp/cstdio.hpp>
#include <kpp/cstring.hpp>

static thread::ThreadId currentThreadId = 1;
static constexpr int maxThreads = 256; // Maximum number of threads

thread::Thread threadList[maxThreads];

[[ noreturn ]] static void threadEntryTrampoline()
{
    // currentThreadId should be the ID of the thread to jump to
    thread::ThreadId threadId = currentThreadId;
    thread::Thread& thread = threadList[threadId];
    if (thread.entryPoint) {
        thread.entryPoint(thread.entryPointArg);
    }
    thread::destroy_thread(threadId);
    thread::yield();
    __builtin_unreachable();
}

namespace thread {

void yield()
{
    ThreadId next = (currentThreadId + 1) % maxThreads;
    for (int i = 0; i < maxThreads; ++i) {
        if (threadList[next].state == Thread::State::ready && next != currentThreadId) {
            switch_to(next);
            return;
        }
        next = (next + 1) % maxThreads;
    }
}

void schedule() { yield(); }

ThreadId create_thread(void (*func)(void*), void* arg)
{
    ThreadId tid = 0;
    while (tid < maxThreads && threadList[tid].state != Thread::State::uninitialized) ++tid;
    if (tid >= maxThreads) return -1;

    Thread& t = threadList[tid];
    t.entryPoint    = func;
    t.entryPointArg = arg;

    constexpr size_t stackSize = 8192;
    t.stackBase = vmalloc(stackSize);
    if (!t.stackBase) return -1;
    // Eagerly map pages for the stack.
    // In the kernel, interrupts (including page faults) push to the kernel
    // stack, so we can easily double-fault if we lazily-map the stack pages.
    kpp::memset(t.stackBase, 0, stackSize);

    // --- Build a valid initial stack & context ---

    // 1) Compute top-of-stack and satisfy SysV alignment.
    //    At *function entry* (threadEntryTrampoline), RSP%16 must be 8.
    uintptr_t top = reinterpret_cast<uintptr_t>(t.stackBase) + stackSize;
    top &= ~uintptr_t(0xF);   // align to 16
    top -= 8;                 // pretend there is a return address
    *reinterpret_cast<uint64_t*>(top) = 0; // dummy return address

    t.context.rsp = top;
    t.context.rip = reinterpret_cast<uint64_t>(&threadEntryTrampoline);

    // 2) Seed callee-saved registers (not strictly required, but tidy).
    t.context.rbp = 0;
    t.context.rbx = 0;
    t.context.r12 = t.context.r13 = t.context.r14 = t.context.r15 = 0;

    // 3) Seed RFLAGS with sane bits: keep current flags, ensure IF=1.
    uint64_t rflags;
    asm volatile("pushfq; pop %0" : "=r"(rflags));
    rflags |= (1ull << 9);   // IF
    // Bit 1 (reserved) will already be 1 because we copied current RFLAGS.
    t.context.rflags = rflags;

    // If your restore() also loads CR3/FS/GS/FPU state, initialize those here too.

    t.state = Thread::State::ready;
#ifdef DEBUG_BUILD
    DEBUG("Created thread %x with entry point %p\n", tid, func);
#endif
    return tid;
}

ThreadError destroy_thread(ThreadId threadId)
{
    if (threadId >= maxThreads) {
        return ThreadError::invalidThreadId;
    }
    if (threadList[threadId].state == Thread::State::uninitialized) {
        return ThreadError::doubleDestroy;
    }
    threadList[threadId].state = Thread::State::uninitialized;
    threadList[threadId].entryPoint = nullptr;
    return ThreadError::success;
}

ThreadError switch_to(ThreadId nextThreadId)
{
    if (nextThreadId >= maxThreads || threadList[nextThreadId].state == Thread::State::uninitialized) {
        return ThreadError::invalidThreadId;
    }
#ifdef DEBUG_BUILD
    DEBUG("Switching from thread %x to thread %x\n", currentThreadId, nextThreadId);
#endif
    Thread& currentThread = threadList[currentThreadId];
    Thread& nextThread = threadList[nextThreadId];
    if (save_cpu_context(&currentThread.context) == 0) {
        currentThread.state = Thread::State::ready;
        nextThread.state = Thread::State::running;
        currentThreadId = nextThreadId;
        restore_cpu_context(&nextThread.context);
        __builtin_unreachable();
    }
    // We are here after returning to this threads context
    return ThreadError::success;
}

ThreadId get_current_thread_id()
{
    return currentThreadId;
}

} // namespace thread
