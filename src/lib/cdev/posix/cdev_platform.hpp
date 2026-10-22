
#pragma once

#অন্তর্ভাব <inttypes.h>
#অন্তর্ভাব <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

নামাকাশ cdev
{

সংরচনা file_operations {
	শূন্য *op;
};

using px4_file_operations_t = সংরচনা file_operations;
using mode_t = uint32_t;

বর্গ CDev;

সংরচনা file_t {
	পূর্ণাঙ্ক f_oflags{0};
	শূন্য *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = স্বভাব;
	file_t(পূর্ণাঙ্ক f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

বাহ্য "C" __EXPORT পূর্ণাঙ্ক register_driver(ধ্রুব বর্ণ *name, ধ্রুব cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, শূন্য *data);
বাহ্য "C" __EXPORT পূর্ণাঙ্ক unregister_driver(ধ্রুব বর্ণ *path);
