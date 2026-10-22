
#هدايت once

#ضمن <inttypes.h>

#ضمن <nuttx/arch.h>

#ضمن <px4_platform_common/micro_hal.h>

#وصف_ڪريو ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#وصف_ڪريو ATOMIC_LEAVE px4_leave_critical_section(flags)

نطاق cdev
{

استعمال px4_file_operations_t = بنية file_operations;
استعمال file_t = بنية file;

} // namespace cdev
