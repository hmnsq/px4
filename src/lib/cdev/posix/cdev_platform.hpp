
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <inttypes.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ATOMIC_ENTER ಬೀಗ()
#ವ್ಯಾಖ್ಯಾನಿಸು ATOMIC_LEAVE unlock()

ನಾಮಾಕಾಶ cdev
{

ರಚನೆ file_operations {
	ಶೂನ್ಯ *op;
};

ಬಳಸಿ px4_file_operations_t = ರಚನೆ file_operations;
ಬಳಸಿ mode_t = uint32_t;

ವರ್ಗ CDev;

ರಚನೆ file_t {
	ಪೂರ್ಣಾಂಕ f_oflags{0};
	ಶೂನ್ಯ *f_priv{ಶೂನ್ಯಸೂಚಕ};
	CDev *cdev{ಶೂನ್ಯಸೂಚಕ};

	file_t() = ಪೂರ್ವನಿಯೋಜಿತ;
	file_t(ಪೂರ್ಣಾಂಕ f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

ಬಾಹ್ಯ "C" __EXPORT ಪೂರ್ಣಾಂಕ ರಿಜಿಸ್ಟರ್_driver(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ cdev::px4_file_operations_t *fops,
					cdev::mode_t ಬಹುಲಕ, ಶೂನ್ಯ *ದತ್ತಾಂಶ);
ಬಾಹ್ಯ "C" __EXPORT ಪೂರ್ಣಾಂಕ unregister_driver(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);
