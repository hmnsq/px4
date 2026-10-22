#ضمن <px4_random.h>
#ضمن <tomcrypt.h>

بنية ltc_hash_descriptor hash_descriptor[] = {
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
		&sha256_پورا,
		&sha256_test,
		عدم
	}
};

بنية ltc_prng_descriptor prng_descriptor[] = {
	{
		"sprng", 0,
		&sprng_شروع,
		&sprng_جوڑو_entropy,
		&sprng_ready,
		&sprng_read,
		&sprng_پورا,
		&sprng_export,
		&sprng_import,
		&sprng_test
	}
};

بنانشانی لما rng_get_بائٹاں(بنانشانی حرف *باہرلا,
			    بنانشانی لما outlen,
			    فراغ (*callback)(فراغ))
{
	أرجع px4_get_secure_random((uint8_t *)باہرلا, (size_t)outlen);
}

فراغ libtomcrypt_init(فراغ)
{
	ltc_mp = ltm_desc;
}
