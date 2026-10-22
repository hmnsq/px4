
#pragma once

#অন্তর্ভাৱ <inttypes.h>

#অন্তর্ভাৱ <nuttx/arch.h>

#অন্তর্ভাৱ <px4_platform_common/micro_hal.h>

#define ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#define ATOMIC_LEAVE px4_leave_critical_section(flags)

নামাকাশ cdev
{

using px4_file_operations_t = সংরচনা file_operations;
using file_t = সংরচনা file;

} // namespace cdev
