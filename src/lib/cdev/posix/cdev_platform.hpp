
#ہدایت once

#شامل <inttypes.h>
#شامل <string.h>

#تعریف_کرو ATOMIC_ENTER تالا()
#تعریف_کرو ATOMIC_LEAVE unlock()

نامخلا cdev
{

ساخت file_operations {
	خالی *op;
};

استعمال px4_file_operations_t = ساخت file_operations;
استعمال mode_t = uint32_t;

جماعت CDev;

ساخت file_t {
	صحیح f_oflags{0};
	خالی *f_priv{خالی_اشاریہ};
	CDev *cdev{خالی_اشاریہ};

	file_t() = طےشدہ;
	file_t(صحیح f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

خارجی "C" __EXPORT صحیح رجسٹر_driver(ثابت حرف *name, ثابت cdev::px4_file_operations_t *fops,
					cdev::mode_t منوال, خالی *ڈیٹا);
خارجی "C" __EXPORT صحیح unregister_driver(ثابت حرف *راستہ);
