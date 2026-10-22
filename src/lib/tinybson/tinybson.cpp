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

#অন্তর্ভাব <px4_platform_common/posix.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <systemlib/err.h>

#অন্তর্ভাব "tinybson.h"

#যদি 0
# define debug(fmt, args...)		করো { PX4_INFO("BSON: " fmt, ##args); } যখন(0)
#অন্যথা
# define debug(fmt, args...)		করো { } যখন(0)
#endif

#define CODER_CHECK(_c)		করো { যদি ((_c)->dead) { PX4_ERR("coder dead"); ফেরত -1; }} যখন(0)
#define CODER_KILL(_c, _reason)	করো { PX4_ERR("killed: %s", _reason); (_c)->dead = সত্য; ফেরত -1; } যখন(0)

স্থির পূর্ণসংখ্যা
read_x(bson_decoder_t decoder, শূন্য *p, size_t s)
{
	CODER_CHECK(decoder);

	যদি (decoder->fd > -1) {
		পূর্ণসংখ্যা ret = ::পঠ(decoder->fd, p, s);

		যদি (ret == s) {
			decoder->যোগ_decoded_আকার += ret;
			ফেরত 0;
		}

		ফেরত -1;
	}

	যদি (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		যদি (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		যদি ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->যোগ_decoded_আকার += s;
		ফেরত 0;
	}

	debug("no source");
	ফেরত -1;
}

স্থির পূর্ণসংখ্যা
read_int8(bson_decoder_t decoder, int8_t *b)
{
	ফেরত read_x(decoder, b, আকার(*b));
}

স্থির পূর্ণসংখ্যা
read_int32(bson_decoder_t decoder, int32_t *i)
{
	ফেরত read_x(decoder, i, আকার(*i));
}

স্থির পূর্ণসংখ্যা
read_int64(bson_decoder_t decoder, int64_t *i)
{
	ফেরত read_x(decoder, i, আকার(*i));
}

স্থির পূর্ণসংখ্যা
read_double(bson_decoder_t decoder, দ্বিগুণ *d)
{
	ফেরত read_x(decoder, d, আকার(*d));
}

পূর্ণসংখ্যা
bson_decoder_init_file(bson_decoder_t decoder, পূর্ণসংখ্যা fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->গ্রন্থি_2.type = BSON_UNDEFINED;

	// read document size
	যদি (read_int32(decoder, &decoder->যোগ_document_আকার)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->যোগ_document_আকার);

	// The length is signed and comes from the document itself
	যদি (decoder->যোগ_document_আকার < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	ফেরত 0;
}

পূর্ণসংখ্যা
bson_decoder_init_buf(bson_decoder_t decoder, শূন্য *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	যদি ((buf == nullptr) || (callback == nullptr)) {
		ফেরত -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = মিথ্যা;

	যদি (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} অন্যথা {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->গ্রন্থি_2.type = BSON_UNDEFINED;
	decoder->যোগ_decoded_আকার = 0;

	// read document size
	decoder->যোগ_document_আকার = 0;

	যদি (read_int32(decoder, &decoder->যোগ_document_আকার)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->যোগ_document_আকার);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	যদি (decoder->যোগ_document_আকার < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	যদি ((decoder->যোগ_document_আকার > 0) && (decoder->যোগ_document_আকার > (পূর্ণসংখ্যা)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	ফেরত 0;
}

পূর্ণসংখ্যা
bson_decoder_পরবর্তী(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	যদি (decoder->গ্রন্থি_2.type == BSON_EOO) {
		যদি (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		যদি (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			ফেরত 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	যখন (decoder->pending > 0) {
		যদি (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	যদি (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->গ্রন্থি_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->গ্রন্থি_2.type);

	/* EOO is special; it has no name/data following */
	যদি (decoder->গ্রন্থি_2.type == BSON_EOO) {
		decoder->গ্রন্থি_2.name[0] = '\0';

	} অন্যথা যদি ((পূর্ণসংখ্যা)decoder->গ্রন্থি_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = সত্য;
		ফেরত -ENODATA;

	} অন্যথা {

		/* get the node name */
		nlen = 0;

		জন্য (;;) {
			যদি (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->গ্রন্থি_2.type, decoder->গ্রন্থি_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			যদি (read_int8(decoder, (int8_t *)&decoder->গ্রন্থি_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			যদি (decoder->গ্রন্থি_2.name[nlen] == '\0') {
				বিরতি;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->গ্রন্থি_2.name);

		নির্বাচন (decoder->গ্রন্থি_2.type) {
		ক্ষেত্র BSON_DOUBLE:
			যদি (read_double(decoder, &decoder->গ্রন্থি_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->গুনতি_গ্রন্থি_double++;
			বিরতি;

		ক্ষেত্র BSON_STRING:
			যদি (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->গুনতি_গ্রন্থি_string++;
			বিরতি;

		ক্ষেত্র BSON_BINDATA:
			যদি (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			যদি (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->গ্রন্থি_2.subtype = (bson_দ্বিমিক_subtype_t)tbyte;
			decoder->গুনতি_গ্রন্থি_bindata++;
			বিরতি;

		ক্ষেত্র BSON_BOOL:
			যদি (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->গ্রন্থি_2.b = (tbyte != 0);
			decoder->গুনতি_গ্রন্থি_bool++;
			বিরতি;

		ক্ষেত্র BSON_INT32:
			যদি (read_int32(decoder, &decoder->গ্রন্থি_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->গুনতি_গ্রন্থি_int32++;
			বিরতি;

		ক্ষেত্র BSON_INT64:
			যদি (read_int64(decoder, &decoder->গ্রন্থি_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->গুনতি_গ্রন্থি_int64++;
			বিরতি;

		ক্ষেত্র BSON_nullptr:
		ক্ষেত্র BSON_UNDEFINED:
			বিরতি;

		/* XXX currently not supporting other types */
		পূর্বনির্ধারিত:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	ফেরত decoder->callback(decoder, &decoder->গ্রন্থি_2);
}

পূর্ণসংখ্যা
bson_decoder_copy_তথ্য(bson_decoder_t decoder, শূন্য *buf)
{
	পূর্ণসংখ্যা ফলাফল;

	CODER_CHECK(decoder);

	/* copy data */
	ফলাফল = read_x(decoder, buf, decoder->pending);

	যদি (ফলাফল != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	ফেরত 0;
}

size_t
bson_decoder_তথ্য_pending(bson_decoder_t decoder)
{
	ফেরত decoder->pending;
}

স্থির পূর্ণসংখ্যা
write_x(bson_encoder_t encoder, ধ্রুবক শূন্য *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	যদি (encoder->fd > -1 && encoder->buf == nullptr) {
		পূর্ণসংখ্যা ret = ::লিখ(encoder->fd, p, s);

		যদি (ret == s) {
			encoder->যোগ_document_আকার += ret;
			ফেরত 0;
		}

		ফেরত -1;
	}

	/* do we need to extend the buffer? */
	যখন ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		যদি (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			পূর্ণসংখ্যা ret = ::লিখ(encoder->fd, encoder->buf, encoder->bufpos);

			যদি (ret == (পূর্ণসংখ্যা)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->যোগ_document_আকার += ret;

				যদি ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				বিরতি;

			} অন্যথা {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		যদি (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		যদি (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা
write_int8(bson_encoder_t encoder, int8_t b)
{
	ফেরত write_x(encoder, &b, আকার(b));
}

স্থির পূর্ণসংখ্যা
write_int32(bson_encoder_t encoder, int32_t i)
{
	ফেরত write_x(encoder, &i, আকার(i));
}

স্থির পূর্ণসংখ্যা
write_int64(bson_encoder_t encoder, int64_t i)
{
	ফেরত write_x(encoder, &i, আকার(i));
}

স্থির পূর্ণসংখ্যা
write_double(bson_encoder_t encoder, দ্বিগুণ d)
{
	ফেরত write_x(encoder, &d, আকার(d));
}

স্থির পূর্ণসংখ্যা
write_name(bson_encoder_t encoder, ধ্রুবক অক্ষর *name)
{
	size_t len = strlen(name);

	যদি (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	ফেরত write_x(encoder, name, len + 1);
}

পূর্ণসংখ্যা
bson_encoder_init_file(bson_encoder_t encoder, পূর্ণসংখ্যা fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = মিথ্যা;
	encoder->যোগ_document_আকার = 0;

	যদি (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_init_buf_file(bson_encoder_t encoder, পূর্ণসংখ্যা fd, শূন্য *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = মিথ্যা;
	encoder->realloc_ok = মিথ্যা;
	encoder->যোগ_document_আকার = 0;

	যদি (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_init_buf(bson_encoder_t encoder, শূন্য *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	যদি (encoder->buf == nullptr) {
		encoder->realloc_ok = সত্য;

	} অন্যথা {
		encoder->bufsize = bufsize;
	}

	যদি (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	যদি (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	যদি (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		পূর্ণসংখ্যা ret = ::লিখ(encoder->fd, encoder->buf, encoder->bufpos);

		যদি (ret == (পূর্ণসংখ্যা)encoder->bufpos) {
			encoder->যোগ_document_আকার += ret;

		} অন্যথা {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ধ্রুবক int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->যোগ_document_আকার : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	যদি (encoder->fd > -1) {
		যদি ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::লিখ(encoder->fd, &bson_doc_bytes, আকার(bson_doc_bytes)) != আকার(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} অন্যথা যদি (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, আকার(bson_doc_bytes));
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_buf_আকার(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	যদি (encoder->fd > -1) {
		ফেরত -1;
	}

	ফেরত encoder->bufpos;
}

শূন্য *
bson_encoder_buf_তথ্য(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	যদি (encoder->fd > -1) {
		ফেরত nullptr;
	}

	ফেরত encoder->buf;
}

পূর্ণসংখ্যা bson_encoder_append_bool(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, যৌক্তিক value)
{
	CODER_CHECK(encoder);

	যদি (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_append_int32(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, int32_t value)
{
	CODER_CHECK(encoder);

	যদি (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_append_int64(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, int64_t value)
{
	CODER_CHECK(encoder);

	যদি (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_append_double(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, দ্বিগুণ value)
{
	CODER_CHECK(encoder);

	যদি (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_append_string(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	যদি (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_append_দ্বিমিক(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, bson_দ্বিমিক_subtype_t subtype, size_t আকার_2,
			   ধ্রুবক শূন্য *তথ্য)
{
	CODER_CHECK(encoder);

	যদি (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, আকার_2) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, তথ্য, আকার_2)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	ফেরত 0;
}

পূর্ণসংখ্যা
bson_encoder_append_null(bson_encoder_t encoder, ধ্রুবক অক্ষর *name)
{
	CODER_CHECK(encoder);

	যদি (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	ফেরত 0;
}
