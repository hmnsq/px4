#അന്തര്ഭാവ <px4_random.h>
#അന്തര്ഭാവ <tomcrypt.h>

ഘടന ltc_hash_descriptor hash_descriptor[] = {
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
		ശൂന്യ
	}
};

ഘടന ltc_prng_descriptor prng_descriptor[] = {
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

unsigned long rng_get_bytes(unsigned അക്ഷരം *out,
			    unsigned long outlen,
			    ശൂന്യം (*callback)(ശൂന്യം))
{
	തിരികെ px4_get_secure_random((uint8_t *)out, (size_t)outlen);
}

ശൂന്യം libtomcrypt_init(ശൂന്യം)
{
	ltc_mp = ltm_desc;
}
