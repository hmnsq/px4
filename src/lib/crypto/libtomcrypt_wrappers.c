#அந்தர்பாவ <px4_random.h>
#அந்தர்பாவ <tomcrypt.h>

கட்டமைப்பு ltc_hash_descriptor hash_descriptor[] = {
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
		வெற்று
	}
};

கட்டமைப்பு ltc_prng_descriptor prng_descriptor[] = {
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

unsigned long rng_get_bytes(unsigned எழுத்து *out,
			    unsigned long outlen,
			    வெறுமை (*callback)(வெறுமை))
{
	திரும்பு px4_get_secure_random((uint8_t *)out, (size_t)outlen);
}

வெறுமை libtomcrypt_init(வெறுமை)
{
	ltc_mp = ltm_desc;
}
