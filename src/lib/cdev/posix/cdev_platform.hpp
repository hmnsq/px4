
#pragma once

#شامل <inttypes.h>
#شامل <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

فضاینام cdev
{

ساختار file_operations {
	تهی *op;
};

using px4_file_operations_t = ساختار file_operations;
using mode_t = uint32_t;

رده CDev;

ساختار file_t {
	صحیح f_oflags{0};
	تهی *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = پیشفرض;
	file_t(صحیح f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

بیرونی "C" __EXPORT صحیح register_driver(ثابت نویسه *name, ثابت cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, تهی *data);
بیرونی "C" __EXPORT صحیح unregister_driver(ثابت نویسه *path);
