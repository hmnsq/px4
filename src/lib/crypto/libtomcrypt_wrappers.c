#ଅନ୍ତର୍ଭାଵ <px4_random.h>
#ଅନ୍ତର୍ଭାଵ <tomcrypt.h>

ସଂରଚନା ltc_hash_descriptor hash_descriptor[] = {
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
		ଖାଲି
	}
};

ସଂରଚନା ltc_prng_descriptor prng_descriptor[] = {
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

unsigned long rng_get_bytes(unsigned ଅକ୍ଷର *out,
			    unsigned long outlen,
			    ଶୂନ୍ୟ (*callback)(ଶୂନ୍ୟ))
{
	ଫେରନ୍ତୁ px4_get_secure_random((uint8_t *)out, (size_t)outlen);
}

ଶୂନ୍ୟ libtomcrypt_init(ଶୂନ୍ୟ)
{
	ltc_mp = ltm_desc;
}
