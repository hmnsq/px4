#شامل <px4_random.h>
#شامل <tomcrypt.h>

ساخت ltc_hash_descriptor hash_descriptor[] = {
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
		&sha256_مکمل,
		&sha256_test,
		تہی
	}
};

ساخت ltc_prng_descriptor prng_descriptor[] = {
	{
		"sprng", 0,
		&sprng_آغاز,
		&sprng_جوڑو_entropy,
		&sprng_ready,
		&sprng_read,
		&sprng_مکمل,
		&sprng_export,
		&sprng_import,
		&sprng_test
	}
};

غیرعلامتی طویل rng_get_بائٹیں(غیرعلامتی حرف *برآمد,
			    غیرعلامتی طویل outlen,
			    خالی (*callback)(خالی))
{
	واپس px4_get_secure_random((uint8_t *)برآمد, (size_t)outlen);
}

خالی libtomcrypt_init(خالی)
{
	ltc_mp = ltm_desc;
}
