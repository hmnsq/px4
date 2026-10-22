
#pragma once

#ଅନ୍ତର୍ଭାଵ <inttypes.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

ନାମାକାଶ cdev
{

ସଂରଚନା file_operations {
	ଶୂନ୍ୟ *op;
};

using px4_file_operations_t = ସଂରଚନା file_operations;
using mode_t = uint32_t;

ଶ୍ରେଣୀ CDev;

ସଂରଚନା file_t {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା f_oflags{0};
	ଶୂନ୍ୟ *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	file_t(ପୂର୍ଣ୍ଣସଂଖ୍ୟା f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

ବାହ୍ୟ "C" __EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା register_driver(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, ଶୂନ୍ୟ *data);
ବାହ୍ୟ "C" __EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା unregister_driver(ଅଚଳ ଅକ୍ଷର *path);
