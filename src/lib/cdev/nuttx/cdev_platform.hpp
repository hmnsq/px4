
#pragma once

#شامل <inttypes.h>

#شامل <nuttx/arch.h>

#شامل <px4_platform_common/micro_hal.h>

#define ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#define ATOMIC_LEAVE px4_leave_critical_section(flags)

فضاینام cdev
{

using px4_file_operations_t = ساختار file_operations;
using file_t = ساختار file;

} // namespace cdev
