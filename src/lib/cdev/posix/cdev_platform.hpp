
#निर्देशः once

#अन्तर्भाव <inttypes.h>
#अन्तर्भाव <string.h>

#परिभाषय ATOMIC_ENTER तालकम्()
#परिभाषय ATOMIC_LEAVE unlock()

नामाकाश cdev
{

संरचना file_operations {
	शून्य *op;
};

उपयुज्य px4_file_operations_t = संरचना file_operations;
उपयुज्य mode_t = uint32_t;

वर्ग CDev;

संरचना file_t {
	पूर्णाङ्क f_oflags{0};
	शून्य *f_priv{शून्यसूचकः};
	CDev *cdev{शून्यसूचकः};

	file_t() = स्वभाव;
	file_t(पूर्णाङ्क f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

बाह्य "C" __EXPORT पूर्णाङ्क पञ्जिकाकोशः_driver(ध्रुव वर्ण *name, ध्रुव cdev::px4_file_operations_t *fops,
					cdev::mode_t बहुलकम्, शून्य *दत्त);
बाह्य "C" __EXPORT पूर्णाङ्क unregister_driver(ध्रुव वर्ण *मार्गः);
