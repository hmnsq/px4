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

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/posix.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <systemlib/err.h>

#ଅନ୍ତର୍ଭାଵ "tinybson.h"

#ଯଦି 0
# define debug(fmt, args...)		କର { PX4_INFO("BSON: " fmt, ##args); } ଯେତେବେଳେ(0)
#ଅନ୍ୟଥା
# define debug(fmt, args...)		କର { } ଯେତେବେଳେ(0)
#endif

#define CODER_CHECK(_c)		କର { ଯଦି ((_c)->dead) { PX4_ERR("coder dead"); ଫେରନ୍ତୁ -1; }} ଯେତେବେଳେ(0)
#define CODER_KILL(_c, _reason)	କର { PX4_ERR("killed: %s", _reason); (_c)->dead = ସତ୍ୟ; ଫେରନ୍ତୁ -1; } ଯେତେବେଳେ(0)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_x(bson_decoder_t decoder, ଶୂନ୍ୟ *p, size_t s)
{
	CODER_CHECK(decoder);

	ଯଦି (decoder->fd > -1) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = ::ପଠ(decoder->fd, p, s);

		ଯଦି (ret == s) {
			decoder->total_decoded_size += ret;
			ଫେରନ୍ତୁ 0;
		}

		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		ଯଦି (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		ଯଦି ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		ଫେରନ୍ତୁ 0;
	}

	debug("no source");
	ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_int8(bson_decoder_t decoder, int8_t *b)
{
	ଫେରନ୍ତୁ read_x(decoder, b, ଆକାର(*b));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_int32(bson_decoder_t decoder, int32_t *i)
{
	ଫେରନ୍ତୁ read_x(decoder, i, ଆକାର(*i));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_int64(bson_decoder_t decoder, int64_t *i)
{
	ଫେରନ୍ତୁ read_x(decoder, i, ଆକାର(*i));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_double(bson_decoder_t decoder, ଦ୍ୱିଗୁଣ *d)
{
	ଫେରନ୍ତୁ read_x(decoder, d, ଆକାର(*d));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_decoder_init_file(bson_decoder_t decoder, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->ଗଣ୍ଠି_2.type = BSON_UNDEFINED;

	// read document size
	ଯଦି (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	ଯଦି (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_decoder_init_buf(bson_decoder_t decoder, ଶୂନ୍ୟ *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	ଯଦି ((buf == nullptr) || (callback == nullptr)) {
		ଫେରନ୍ତୁ -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = ମିଥ୍ୟା;

	ଯଦି (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} ଅନ୍ୟଥା {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->ଗଣ୍ଠି_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	ଯଦି (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	ଯଦି (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	ଯଦି ((decoder->total_document_size > 0) && (decoder->total_document_size > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	ଯଦି (decoder->ଗଣ୍ଠି_2.type == BSON_EOO) {
		ଯଦି (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		ଯଦି (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			ଫେରନ୍ତୁ 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	ଯେତେବେଳେ (decoder->pending > 0) {
		ଯଦି (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	ଯଦି (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->ଗଣ୍ଠି_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->ଗଣ୍ଠି_2.type);

	/* EOO is special; it has no name/data following */
	ଯଦି (decoder->ଗଣ୍ଠି_2.type == BSON_EOO) {
		decoder->ଗଣ୍ଠି_2.name[0] = '\0';

	} ଅନ୍ୟଥା ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)decoder->ଗଣ୍ଠି_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = ସତ୍ୟ;
		ଫେରନ୍ତୁ -ENODATA;

	} ଅନ୍ୟଥା {

		/* get the node name */
		nlen = 0;

		ପାଇଁ (;;) {
			ଯଦି (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->ଗଣ୍ଠି_2.type, decoder->ଗଣ୍ଠି_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			ଯଦି (read_int8(decoder, (int8_t *)&decoder->ଗଣ୍ଠି_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			ଯଦି (decoder->ଗଣ୍ଠି_2.name[nlen] == '\0') {
				ଭାଙ୍ଗ;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->ଗଣ୍ଠି_2.name);

		ବାଛ (decoder->ଗଣ୍ଠି_2.type) {
		ସ୍ଥିତି BSON_DOUBLE:
			ଯଦି (read_double(decoder, &decoder->ଗଣ୍ଠି_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->ଗଣନା_ଗଣ୍ଠି_double++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି BSON_STRING:
			ଯଦି (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->ଗଣନା_ଗଣ୍ଠି_string++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି BSON_BINDATA:
			ଯଦି (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			ଯଦି (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->ଗଣ୍ଠି_2.subtype = (bson_ଦ୍ୱିମିକ_subtype_t)tbyte;
			decoder->ଗଣନା_ଗଣ୍ଠି_bindata++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି BSON_BOOL:
			ଯଦି (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->ଗଣ୍ଠି_2.b = (tbyte != 0);
			decoder->ଗଣନା_ଗଣ୍ଠି_bool++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି BSON_INT32:
			ଯଦି (read_int32(decoder, &decoder->ଗଣ୍ଠି_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ଗଣନା_ଗଣ୍ଠି_int32++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି BSON_INT64:
			ଯଦି (read_int64(decoder, &decoder->ଗଣ୍ଠି_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ଗଣନା_ଗଣ୍ଠି_int64++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି BSON_nullptr:
		ସ୍ଥିତି BSON_UNDEFINED:
			ଭାଙ୍ଗ;

		/* XXX currently not supporting other types */
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	ଫେରନ୍ତୁ decoder->callback(decoder, &decoder->ଗଣ୍ଠି_2);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_decoder_copy_data(bson_decoder_t decoder, ଶୂନ୍ୟ *buf)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	ଯଦି (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	ଫେରନ୍ତୁ 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	ଫେରନ୍ତୁ decoder->pending;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_x(bson_encoder_t encoder, ଅଚଳ ଶୂନ୍ୟ *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	ଯଦି (encoder->fd > -1 && encoder->buf == nullptr) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = ::ଲିଖ(encoder->fd, p, s);

		ଯଦି (ret == s) {
			encoder->total_document_size += ret;
			ଫେରନ୍ତୁ 0;
		}

		ଫେରନ୍ତୁ -1;
	}

	/* do we need to extend the buffer? */
	ଯେତେବେଳେ ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		ଯଦି (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = ::ଲିଖ(encoder->fd, encoder->buf, encoder->bufpos);

			ଯଦି (ret == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				ଯଦି ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				ଭାଙ୍ଗ;

			} ଅନ୍ୟଥା {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		ଯଦି (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		ଯଦି (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_int8(bson_encoder_t encoder, int8_t b)
{
	ଫେରନ୍ତୁ write_x(encoder, &b, ଆକାର(b));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_int32(bson_encoder_t encoder, int32_t i)
{
	ଫେରନ୍ତୁ write_x(encoder, &i, ଆକାର(i));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_int64(bson_encoder_t encoder, int64_t i)
{
	ଫେରନ୍ତୁ write_x(encoder, &i, ଆକାର(i));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_double(bson_encoder_t encoder, ଦ୍ୱିଗୁଣ d)
{
	ଫେରନ୍ତୁ write_x(encoder, &d, ଆକାର(d));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_name(bson_encoder_t encoder, ଅଚଳ ଅକ୍ଷର *name)
{
	size_t len = strlen(name);

	ଯଦି (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	ଫେରନ୍ତୁ write_x(encoder, name, len + 1);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_init_file(bson_encoder_t encoder, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = ମିଥ୍ୟା;
	encoder->total_document_size = 0;

	ଯଦି (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_init_buf_file(bson_encoder_t encoder, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = ମିଥ୍ୟା;
	encoder->realloc_ok = ମିଥ୍ୟା;
	encoder->total_document_size = 0;

	ଯଦି (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_init_buf(bson_encoder_t encoder, ଶୂନ୍ୟ *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	ଯଦି (encoder->buf == nullptr) {
		encoder->realloc_ok = ସତ୍ୟ;

	} ଅନ୍ୟଥା {
		encoder->bufsize = bufsize;
	}

	ଯଦି (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ଯଦି (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	ଯଦି (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = ::ଲିଖ(encoder->fd, encoder->buf, encoder->bufpos);

		ଯଦି (ret == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)encoder->bufpos) {
			encoder->total_document_size += ret;

		} ଅନ୍ୟଥା {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ଅଚଳ int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	ଯଦି (encoder->fd > -1) {
		ଯଦି ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::ଲିଖ(encoder->fd, &bson_doc_bytes, ଆକାର(bson_doc_bytes)) != ଆକାର(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} ଅନ୍ୟଥା ଯଦି (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, ଆକାର(bson_doc_bytes));
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ଯଦି (encoder->fd > -1) {
		ଫେରନ୍ତୁ -1;
	}

	ଫେରନ୍ତୁ encoder->bufpos;
}

ଶୂନ୍ୟ *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	ଯଦି (encoder->fd > -1) {
		ଫେରନ୍ତୁ nullptr;
	}

	ଫେରନ୍ତୁ encoder->buf;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା bson_encoder_append_bool(bson_encoder_t encoder, ଅଚଳ ଅକ୍ଷର *name, ତାର୍କିକ value)
{
	CODER_CHECK(encoder);

	ଯଦି (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_append_int32(bson_encoder_t encoder, ଅଚଳ ଅକ୍ଷର *name, int32_t value)
{
	CODER_CHECK(encoder);

	ଯଦି (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_append_int64(bson_encoder_t encoder, ଅଚଳ ଅକ୍ଷର *name, int64_t value)
{
	CODER_CHECK(encoder);

	ଯଦି (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_append_double(bson_encoder_t encoder, ଅଚଳ ଅକ୍ଷର *name, ଦ୍ୱିଗୁଣ value)
{
	CODER_CHECK(encoder);

	ଯଦି (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_append_string(bson_encoder_t encoder, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	ଯଦି (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_append_ଦ୍ୱିମିକ(bson_encoder_t encoder, ଅଚଳ ଅକ୍ଷର *name, bson_ଦ୍ୱିମିକ_subtype_t subtype, size_t size,
			   ଅଚଳ ଶୂନ୍ୟ *data)
{
	CODER_CHECK(encoder);

	ଯଦି (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bson_encoder_append_null(bson_encoder_t encoder, ଅଚଳ ଅକ୍ଷର *name)
{
	CODER_CHECK(encoder);

	ଯଦି (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	ଫେରନ୍ତୁ 0;
}
