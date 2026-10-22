#अन्तर्भाव <px4_random.h>
#अन्तर्भाव <tomcrypt.h>

संरचना ltc_hash_descriptor hash_descriptor[] = {
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
		रिक्त
	}
};

संरचना ltc_prng_descriptor prng_descriptor[] = {
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

unsigned long rng_get_bytes(unsigned वर्ण *out,
			    unsigned long outlen,
			    शून्य (*callback)(शून्य))
{
	प्रत्यागम px4_get_secure_random((uint8_t *)out, (size_t)outlen);
}

शून्य libtomcrypt_init(शून्य)
{
	ltc_mp = ltm_desc;
}
