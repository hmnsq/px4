#అన్తర్భావ <px4_random.h>
#అన్తర్భావ <tomcrypt.h>

నిర్మాణం ltc_hash_descriptor hash_descriptor[] = {
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
		&sha256_పూర్తి,
		&sha256_test,
		ఖాళీ
	}
};

నిర్మాణం ltc_prng_descriptor prng_descriptor[] = {
	{
		"sprng", 0,
		&sprng_ప్రారంభం,
		&sprng_కూడు_entropy,
		&sprng_ready,
		&sprng_read,
		&sprng_పూర్తి,
		&sprng_export,
		&sprng_import,
		&sprng_test
	}
};

గుర్తులేని దీర్ఘ rng_get_బైట్లు(గుర్తులేని అక్షరం *బహిర్గతం,
			    గుర్తులేని దీర్ఘ outlen,
			    శూన్య (*callback)(శూన్య))
{
	తిరిగి px4_get_secure_random((uint8_t *)బహిర్గతం, (size_t)outlen);
}

శూన్య libtomcrypt_init(శూన్య)
{
	ltc_mp = ltm_desc;
}
