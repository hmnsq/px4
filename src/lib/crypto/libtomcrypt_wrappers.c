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
		&sha256_समाप्तम्,
		&sha256_test,
		रिक्त
	}
};

संरचना ltc_prng_descriptor prng_descriptor[] = {
	{
		"sprng", 0,
		&sprng_start,
		&sprng_सङ्कलय_entropy,
		&sprng_ready,
		&sprng_read,
		&sprng_समाप्तम्,
		&sprng_export,
		&sprng_import,
		&sprng_test
	}
};

अचिह्नित दीर्घ rng_get_अष्टकानि(अचिह्नित वर्ण *बहिर्गमनम्,
			    अचिह्नित दीर्घ outlen,
			    शून्य (*callback)(शून्य))
{
	प्रत्यागम px4_get_secure_random((uint8_t *)बहिर्गमनम्, (size_t)outlen);
}

शून्य libtomcrypt_init(शून्य)
{
	ltc_mp = ltm_desc;
}
