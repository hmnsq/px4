
#pragma once

#அந்தர்பாவ <inttypes.h>

#அந்தர்பாவ <nuttx/arch.h>

#அந்தர்பாவ <px4_platform_common/micro_hal.h>

#define ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#define ATOMIC_LEAVE px4_leave_critical_section(flags)

நாமாகாஶ cdev
{

using px4_file_operations_t = கட்டமைப்பு file_operations;
using file_t = கட்டமைப்பு file;

} // namespace cdev
