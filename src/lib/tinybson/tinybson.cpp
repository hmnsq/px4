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

#అన్తర్భావ <px4_platform_common/posix.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <systemlib/err.h>

#అన్తర్భావ "tinybson.h"

#ఒకవేళ 0
# define debug(fmt, వాదనలు...)		చేయి { PX4_INFO("BSON: " fmt, ##args); } అయితే(0)
#లేకపోతే
# define debug(fmt, వాదనలు...)		చేయి { } అయితే(0)
#అయితే_ముగింపు

#నిర్వచించు CODER_CHECK(_c)		చేయి { ఒకవేళ ((_c)->dead) { PX4_ERR("coder dead"); తిరిగి -1; }} అయితే(0)
#నిర్వచించు CODER_KILL(_c, _reason)	చేయి { PX4_ERR("killed: %s", _reason); (_c)->dead = నిజం; తిరిగి -1; } అయితే(0)

స్థిర పూర్ణాంకం
read_x(bson_decoder_t decoder, శూన్య *p, size_t s)
{
	CODER_CHECK(decoder);

	ఒకవేళ (decoder->fd > -1) {
		పూర్ణాంకం ret = ::పఠ(decoder->fd, p, s);

		ఒకవేళ (ret == s) {
			decoder->మొత్తం_decoded_కొలత += ret;
			తిరిగి 0;
		}

		తిరిగి -1;
	}

	ఒకవేళ (decoder->buf != శూన్యసూచిక) {
		/* staged operations to avoid integer overflow for corrupt data */
		ఒకవేళ (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		ఒకవేళ ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		జ్ఞాపకం_నకలు(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->మొత్తం_decoded_కొలత += s;
		తిరిగి 0;
	}

	debug("no source");
	తిరిగి -1;
}

స్థిర పూర్ణాంకం
read_int8(bson_decoder_t decoder, int8_t *b)
{
	తిరిగి read_x(decoder, b, పరిమాణం(*b));
}

స్థిర పూర్ణాంకం
read_int32(bson_decoder_t decoder, int32_t *i)
{
	తిరిగి read_x(decoder, i, పరిమాణం(*i));
}

స్థిర పూర్ణాంకం
read_int64(bson_decoder_t decoder, int64_t *i)
{
	తిరిగి read_x(decoder, i, పరిమాణం(*i));
}

స్థిర పూర్ణాంకం
read_double(bson_decoder_t decoder, ద్విగుణ *d)
{
	తిరిగి read_x(decoder, d, పరిమాణం(*d));
}

పూర్ణాంకం
bson_decoder_init_file(bson_decoder_t decoder, పూర్ణాంకం fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->కణుపు_2.type = BSON_UNDEFINED;

	// read document size
	ఒకవేళ (read_int32(decoder, &decoder->మొత్తం_document_కొలత)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->మొత్తం_document_కొలత);

	// The length is signed and comes from the document itself
	ఒకవేళ (decoder->మొత్తం_document_కొలత < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	తిరిగి 0;
}

పూర్ణాంకం
bson_decoder_init_buf(bson_decoder_t decoder, శూన్య *buf, గుర్తులేని bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	ఒకవేళ ((buf == శూన్యసూచిక) || (callback == శూన్యసూచిక)) {
		తిరిగి -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = అబద్ధం;

	ఒకవేళ (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} లేకపోతే {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->కణుపు_2.type = BSON_UNDEFINED;
	decoder->మొత్తం_decoded_కొలత = 0;

	// read document size
	decoder->మొత్తం_document_కొలత = 0;

	ఒకవేళ (read_int32(decoder, &decoder->మొత్తం_document_కొలత)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->మొత్తం_document_కొలత);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	ఒకవేళ (decoder->మొత్తం_document_కొలత < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	ఒకవేళ ((decoder->మొత్తం_document_కొలత > 0) && (decoder->మొత్తం_document_కొలత > (పూర్ణాంకం)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	తిరిగి 0;
}

పూర్ణాంకం
bson_decoder_తదుపరి(bson_decoder_t decoder)
{
	int8_t	tbyte;
	గుర్తులేని nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	ఒకవేళ (decoder->కణుపు_2.type == BSON_EOO) {
		ఒకవేళ (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		ఒకవేళ (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			తిరిగి 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	అయితే (decoder->pending > 0) {
		ఒకవేళ (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	ఒకవేళ (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->కణుపు_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->కణుపు_2.type);

	/* EOO is special; it has no name/data following */
	ఒకవేళ (decoder->కణుపు_2.type == BSON_EOO) {
		decoder->కణుపు_2.name[0] = '\0';

	} లేకపోతే ఒకవేళ ((పూర్ణాంకం)decoder->కణుపు_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = నిజం;
		తిరిగి -ENODATA;

	} లేకపోతే {

		/* get the node name */
		nlen = 0;

		కొరకు (;;) {
			ఒకవేళ (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->కణుపు_2.type, decoder->కణుపు_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			ఒకవేళ (read_int8(decoder, (int8_t *)&decoder->కణుపు_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			ఒకవేళ (decoder->కణుపు_2.name[nlen] == '\0') {
				ఆపు;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->కణుపు_2.name);

		ఎంపిక (decoder->కణుపు_2.type) {
		స్థితి BSON_DOUBLE:
			ఒకవేళ (read_double(decoder, &decoder->కణుపు_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->లెక్క_కణుపు_double++;
			ఆపు;

		స్థితి BSON_STRING:
			ఒకవేళ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->లెక్క_కణుపు_string++;
			ఆపు;

		స్థితి BSON_BINDATA:
			ఒకవేళ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			ఒకవేళ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->కణుపు_2.subtype = (bson_ద్విమాన_subtype_t)tbyte;
			decoder->లెక్క_కణుపు_bindata++;
			ఆపు;

		స్థితి BSON_BOOL:
			ఒకవేళ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->కణుపు_2.b = (tbyte != 0);
			decoder->లెక్క_కణుపు_bool++;
			ఆపు;

		స్థితి BSON_INT32:
			ఒకవేళ (read_int32(decoder, &decoder->కణుపు_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->లెక్క_కణుపు_int32++;
			ఆపు;

		స్థితి BSON_INT64:
			ఒకవేళ (read_int64(decoder, &decoder->కణుపు_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->లెక్క_కణుపు_int64++;
			ఆపు;

		స్థితి BSON_nullptr:
		స్థితి BSON_UNDEFINED:
			ఆపు;

		/* XXX currently not supporting other types */
		అప్రమేయ:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	తిరిగి decoder->callback(decoder, &decoder->కణుపు_2);
}

పూర్ణాంకం
bson_decoder_copy_దత్తాంశం(bson_decoder_t decoder, శూన్య *buf)
{
	పూర్ణాంకం ఫలితం;

	CODER_CHECK(decoder);

	/* copy data */
	ఫలితం = read_x(decoder, buf, decoder->pending);

	ఒకవేళ (ఫలితం != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	తిరిగి 0;
}

size_t
bson_decoder_దత్తాంశం_pending(bson_decoder_t decoder)
{
	తిరిగి decoder->pending;
}

స్థిర పూర్ణాంకం
write_x(bson_encoder_t encoder, స్థిరరాశి శూన్య *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	ఒకవేళ (encoder->fd > -1 && encoder->buf == శూన్యసూచిక) {
		పూర్ణాంకం ret = ::లిఖ(encoder->fd, p, s);

		ఒకవేళ (ret == s) {
			encoder->మొత్తం_document_కొలత += ret;
			తిరిగి 0;
		}

		తిరిగి -1;
	}

	/* do we need to extend the buffer? */
	అయితే ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		ఒకవేళ (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			పూర్ణాంకం ret = ::లిఖ(encoder->fd, encoder->buf, encoder->bufpos);

			ఒకవేళ (ret == (పూర్ణాంకం)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->మొత్తం_document_కొలత += ret;

				ఒకవేళ ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				ఆపు;

			} లేకపోతే {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		ఒకవేళ (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		ఒకవేళ (newbuf == శూన్యసూచిక) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	జ్ఞాపకం_నకలు(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం
write_int8(bson_encoder_t encoder, int8_t b)
{
	తిరిగి write_x(encoder, &b, పరిమాణం(b));
}

స్థిర పూర్ణాంకం
write_int32(bson_encoder_t encoder, int32_t i)
{
	తిరిగి write_x(encoder, &i, పరిమాణం(i));
}

స్థిర పూర్ణాంకం
write_int64(bson_encoder_t encoder, int64_t i)
{
	తిరిగి write_x(encoder, &i, పరిమాణం(i));
}

స్థిర పూర్ణాంకం
write_double(bson_encoder_t encoder, ద్విగుణ d)
{
	తిరిగి write_x(encoder, &d, పరిమాణం(d));
}

స్థిర పూర్ణాంకం
write_name(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name)
{
	size_t len = అక్షరశ్రేణి_పొడవు(name);

	ఒకవేళ (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	తిరిగి write_x(encoder, name, len + 1);
}

పూర్ణాంకం
bson_encoder_init_file(bson_encoder_t encoder, పూర్ణాంకం fd)
{
	encoder->fd = fd;
	encoder->buf = శూన్యసూచిక;
	encoder->dead = అబద్ధం;
	encoder->మొత్తం_document_కొలత = 0;

	ఒకవేళ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_init_buf_file(bson_encoder_t encoder, పూర్ణాంకం fd, శూన్య *buf, గుర్తులేని bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = అబద్ధం;
	encoder->realloc_ok = అబద్ధం;
	encoder->మొత్తం_document_కొలత = 0;

	ఒకవేళ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_init_buf(bson_encoder_t encoder, శూన్య *buf, గుర్తులేని bufsize)
{
	encoder->buf = (uint8_t *)buf;

	ఒకవేళ (encoder->buf == శూన్యసూచిక) {
		encoder->realloc_ok = నిజం;

	} లేకపోతే {
		encoder->bufsize = bufsize;
	}

	ఒకవేళ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ఒకవేళ (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	ఒకవేళ (encoder->fd > -1 && encoder->buf != శూన్యసూచిక && encoder->bufpos > 0) {
		/* write final buffer to disk */
		పూర్ణాంకం ret = ::లిఖ(encoder->fd, encoder->buf, encoder->bufpos);

		ఒకవేళ (ret == (పూర్ణాంకం)encoder->bufpos) {
			encoder->మొత్తం_document_కొలత += ret;

		} లేకపోతే {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	స్థిరరాశి int32_t bson_doc_బైట్లు = (encoder->fd > -1) ? encoder->మొత్తం_document_కొలత : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_బైట్లు);

	ఒకవేళ (encoder->fd > -1) {
		ఒకవేళ ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::లిఖ(encoder->fd, &bson_doc_బైట్లు, పరిమాణం(bson_doc_బైట్లు)) != పరిమాణం(bson_doc_బైట్లు))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} లేకపోతే ఒకవేళ (encoder->buf != శూన్యసూచిక) {
		/* update buffer length */
		జ్ఞాపకం_నకలు(encoder->buf, &bson_doc_బైట్లు, పరిమాణం(bson_doc_బైట్లు));
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_buf_కొలత(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ఒకవేళ (encoder->fd > -1) {
		తిరిగి -1;
	}

	తిరిగి encoder->bufpos;
}

శూన్య *
bson_encoder_buf_దత్తాంశం(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	ఒకవేళ (encoder->fd > -1) {
		తిరిగి శూన్యసూచిక;
	}

	తిరిగి encoder->buf;
}

పూర్ణాంకం bson_encoder_append_bool(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, తార్కిక value)
{
	CODER_CHECK(encoder);

	ఒకవేళ (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_append_int32(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, int32_t value)
{
	CODER_CHECK(encoder);

	ఒకవేళ (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_append_int64(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, int64_t value)
{
	CODER_CHECK(encoder);

	ఒకవేళ (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_append_double(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, ద్విగుణ value)
{
	CODER_CHECK(encoder);

	ఒకవేళ (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_append_string(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = అక్షరశ్రేణి_పొడవు(string) + 1;	/* include trailing nul */

	ఒకవేళ (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_append_ద్విమాన(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, bson_ద్విమాన_subtype_t subtype, size_t కొలత,
			   స్థిరరాశి శూన్య *దత్తాంశం)
{
	CODER_CHECK(encoder);

	ఒకవేళ (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, కొలత) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, దత్తాంశం, కొలత)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	తిరిగి 0;
}

పూర్ణాంకం
bson_encoder_append_null(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name)
{
	CODER_CHECK(encoder);

	ఒకవేళ (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	తిరిగి 0;
}
