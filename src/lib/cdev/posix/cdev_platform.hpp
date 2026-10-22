
#pragma once

#अन्तर्भाव <inttypes.h>
#अन्तर्भाव <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

नामाकाश cdev
{

संरचना file_operations {
	शून्य *op;
};

using px4_file_operations_t = संरचना file_operations;
using mode_t = uint32_t;

वग्ग CDev;

संरचना file_t {
	पूर्णाङ्क f_oflags{0};
	शून्य *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = स्वभाव;
	file_t(पूर्णाङ्क f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

बाह्य "C" __EXPORT पूर्णाङ्क register_driver(ध्रुव वर्ण *name, ध्रुव cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, शून्य *data);
बाह्य "C" __EXPORT पूर्णाङ्क unregister_driver(ध्रुव वर्ण *path);
