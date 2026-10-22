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

#شامل <px4_platform_common/posix.h>
#شامل <unistd.h>
#شامل <string.h>
#شامل <stdlib.h>
#شامل <systemlib/err.h>

#شامل "tinybson.h"

#اگر 0
# define debug(fmt, args...)		انجامبده { PX4_INFO("BSON: " fmt, ##args); } وقتی(0)
#وگرنه
# define debug(fmt, args...)		انجامبده { } وقتی(0)
#endif

#define CODER_CHECK(_c)		انجامبده { اگر ((_c)->dead) { PX4_ERR("coder dead"); بازگشت -1; }} وقتی(0)
#define CODER_KILL(_c, _reason)	انجامبده { PX4_ERR("killed: %s", _reason); (_c)->dead = درست; بازگشت -1; } وقتی(0)

ایستا صحیح
read_x(bson_decoder_t decoder, تهی *p, size_t s)
{
	CODER_CHECK(decoder);

	اگر (decoder->fd > -1) {
		صحیح ret = ::اقرأ(decoder->fd, p, s);

		اگر (ret == s) {
			decoder->total_decoded_size += ret;
			بازگشت 0;
		}

		بازگشت -1;
	}

	اگر (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		اگر (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		اگر ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		بازگشت 0;
	}

	debug("no source");
	بازگشت -1;
}

ایستا صحیح
read_int8(bson_decoder_t decoder, int8_t *b)
{
	بازگشت read_x(decoder, b, اندازه(*b));
}

ایستا صحیح
read_int32(bson_decoder_t decoder, int32_t *i)
{
	بازگشت read_x(decoder, i, اندازه(*i));
}

ایستا صحیح
read_int64(bson_decoder_t decoder, int64_t *i)
{
	بازگشت read_x(decoder, i, اندازه(*i));
}

ایستا صحیح
read_double(bson_decoder_t decoder, دوگانه *d)
{
	بازگشت read_x(decoder, d, اندازه(*d));
}

صحیح
bson_decoder_init_file(bson_decoder_t decoder, صحیح fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->گره_2.type = BSON_UNDEFINED;

	// read document size
	اگر (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	اگر (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	بازگشت 0;
}

صحیح
bson_decoder_init_buf(bson_decoder_t decoder, تهی *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	اگر ((buf == nullptr) || (callback == nullptr)) {
		بازگشت -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = نادرست;

	اگر (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} وگرنه {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->گره_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	اگر (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	اگر (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	اگر ((decoder->total_document_size > 0) && (decoder->total_document_size > (صحیح)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	بازگشت 0;
}

صحیح
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	اگر (decoder->گره_2.type == BSON_EOO) {
		اگر (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		اگر (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			بازگشت 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	وقتی (decoder->pending > 0) {
		اگر (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	اگر (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->گره_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->گره_2.type);

	/* EOO is special; it has no name/data following */
	اگر (decoder->گره_2.type == BSON_EOO) {
		decoder->گره_2.name[0] = '\0';

	} وگرنه اگر ((صحیح)decoder->گره_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = درست;
		بازگشت -ENODATA;

	} وگرنه {

		/* get the node name */
		nlen = 0;

		برای (;;) {
			اگر (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->گره_2.type, decoder->گره_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			اگر (read_int8(decoder, (int8_t *)&decoder->گره_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			اگر (decoder->گره_2.name[nlen] == '\0') {
				بشکن;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->گره_2.name);

		انتخاب (decoder->گره_2.type) {
		حالت BSON_DOUBLE:
			اگر (read_double(decoder, &decoder->گره_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->شمار_گره_double++;
			بشکن;

		حالت BSON_STRING:
			اگر (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->شمار_گره_string++;
			بشکن;

		حالت BSON_BINDATA:
			اگر (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			اگر (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->گره_2.subtype = (bson_دودویی_subtype_t)tbyte;
			decoder->شمار_گره_bindata++;
			بشکن;

		حالت BSON_BOOL:
			اگر (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->گره_2.b = (tbyte != 0);
			decoder->شمار_گره_bool++;
			بشکن;

		حالت BSON_INT32:
			اگر (read_int32(decoder, &decoder->گره_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->شمار_گره_int32++;
			بشکن;

		حالت BSON_INT64:
			اگر (read_int64(decoder, &decoder->گره_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->شمار_گره_int64++;
			بشکن;

		حالت BSON_nullptr:
		حالت BSON_UNDEFINED:
			بشکن;

		/* XXX currently not supporting other types */
		پیشفرض:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	بازگشت decoder->callback(decoder, &decoder->گره_2);
}

صحیح
bson_decoder_copy_data(bson_decoder_t decoder, تهی *buf)
{
	صحیح result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	اگر (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	بازگشت 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	بازگشت decoder->pending;
}

ایستا صحیح
write_x(bson_encoder_t encoder, ثابت تهی *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	اگر (encoder->fd > -1 && encoder->buf == nullptr) {
		صحیح ret = ::اكتب(encoder->fd, p, s);

		اگر (ret == s) {
			encoder->total_document_size += ret;
			بازگشت 0;
		}

		بازگشت -1;
	}

	/* do we need to extend the buffer? */
	وقتی ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		اگر (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			صحیح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

			اگر (ret == (صحیح)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				اگر ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				بشکن;

			} وگرنه {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		اگر (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		اگر (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	بازگشت 0;
}

ایستا صحیح
write_int8(bson_encoder_t encoder, int8_t b)
{
	بازگشت write_x(encoder, &b, اندازه(b));
}

ایستا صحیح
write_int32(bson_encoder_t encoder, int32_t i)
{
	بازگشت write_x(encoder, &i, اندازه(i));
}

ایستا صحیح
write_int64(bson_encoder_t encoder, int64_t i)
{
	بازگشت write_x(encoder, &i, اندازه(i));
}

ایستا صحیح
write_double(bson_encoder_t encoder, دوگانه d)
{
	بازگشت write_x(encoder, &d, اندازه(d));
}

ایستا صحیح
write_name(bson_encoder_t encoder, ثابت نویسه *name)
{
	size_t len = strlen(name);

	اگر (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	بازگشت write_x(encoder, name, len + 1);
}

صحیح
bson_encoder_init_file(bson_encoder_t encoder, صحیح fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = نادرست;
	encoder->total_document_size = 0;

	اگر (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	بازگشت 0;
}

صحیح
bson_encoder_init_buf_file(bson_encoder_t encoder, صحیح fd, تهی *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = نادرست;
	encoder->realloc_ok = نادرست;
	encoder->total_document_size = 0;

	اگر (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	بازگشت 0;
}

صحیح
bson_encoder_init_buf(bson_encoder_t encoder, تهی *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	اگر (encoder->buf == nullptr) {
		encoder->realloc_ok = درست;

	} وگرنه {
		encoder->bufsize = bufsize;
	}

	اگر (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	بازگشت 0;
}

صحیح
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	اگر (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		صحیح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

		اگر (ret == (صحیح)encoder->bufpos) {
			encoder->total_document_size += ret;

		} وگرنه {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ثابت int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	اگر (encoder->fd > -1) {
		اگر ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::اكتب(encoder->fd, &bson_doc_bytes, اندازه(bson_doc_bytes)) != اندازه(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} وگرنه اگر (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, اندازه(bson_doc_bytes));
	}

	بازگشت 0;
}

صحیح
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	اگر (encoder->fd > -1) {
		بازگشت -1;
	}

	بازگشت encoder->bufpos;
}

تهی *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	اگر (encoder->fd > -1) {
		بازگشت nullptr;
	}

	بازگشت encoder->buf;
}

صحیح bson_encoder_append_bool(bson_encoder_t encoder, ثابت نویسه *name, منطقی value)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	بازگشت 0;
}

صحیح
bson_encoder_append_int32(bson_encoder_t encoder, ثابت نویسه *name, int32_t value)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	بازگشت 0;
}

صحیح
bson_encoder_append_int64(bson_encoder_t encoder, ثابت نویسه *name, int64_t value)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	بازگشت 0;
}

صحیح
bson_encoder_append_double(bson_encoder_t encoder, ثابت نویسه *name, دوگانه value)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	بازگشت 0;
}

صحیح
bson_encoder_append_string(bson_encoder_t encoder, ثابت نویسه *name, ثابت نویسه *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	اگر (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	بازگشت 0;
}

صحیح
bson_encoder_append_دودویی(bson_encoder_t encoder, ثابت نویسه *name, bson_دودویی_subtype_t subtype, size_t size,
			   ثابت تهی *data)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	بازگشت 0;
}

صحیح
bson_encoder_append_null(bson_encoder_t encoder, ثابت نویسه *name)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	بازگشت 0;
}
