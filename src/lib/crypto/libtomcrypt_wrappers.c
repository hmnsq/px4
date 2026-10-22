#ਅਨ੍ਤਰ੍ਭਾਵ <px4_random.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <tomcrypt.h>

ਬਣਤਰ ltc_hash_descriptor hash_descriptor[] = {
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
		&sha256_ਪੂਰਾ,
		&sha256_test,
		ਨਿਰਾ
	}
};

ਬਣਤਰ ltc_prng_descriptor prng_descriptor[] = {
	{
		"sprng", 0,
		&sprng_ਸ਼ੁਰੂ,
		&sprng_ਜੋੜੋ_entropy,
		&sprng_ready,
		&sprng_read,
		&sprng_ਪੂਰਾ,
		&sprng_export,
		&sprng_import,
		&sprng_test
	}
};

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rng_get_ਬਾਈਟਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਾਹਰਲਾ,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ outlen,
			    ਖਾਲੀ (*callback)(ਖਾਲੀ))
{
	ਵਾਪਸ px4_get_secure_random((uint8_t *)ਬਾਹਰਲਾ, (size_t)outlen);
}

ਖਾਲੀ libtomcrypt_init(ਖਾਲੀ)
{
	ltc_mp = ltm_desc;
}
