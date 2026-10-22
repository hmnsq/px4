
#pragma once

#ଅନ୍ତର୍ଭାଵ <inttypes.h>

#ଅନ୍ତର୍ଭାଵ <nuttx/arch.h>

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/micro_hal.h>

#define ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#define ATOMIC_LEAVE px4_leave_critical_section(flags)

ନାମାକାଶ cdev
{

using px4_file_operations_t = ସଂରଚନା file_operations;
using file_t = ସଂରଚନା file;

} // namespace cdev
