
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATOMIC_ENTER ਤਾਲਾ()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATOMIC_LEAVE unlock()

ਨਾਮਾਕਾਸ਼ cdev
{

ਬਣਤਰ file_operations {
	ਖਾਲੀ *op;
};

ਵਰਤੋਂ px4_file_operations_t = ਬਣਤਰ file_operations;
ਵਰਤੋਂ mode_t = uint32_t;

ਵਰਗ CDev;

ਬਣਤਰ file_t {
	ਪੂਰਨ f_oflags{0};
	ਖਾਲੀ *f_priv{ਖਾਲੀ_ਸੂਚਕ};
	CDev *cdev{ਖਾਲੀ_ਸੂਚਕ};

	file_t() = ਮੂਲ;
	file_t(ਪੂਰਨ f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

ਬਾਹਰੀ "C" __EXPORT ਪੂਰਨ ਰਜਿਸਟਰ_driver(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ cdev::px4_file_operations_t *fops,
					cdev::mode_t ਬਹੁਲਕ, ਖਾਲੀ *ਅੰਕੜੇ);
ਬਾਹਰੀ "C" __EXPORT ਪੂਰਨ unregister_driver(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);
