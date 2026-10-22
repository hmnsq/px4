
#pragma once

#അന്തര്ഭാവ <inttypes.h>
#അന്തര്ഭാവ <string.h>

#define ATOMIC_ENTER lock()
#define ATOMIC_LEAVE unlock()

നാമാകാശ cdev
{

ഘടന file_operations {
	ശൂന്യം *op;
};

using px4_file_operations_t = ഘടന file_operations;
using mode_t = uint32_t;

വർഗ്ഗം CDev;

ഘടന file_t {
	പൂർണ്ണസംഖ്യ f_oflags{0};
	ശൂന്യം *f_priv{nullptr};
	CDev *cdev{nullptr};

	file_t() = സ്ഥിരസ്ഥിതി;
	file_t(പൂർണ്ണസംഖ്യ f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

ബാഹ്യ "C" __EXPORT പൂർണ്ണസംഖ്യ register_driver(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം cdev::px4_file_operations_t *fops,
					cdev::mode_t mode, ശൂന്യം *data);
ബാഹ്യ "C" __EXPORT പൂർണ്ണസംഖ്യ unregister_driver(സ്ഥിരം അക്ഷരം *path);
