#ifndef DAVOS_KERNEL_FRAME_H_INCLUDED
#define DAVOS_KERNEL_FRAME_H_INCLUDED

#include <stdint.h>

/**
 * @brief A RAII wrapper class for a pointer to a physical frame. 
 */
class Frame
{
public:
    Frame();
    ~Frame();

    void *address() const noexcept;

    operator void *() const noexcept;
    
    operator uintptr_t() const;

private:
    void *address_;
};

#endif