#অন্তর্ভাৱ <px4_random.h>
#অন্তর্ভাৱ <tomcrypt.h>

সংরচনা ltc_hash_descriptor hash_descriptor[] = {
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
		রিক্ত
	}
};

সংরচনা ltc_prng_descriptor prng_descriptor[] = {
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

unsigned long rng_get_bytes(unsigned ৱর্ণ *out,
			    unsigned long outlen,
			    শূন্য (*callback)(শূন্য))
{
	ঘূৰাই px4_get_secure_random((uint8_t *)out, (size_t)outlen);
}

শূন্য libtomcrypt_init(শূন্য)
{
	ltc_mp = ltm_desc;
}
