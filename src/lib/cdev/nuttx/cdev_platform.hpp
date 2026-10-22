
#ہدایت once

#شامل <inttypes.h>

#شامل <nuttx/arch.h>

#شامل <px4_platform_common/micro_hal.h>

#تعریف_کرو ATOMIC_ENTER irqstate_t flags = px4_enter_critical_section()
#تعریف_کرو ATOMIC_LEAVE px4_leave_critical_section(flags)

نامخلا cdev
{

استعمال px4_file_operations_t = ساخت file_operations;
استعمال file_t = ساخت file;

} // namespace cdev
