
#pragma once

#અન્તર્ભાવ <inttypes.h>

#અન્તર્ભાવ <nuttx/arch.h>

#અન્તર્ભાવ <px4_platform_common/micro_hal.h>

#define ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#define ATOMIC_LEAVE px4_leave_critical_section(flags)

નામાકાશ cdev
{

using px4_file_operations_t = સંરચના file_operations;
using file_t = સંરચના file;

} // namespace cdev
