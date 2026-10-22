
#pragma once

#অন্তর্ভাব <inttypes.h>
#অন্তর্ভাব <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

নামাকাশ cdev
{

কাঠামো file_operations {
	শূন্য *op;
};

using px4_file_operations_t = কাঠামো file_operations;
using mode_t = uint32_t;

শ্রেণী CDev;

কাঠামো file_t {
	পূর্ণসংখ্যা f_oflags{0};
	শূন্য *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = পূর্বনির্ধারিত;
	file_t(পূর্ণসংখ্যা f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

বাহ্যিক "C" __EXPORT পূর্ণসংখ্যা register_driver(ধ্রুবক অক্ষর *name, ধ্রুবক cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, শূন্য *তথ্য);
বাহ্যিক "C" __EXPORT পূর্ণসংখ্যা unregister_driver(ধ্রুবক অক্ষর *path);
