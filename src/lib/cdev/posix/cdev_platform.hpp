
#pragma once

#அந்தர்பாவ <inttypes.h>
#அந்தர்பாவ <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

நாமாகாஶ cdev
{

கட்டமைப்பு file_operations {
	வெறுமை *op;
};

using px4_file_operations_t = கட்டமைப்பு file_operations;
using mode_t = uint32_t;

வகுப்பு CDev;

கட்டமைப்பு file_t {
	முழு f_oflags{0};
	வெறுமை *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = இயல்பு;
	file_t(முழு f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

வெளி "C" __EXPORT முழு register_driver(மாறிலி எழுத்து *name, மாறிலி cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, வெறுமை *data);
வெளி "C" __EXPORT முழு unregister_driver(மாறிலி எழுத்து *path);
