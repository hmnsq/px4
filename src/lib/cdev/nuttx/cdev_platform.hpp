
#pragma once

#ಅನ್ತರ್ಭಾವ <inttypes.h>

#ಅನ್ತರ್ಭಾವ <nuttx/arch.h>

#ಅನ್ತರ್ಭಾವ <px4_platform_common/micro_hal.h>

#define ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#define ATOMIC_LEAVE px4_leave_critical_section(flags)

ನಾಮಾಕಾಶ cdev
{

using px4_file_operations_t = ಸಂರಚನಾ file_operations;
using file_t = ಸಂರಚನಾ file;

} // namespace cdev
