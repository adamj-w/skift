#include <libsystem/Logger.h>
#include <libsystem/Stdio.h>

#include "arch/Arch.h"
#include "arch/x86/TerminalStream.h"
#include "arch/x86/boot/Multiboot.h"
#include "arch/x86/device/CGAScreen.h"
#include "arch/x86/device/SerialStream.h"
#include "arch/x86/interupts/Interupts.h"
#include "arch/x86/segmentation/Segmentation.h"

#include "system/memory/Memory.h"
#include "system/sheduling/Sheduling.h"
#include "system/tasking/Process.h"

using namespace x86;
using namespace system;
using namespace libruntime;
using namespace libsystem;

RefPtr<Stream> libsystem::stdin;
RefPtr<Stream> libsystem::stdout;
RefPtr<Stream> libsystem::stderr;
RefPtr<Stream> libsystem::stdlog;

void taskA()
{
    do
    {
        libsystem::stderr->write("A", 1);
        arch::halt();
    } while (1);
}

void taskB()
{
    do
    {
        libsystem::stderr->write("B", 1);
        arch::halt();
    } while (1);
}

extern "C" void arch_main(u32 multiboot_magic, multiboot_info_t *multiboot_info)
{
    auto serial = SerialStream(SerialPort::COM1);
    auto terminal = TerminalStream(CGAScreen((void *)0xB8000));

    // We don't went the ref count to try to delete these.
    serial.make_orphan();
    terminal.make_orphan();

    //libsystem::stdin = nullptr;
    libsystem::stderr = adopt(serial);
    libsystem::stdout = adopt(terminal);
    libsystem::stdlog = adopt(serial);

    logger_info("Booting...");
    logger_info("hjert kernel ({})", __BUILD_TARGET__);
    logger_info("Kernel build on \"{}\"", __BUILD_UNAME__);

    auto multiboot = Multiboot(multiboot_magic, multiboot_info);

    if (!multiboot.is_valid())
    {
        logger_info("Invalid bootloader, how did you do that !?");
    }
    else
    {
        logger_info("Bootloader is {}", multiboot.bootloader());
    }

    multiboot.with_memory_map([](auto entry) {
        if (entry.is_available())
        {
            logger_info("Marking {} as free usable memory by the kernel...", entry);
            memory::free_region(entry.region());
        }
        else if (entry.is_bad())
        {
            logger_warn("Badram at {}, skipping...", entry.region());
        }
        else
        {
            logger_info("Skipping {}...", entry);
        }

        return Iteration::CONTINUE;
    });

    x86::segmentation_initialize();
    sheduling::initialize();
    x86::interupts_initialise();

    auto kernel_process = new tasking::Process(nullptr);

    auto kernel_thread = kernel_process->create_thread(nullptr);
    auto taskA_thread = kernel_process->create_thread(reinterpret_cast<tasking::ThreadEntry>(taskA));
    auto taskB_thread = kernel_process->create_thread(reinterpret_cast<tasking::ThreadEntry>(taskB));

    kernel_thread->start();
    taskA_thread->start();
    taskB_thread->start();

    logger_info("Hi {}!", kernel_thread.necked());
    logger_info("Hi {}!", taskA_thread.necked());
    logger_info("Hi {}!", taskB_thread.necked());

    print("hjert kernel v0.0.1\n");
    print("--------------------------------------------------------------------------------\n");
    print("System halted!\n");

    do
    {
        libsystem::stderr->write("K", 1);

        arch::halt();
    } while (true);

    asm volatile("int $0x80");

    arch::idle();
}