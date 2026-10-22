
#pragma once

#ಅನ್ತರ್ಭಾವ <inttypes.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

ನಾಮಾಕಾಶ cdev
{

ಸಂರಚನಾ file_operations {
	ಶೂನ್ಯ *op;
};

using px4_file_operations_t = ಸಂರಚನಾ file_operations;
using mode_t = uint32_t;

ವರ್ಗ CDev;

ಸಂರಚನಾ file_t {
	ಪೂರ್ಣಾಙ್ಕ f_oflags{0};
	ಶೂನ್ಯ *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = ಸ್ವಭಾವ;
	file_t(ಪೂರ್ಣಾಙ್ಕ f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

ಬಾಹ್ಯ "C" __EXPORT ಪೂರ್ಣಾಙ್ಕ register_driver(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, ಶೂನ್ಯ *data);
ಬಾಹ್ಯ "C" __EXPORT ಪೂರ್ಣಾಙ್ಕ unregister_driver(ಧ್ರುವ ವರ್ಣ *path);
