#অন্তর্ভাব <px4_random.h>
#অন্তর্ভাব <tomcrypt.h>

কাঠামো ltc_hash_descriptor hash_descriptor[] = {
	{
		"sha256",
		0,
		32,
		64,

		/* OID */
		{ 2, 16, 840, 1, 101, 3, 4, 2, 1,  },
		9,

		&sha256_init,
		&sha256_process,
		&sha256_done,
		&sha256_test,
		ফাঁকা
	}
};

কাঠামো ltc_prng_descriptor prng_descriptor[] = {
	{
		"sprng", 0,
		&sprng_start,
		&sprng_add_entropy,
		&sprng_ready,
		&sprng_read,
		&sprng_done,
		&sprng_export,
		&sprng_import,
		&sprng_test
	}
};

unsigned long rng_get_bytes(unsigned অক্ষর *out,
			    unsigned long outlen,
			    শূন্য (*callback)(শূন্য))
{
	ফেরত px4_get_secure_random((uint8_t *)out, (size_t)outlen);
}

শূন্য libtomcrypt_init(শূন্য)
{
	ltc_mp = ltm_desc;
}
