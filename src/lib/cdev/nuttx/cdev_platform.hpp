
#pragma once

#अन्तर्भाव <inttypes.h>

#अन्तर्भाव <nuttx/arch.h>

#अन्तर्भाव <px4_platform_common/micro_hal.h>

#define ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#define ATOMIC_LEAVE px4_leave_critical_section(flags)

नामाकाश cdev
{

using px4_file_operations_t = संरचना file_operations;
using file_t = संरचना file;

} // namespace cdev
