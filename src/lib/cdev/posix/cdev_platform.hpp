
#pragma once

#include <inttypes.h>
#include <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

namespace cdev
{

sa_nracanaa file_operations {
	void *op;
};

using px4_file_operations_t = sa_nracanaa file_operations;
using mode_t = uint32_t;

class CDev;

sa_nracanaa file_t {
	පූර්ණ_සංඛ්‍යා f_oflags{0};
	void *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = default;
	file_t(පූර්ණ_සංඛ්‍යා f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

extern "C" __EXPORT පූර්ණ_සංඛ්‍යා register_driver(const var_na *name, const cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, void *data);
extern "C" __EXPORT පූර්ණ_සංඛ්‍යා unregister_driver(const var_na *path);
