#شامل <px4_random.h>
#شامل <tomcrypt.h>

ساختار ltc_hash_descriptor hash_descriptor[] = {
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
		پوچ
	}
};

ساختار ltc_prng_descriptor prng_descriptor[] = {
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

unsigned long rng_get_bytes(unsigned نویسه *out,
			    unsigned long outlen,
			    تهی (*callback)(تهی))
{
	بازگشت px4_get_secure_random((uint8_t *)out, (size_t)outlen);
}

تهی libtomcrypt_init(تهی)
{
	ltc_mp = ltm_desc;
}
