
#ہدایت once

#ضمن <inttypes.h>
#ضمن <string.h>

#تعریف_کرو ATOMIC_ENTER جندرا()
#تعریف_کرو ATOMIC_LEAVE unlock()

نطاق cdev
{

بنية file_operations {
	فراغ *op;
};

ورتوں px4_file_operations_t = بنية file_operations;
ورتوں mode_t = uint32_t;

صنف CDev;

بنية file_t {
	صحيح f_oflags{0};
	فراغ *f_priv{خالی_سوچک};
	CDev *cdev{خالی_سوچک};

	file_t() = افتراضي;
	file_t(صحيح f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

خارجي "C" __EXPORT صحيح رجسٹر_driver(ثابت حرف *name, ثابت cdev::px4_file_operations_t *fops,
					cdev::mode_t منوال, فراغ *انکڑے);
خارجي "C" __EXPORT صحيح unregister_driver(ثابت حرف *رستہ);
