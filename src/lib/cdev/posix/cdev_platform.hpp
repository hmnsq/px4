
#لارښوونه once

#ضمن <inttypes.h>
#ضمن <string.h>

#تعریف_کړه ATOMIC_ENTER قلف()
#تعریف_کړه ATOMIC_LEAVE unlock()

نطاق cdev
{

بنية file_operations {
	فراغ *op;
};

کارول px4_file_operations_t = بنية file_operations;
کارول mode_t = uint32_t;

صنف CDev;

بنية file_t {
	صحيح f_oflags{0};
	فراغ *f_priv{تش_اشاره};
	CDev *cdev{تش_اشاره};

	file_t() = افتراضي;
	file_t(صحيح f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

خارجي "C" __EXPORT صحيح ثبتوونکی_driver(ثابت حرف *name, ثابت cdev::px4_file_operations_t *fops,
					cdev::mode_t ډېرپېښ, فراغ *معلومات);
خارجي "C" __EXPORT صحيح unregister_driver(ثابت حرف *لار);
