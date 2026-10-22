
#pragma once

#અન્તર્ભાવ <inttypes.h>
#અન્તર્ભાવ <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

નામાકાશ cdev
{

સંરચના file_operations {
	શૂન્ય *op;
};

using px4_file_operations_t = સંરચના file_operations;
using mode_t = uint32_t;

વર્ગ CDev;

સંરચના file_t {
	પૂર્ણાંક f_oflags{0};
	શૂન્ય *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = મૂળભૂત;
	file_t(પૂર્ણાંક f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

બાહ્ય "C" __EXPORT પૂર્ણાંક register_driver(અચલ અક્ષર *name, અચલ cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, શૂન્ય *data);
બાહ્ય "C" __EXPORT પૂર્ણાંક unregister_driver(અચલ અક્ષર *path);
