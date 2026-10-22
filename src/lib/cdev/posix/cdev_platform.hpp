
#pragma once

#ضمن <inttypes.h>
#ضمن <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

نطاق cdev
{

بنية file_operations {
	فراغ *op;
};

using px4_file_operations_t = بنية file_operations;
using mode_t = uint32_t;

מחלקה CDev;

بنية file_t {
	صحيح f_oflags{0};
	فراغ *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = افتراضي;
	file_t(صحيح f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

خارجي "C" __EXPORT صحيح register_driver(ثابت حرف *name, ثابت cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, فراغ *data);
خارجي "C" __EXPORT صحيح unregister_driver(ثابت حرف *path);
