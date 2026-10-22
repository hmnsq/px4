
#رهنمود once

#ضمن <inttypes.h>

#ضمن <nuttx/arch.h>

#ضمن <px4_platform_common/micro_hal.h>

#تعریف_کن ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#تعریف_کن ATOMIC_LEAVE px4_leave_critical_section(flags)

نطاق cdev
{

استفاده px4_file_operations_t = بنية file_operations;
استفاده file_t = بنية file;

} // namespace cdev
