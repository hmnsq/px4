
#pragma once

#അന്തര്ഭാവ <inttypes.h>

#അന്തര്ഭാവ <nuttx/arch.h>

#അന്തര്ഭാവ <px4_platform_common/micro_hal.h>

#define ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#define ATOMIC_LEAVE px4_leave_critical_section(flags)

നാമാകാശ cdev
{

using px4_file_operations_t = ഘടന file_operations;
using file_t = ഘടന file;

} // namespace cdev
