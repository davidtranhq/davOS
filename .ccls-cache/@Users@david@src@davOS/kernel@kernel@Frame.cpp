#include <kernel/Frame.h>
#include <kernel/frame_allocator.h>

Frame::Frame()
    : address_ {allocate_frame()}
{}

Frame::~Frame()
{
    deallocate_frame(address_);
}

void *Frame::address() const noexcept
{
    return address_;
}

Frame::operator void *() const noexcept
{
    return address_;
}

Frame::operator uintptr_t() const
{
    return reinterpret_cast<uintptr_t>(address_);
}