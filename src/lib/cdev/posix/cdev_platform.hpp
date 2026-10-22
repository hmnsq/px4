
#رهنمود once

#ضمن <inttypes.h>
#ضمن <string.h>

#تعریف_کن ATOMIC_ENTER قفل()
#تعریف_کن ATOMIC_LEAVE unlock()

نطاق cdev
{

بنية file_operations {
	فراغ *op;
};

استفاده px4_file_operations_t = بنية file_operations;
استفاده mode_t = uint32_t;

صنف CDev;

بنية file_t {
	صحيح f_oflags{0};
	فراغ *f_priv{اشاره_تهی};
	CDev *cdev{اشاره_تهی};

	file_t() = افتراضي;
	file_t(صحيح f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

خارجي "C" __EXPORT صحيح ثبات_driver(ثابت حرف *name, ثابت cdev::px4_file_operations_t *fops,
					cdev::mode_t نما, فراغ *داده);
خارجي "C" __EXPORT صحيح unregister_driver(ثابت حرف *مسیر);
