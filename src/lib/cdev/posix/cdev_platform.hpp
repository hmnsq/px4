
#هدايت once

#ضمن <inttypes.h>
#ضمن <string.h>

#وصف_ڪريو ATOMIC_ENTER تالو()
#وصف_ڪريو ATOMIC_LEAVE unlock()

نطاق cdev
{

بنية file_operations {
	فراغ *op;
};

استعمال px4_file_operations_t = بنية file_operations;
استعمال mode_t = uint32_t;

صنف CDev;

بنية file_t {
	صحيح f_oflags{0};
	فراغ *f_priv{خالي_اشارو};
	CDev *cdev{خالي_اشارو};

	file_t() = افتراضي;
	file_t(صحيح f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

خارجي "C" __EXPORT صحيح رجسٽر_driver(ثابت حرف *name, ثابت cdev::px4_file_operations_t *fops,
					cdev::mode_t گهڻائي, فراغ *معلومات);
خارجي "C" __EXPORT صحيح unregister_driver(ثابت حرف *رستو);
