/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file tinybson.cpp
 *
 * A simple subset SAX-style BSON parser and generator.
 */

#अन्तर्भाव <px4_platform_common/posix.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <systemlib/err.h>

#अन्तर्भाव "tinybson.h"

#यदि 0
# define debug(fmt, args...)		कुरु { PX4_INFO("BSON: " fmt, ##args); } यावत्(0)
#अन्यथा
# define debug(fmt, args...)		कुरु { } यावत्(0)
#endif

#define CODER_CHECK(_c)		कुरु { यदि ((_c)->dead) { PX4_ERR("coder dead"); प्रत्यागम -1; }} यावत्(0)
#define CODER_KILL(_c, _reason)	कुरु { PX4_ERR("killed: %s", _reason); (_c)->dead = सत्य; प्रत्यागम -1; } यावत्(0)

स्थिर पूर्णाङ्क
read_x(bson_decoder_t decoder, शून्य *p, size_t s)
{
	CODER_CHECK(decoder);

	यदि (decoder->fd > -1) {
		पूर्णाङ्क ret = ::पठ(decoder->fd, p, s);

		यदि (ret == s) {
			decoder->total_decoded_size += ret;
			प्रत्यागम 0;
		}

		प्रत्यागम -1;
	}

	यदि (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		यदि (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		यदि ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		प्रत्यागम 0;
	}

	debug("no source");
	प्रत्यागम -1;
}

स्थिर पूर्णाङ्क
read_int8(bson_decoder_t decoder, int8_t *b)
{
	प्रत्यागम read_x(decoder, b, परिमाण(*b));
}

स्थिर पूर्णाङ्क
read_int32(bson_decoder_t decoder, int32_t *i)
{
	प्रत्यागम read_x(decoder, i, परिमाण(*i));
}

स्थिर पूर्णाङ्क
read_int64(bson_decoder_t decoder, int64_t *i)
{
	प्रत्यागम read_x(decoder, i, परिमाण(*i));
}

स्थिर पूर्णाङ्क
read_double(bson_decoder_t decoder, द्विगुण *d)
{
	प्रत्यागम read_x(decoder, d, परिमाण(*d));
}

पूर्णाङ्क
bson_decoder_init_file(bson_decoder_t decoder, पूर्णाङ्क fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->गाँठ_2.type = BSON_UNDEFINED;

	// read document size
	यदि (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	यदि (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	प्रत्यागम 0;
}

पूर्णाङ्क
bson_decoder_init_buf(bson_decoder_t decoder, शून्य *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	यदि ((buf == nullptr) || (callback == nullptr)) {
		प्रत्यागम -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = असत्य;

	यदि (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} अन्यथा {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->गाँठ_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	यदि (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	यदि (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	यदि ((decoder->total_document_size > 0) && (decoder->total_document_size > (पूर्णाङ्क)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	प्रत्यागम 0;
}

पूर्णाङ्क
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	यदि (decoder->गाँठ_2.type == BSON_EOO) {
		यदि (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		यदि (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			प्रत्यागम 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	यावत् (decoder->pending > 0) {
		यदि (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	यदि (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->गाँठ_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->गाँठ_2.type);

	/* EOO is special; it has no name/data following */
	यदि (decoder->गाँठ_2.type == BSON_EOO) {
		decoder->गाँठ_2.name[0] = '\0';

	} अन्यथा यदि ((पूर्णाङ्क)decoder->गाँठ_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = सत्य;
		प्रत्यागम -ENODATA;

	} अन्यथा {

		/* get the node name */
		nlen = 0;

		प्रति (;;) {
			यदि (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->गाँठ_2.type, decoder->गाँठ_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			यदि (read_int8(decoder, (int8_t *)&decoder->गाँठ_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			यदि (decoder->गाँठ_2.name[nlen] == '\0') {
				भङ्ग;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->गाँठ_2.name);

		विकल्प (decoder->गाँठ_2.type) {
		पक्ष BSON_DOUBLE:
			यदि (read_double(decoder, &decoder->गाँठ_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->गिनती_गाँठ_double++;
			भङ्ग;

		पक्ष BSON_STRING:
			यदि (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->गिनती_गाँठ_string++;
			भङ्ग;

		पक्ष BSON_BINDATA:
			यदि (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			यदि (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->गाँठ_2.subtype = (bson_द्विआधारी_subtype_t)tbyte;
			decoder->गिनती_गाँठ_bindata++;
			भङ्ग;

		पक्ष BSON_BOOL:
			यदि (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->गाँठ_2.b = (tbyte != 0);
			decoder->गिनती_गाँठ_bool++;
			भङ्ग;

		पक्ष BSON_INT32:
			यदि (read_int32(decoder, &decoder->गाँठ_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->गिनती_गाँठ_int32++;
			भङ्ग;

		पक्ष BSON_INT64:
			यदि (read_int64(decoder, &decoder->गाँठ_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->गिनती_गाँठ_int64++;
			भङ्ग;

		पक्ष BSON_nullptr:
		पक्ष BSON_UNDEFINED:
			भङ्ग;

		/* XXX currently not supporting other types */
		स्वभाव:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	प्रत्यागम decoder->callback(decoder, &decoder->गाँठ_2);
}

पूर्णाङ्क
bson_decoder_copy_data(bson_decoder_t decoder, शून्य *buf)
{
	पूर्णाङ्क result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	यदि (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	प्रत्यागम 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	प्रत्यागम decoder->pending;
}

स्थिर पूर्णाङ्क
write_x(bson_encoder_t encoder, ध्रुव शून्य *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	यदि (encoder->fd > -1 && encoder->buf == nullptr) {
		पूर्णाङ्क ret = ::लिख(encoder->fd, p, s);

		यदि (ret == s) {
			encoder->total_document_size += ret;
			प्रत्यागम 0;
		}

		प्रत्यागम -1;
	}

	/* do we need to extend the buffer? */
	यावत् ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		यदि (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			पूर्णाङ्क ret = ::लिख(encoder->fd, encoder->buf, encoder->bufpos);

			यदि (ret == (पूर्णाङ्क)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				यदि ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				भङ्ग;

			} अन्यथा {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		यदि (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		यदि (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
write_int8(bson_encoder_t encoder, int8_t b)
{
	प्रत्यागम write_x(encoder, &b, परिमाण(b));
}

स्थिर पूर्णाङ्क
write_int32(bson_encoder_t encoder, int32_t i)
{
	प्रत्यागम write_x(encoder, &i, परिमाण(i));
}

स्थिर पूर्णाङ्क
write_int64(bson_encoder_t encoder, int64_t i)
{
	प्रत्यागम write_x(encoder, &i, परिमाण(i));
}

स्थिर पूर्णाङ्क
write_double(bson_encoder_t encoder, द्विगुण d)
{
	प्रत्यागम write_x(encoder, &d, परिमाण(d));
}

स्थिर पूर्णाङ्क
write_name(bson_encoder_t encoder, ध्रुव वर्ण *name)
{
	size_t len = strlen(name);

	यदि (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	प्रत्यागम write_x(encoder, name, len + 1);
}

पूर्णाङ्क
bson_encoder_init_file(bson_encoder_t encoder, पूर्णाङ्क fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = असत्य;
	encoder->total_document_size = 0;

	यदि (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_init_buf_file(bson_encoder_t encoder, पूर्णाङ्क fd, शून्य *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = असत्य;
	encoder->realloc_ok = असत्य;
	encoder->total_document_size = 0;

	यदि (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_init_buf(bson_encoder_t encoder, शून्य *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	यदि (encoder->buf == nullptr) {
		encoder->realloc_ok = सत्य;

	} अन्यथा {
		encoder->bufsize = bufsize;
	}

	यदि (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	यदि (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	यदि (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		पूर्णाङ्क ret = ::लिख(encoder->fd, encoder->buf, encoder->bufpos);

		यदि (ret == (पूर्णाङ्क)encoder->bufpos) {
			encoder->total_document_size += ret;

		} अन्यथा {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ध्रुव int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	यदि (encoder->fd > -1) {
		यदि ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::लिख(encoder->fd, &bson_doc_bytes, परिमाण(bson_doc_bytes)) != परिमाण(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} अन्यथा यदि (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, परिमाण(bson_doc_bytes));
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	यदि (encoder->fd > -1) {
		प्रत्यागम -1;
	}

	प्रत्यागम encoder->bufpos;
}

शून्य *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	यदि (encoder->fd > -1) {
		प्रत्यागम nullptr;
	}

	प्रत्यागम encoder->buf;
}

पूर्णाङ्क bson_encoder_append_bool(bson_encoder_t encoder, ध्रुव वर्ण *name, तार्किक value)
{
	CODER_CHECK(encoder);

	यदि (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_append_int32(bson_encoder_t encoder, ध्रुव वर्ण *name, int32_t value)
{
	CODER_CHECK(encoder);

	यदि (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_append_int64(bson_encoder_t encoder, ध्रुव वर्ण *name, int64_t value)
{
	CODER_CHECK(encoder);

	यदि (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_append_double(bson_encoder_t encoder, ध्रुव वर्ण *name, द्विगुण value)
{
	CODER_CHECK(encoder);

	यदि (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_append_string(bson_encoder_t encoder, ध्रुव वर्ण *name, ध्रुव वर्ण *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	यदि (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_append_द्विआधारी(bson_encoder_t encoder, ध्रुव वर्ण *name, bson_द्विआधारी_subtype_t subtype, size_t size,
			   ध्रुव शून्य *data)
{
	CODER_CHECK(encoder);

	यदि (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
bson_encoder_append_null(bson_encoder_t encoder, ध्रुव वर्ण *name)
{
	CODER_CHECK(encoder);

	यदि (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	प्रत्यागम 0;
}
