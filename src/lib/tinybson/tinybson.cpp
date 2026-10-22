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

#અન્તર્ભાવ <px4_platform_common/posix.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <systemlib/err.h>

#અન્તર્ભાવ "tinybson.h"

#જો 0
# define debug(fmt, args...)		કરો { PX4_INFO("BSON: " fmt, ##args); } જ્યારે(0)
#નહીંતર
# define debug(fmt, args...)		કરો { } જ્યારે(0)
#endif

#define CODER_CHECK(_c)		કરો { જો ((_c)->dead) { PX4_ERR("coder dead"); પાછા -1; }} જ્યારે(0)
#define CODER_KILL(_c, _reason)	કરો { PX4_ERR("killed: %s", _reason); (_c)->dead = સાચું; પાછા -1; } જ્યારે(0)

સ્થિર પૂર્ણાંક
read_x(bson_decoder_t decoder, શૂન્ય *p, size_t s)
{
	CODER_CHECK(decoder);

	જો (decoder->fd > -1) {
		પૂર્ણાંક ret = ::પઠ(decoder->fd, p, s);

		જો (ret == s) {
			decoder->total_decoded_size += ret;
			પાછા 0;
		}

		પાછા -1;
	}

	જો (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		જો (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		જો ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		પાછા 0;
	}

	debug("no source");
	પાછા -1;
}

સ્થિર પૂર્ણાંક
read_int8(bson_decoder_t decoder, int8_t *b)
{
	પાછા read_x(decoder, b, કદ(*b));
}

સ્થિર પૂર્ણાંક
read_int32(bson_decoder_t decoder, int32_t *i)
{
	પાછા read_x(decoder, i, કદ(*i));
}

સ્થિર પૂર્ણાંક
read_int64(bson_decoder_t decoder, int64_t *i)
{
	પાછા read_x(decoder, i, કદ(*i));
}

સ્થિર પૂર્ણાંક
read_double(bson_decoder_t decoder, દ્વિગુણ *d)
{
	પાછા read_x(decoder, d, કદ(*d));
}

પૂર્ણાંક
bson_decoder_init_file(bson_decoder_t decoder, પૂર્ણાંક fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->ગાંઠ_2.type = BSON_UNDEFINED;

	// read document size
	જો (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	જો (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	પાછા 0;
}

પૂર્ણાંક
bson_decoder_init_buf(bson_decoder_t decoder, શૂન્ય *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	જો ((buf == nullptr) || (callback == nullptr)) {
		પાછા -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = ખોટું;

	જો (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} નહીંતર {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->ગાંઠ_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	જો (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	જો (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	જો ((decoder->total_document_size > 0) && (decoder->total_document_size > (પૂર્ણાંક)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	પાછા 0;
}

પૂર્ણાંક
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	જો (decoder->ગાંઠ_2.type == BSON_EOO) {
		જો (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		જો (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			પાછા 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	જ્યારે (decoder->pending > 0) {
		જો (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	જો (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->ગાંઠ_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->ગાંઠ_2.type);

	/* EOO is special; it has no name/data following */
	જો (decoder->ગાંઠ_2.type == BSON_EOO) {
		decoder->ગાંઠ_2.name[0] = '\0';

	} નહીંતર જો ((પૂર્ણાંક)decoder->ગાંઠ_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = સાચું;
		પાછા -ENODATA;

	} નહીંતર {

		/* get the node name */
		nlen = 0;

		માટે (;;) {
			જો (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->ગાંઠ_2.type, decoder->ગાંઠ_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			જો (read_int8(decoder, (int8_t *)&decoder->ગાંઠ_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			જો (decoder->ગાંઠ_2.name[nlen] == '\0') {
				તોડો;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->ગાંઠ_2.name);

		પસંદગી (decoder->ગાંઠ_2.type) {
		સ્થિતિ BSON_DOUBLE:
			જો (read_double(decoder, &decoder->ગાંઠ_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->ગણતરી_ગાંઠ_double++;
			તોડો;

		સ્થિતિ BSON_STRING:
			જો (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->ગણતરી_ગાંઠ_string++;
			તોડો;

		સ્થિતિ BSON_BINDATA:
			જો (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			જો (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->ગાંઠ_2.subtype = (bson_દ્વિઅંકી_subtype_t)tbyte;
			decoder->ગણતરી_ગાંઠ_bindata++;
			તોડો;

		સ્થિતિ BSON_BOOL:
			જો (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->ગાંઠ_2.b = (tbyte != 0);
			decoder->ગણતરી_ગાંઠ_bool++;
			તોડો;

		સ્થિતિ BSON_INT32:
			જો (read_int32(decoder, &decoder->ગાંઠ_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ગણતરી_ગાંઠ_int32++;
			તોડો;

		સ્થિતિ BSON_INT64:
			જો (read_int64(decoder, &decoder->ગાંઠ_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ગણતરી_ગાંઠ_int64++;
			તોડો;

		સ્થિતિ BSON_nullptr:
		સ્થિતિ BSON_UNDEFINED:
			તોડો;

		/* XXX currently not supporting other types */
		મૂળભૂત:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	પાછા decoder->callback(decoder, &decoder->ગાંઠ_2);
}

પૂર્ણાંક
bson_decoder_copy_data(bson_decoder_t decoder, શૂન્ય *buf)
{
	પૂર્ણાંક result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	જો (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	પાછા 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	પાછા decoder->pending;
}

સ્થિર પૂર્ણાંક
write_x(bson_encoder_t encoder, અચલ શૂન્ય *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	જો (encoder->fd > -1 && encoder->buf == nullptr) {
		પૂર્ણાંક ret = ::લિખ(encoder->fd, p, s);

		જો (ret == s) {
			encoder->total_document_size += ret;
			પાછા 0;
		}

		પાછા -1;
	}

	/* do we need to extend the buffer? */
	જ્યારે ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		જો (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			પૂર્ણાંક ret = ::લિખ(encoder->fd, encoder->buf, encoder->bufpos);

			જો (ret == (પૂર્ણાંક)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				જો ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				તોડો;

			} નહીંતર {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		જો (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		જો (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	પાછા 0;
}

સ્થિર પૂર્ણાંક
write_int8(bson_encoder_t encoder, int8_t b)
{
	પાછા write_x(encoder, &b, કદ(b));
}

સ્થિર પૂર્ણાંક
write_int32(bson_encoder_t encoder, int32_t i)
{
	પાછા write_x(encoder, &i, કદ(i));
}

સ્થિર પૂર્ણાંક
write_int64(bson_encoder_t encoder, int64_t i)
{
	પાછા write_x(encoder, &i, કદ(i));
}

સ્થિર પૂર્ણાંક
write_double(bson_encoder_t encoder, દ્વિગુણ d)
{
	પાછા write_x(encoder, &d, કદ(d));
}

સ્થિર પૂર્ણાંક
write_name(bson_encoder_t encoder, અચલ અક્ષર *name)
{
	size_t len = strlen(name);

	જો (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	પાછા write_x(encoder, name, len + 1);
}

પૂર્ણાંક
bson_encoder_init_file(bson_encoder_t encoder, પૂર્ણાંક fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = ખોટું;
	encoder->total_document_size = 0;

	જો (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_init_buf_file(bson_encoder_t encoder, પૂર્ણાંક fd, શૂન્ય *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = ખોટું;
	encoder->realloc_ok = ખોટું;
	encoder->total_document_size = 0;

	જો (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_init_buf(bson_encoder_t encoder, શૂન્ય *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	જો (encoder->buf == nullptr) {
		encoder->realloc_ok = સાચું;

	} નહીંતર {
		encoder->bufsize = bufsize;
	}

	જો (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	જો (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	જો (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		પૂર્ણાંક ret = ::લિખ(encoder->fd, encoder->buf, encoder->bufpos);

		જો (ret == (પૂર્ણાંક)encoder->bufpos) {
			encoder->total_document_size += ret;

		} નહીંતર {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	અચલ int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	જો (encoder->fd > -1) {
		જો ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::લિખ(encoder->fd, &bson_doc_bytes, કદ(bson_doc_bytes)) != કદ(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} નહીંતર જો (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, કદ(bson_doc_bytes));
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	જો (encoder->fd > -1) {
		પાછા -1;
	}

	પાછા encoder->bufpos;
}

શૂન્ય *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	જો (encoder->fd > -1) {
		પાછા nullptr;
	}

	પાછા encoder->buf;
}

પૂર્ણાંક bson_encoder_append_bool(bson_encoder_t encoder, અચલ અક્ષર *name, તાર્કિક value)
{
	CODER_CHECK(encoder);

	જો (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_append_int32(bson_encoder_t encoder, અચલ અક્ષર *name, int32_t value)
{
	CODER_CHECK(encoder);

	જો (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_append_int64(bson_encoder_t encoder, અચલ અક્ષર *name, int64_t value)
{
	CODER_CHECK(encoder);

	જો (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_append_double(bson_encoder_t encoder, અચલ અક્ષર *name, દ્વિગુણ value)
{
	CODER_CHECK(encoder);

	જો (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_append_string(bson_encoder_t encoder, અચલ અક્ષર *name, અચલ અક્ષર *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	જો (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_append_દ્વિઅંકી(bson_encoder_t encoder, અચલ અક્ષર *name, bson_દ્વિઅંકી_subtype_t subtype, size_t size,
			   અચલ શૂન્ય *data)
{
	CODER_CHECK(encoder);

	જો (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	પાછા 0;
}

પૂર્ણાંક
bson_encoder_append_null(bson_encoder_t encoder, અચલ અક્ષર *name)
{
	CODER_CHECK(encoder);

	જો (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	પાછા 0;
}
