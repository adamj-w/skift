#pragma once

#include <libruntime/Macros.h>
#include <libruntime/RefPtr.h>
#include <libruntime/Types.h>

#include "system/memory/Region.h"
#include "system/tasking/Thread.h"

namespace arch
{

void stop() __noreturn;

void idle() __noreturn;

void halt();

size_t get_page_size();

system::memory::Region get_kernel_region();

libruntime::RefPtr<system::tasking::Thread> create_thread(
    system::tasking::Process &process,
    system::tasking::ThreadPromotion promotion,
    system::tasking::ThreadEntry entry);

} // namespace arch
