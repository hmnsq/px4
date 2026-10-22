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

#അന്തര്ഭാവ <px4_platform_common/posix.h>
#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <systemlib/err.h>

#അന്തര്ഭാവ "tinybson.h"

#എങ്കിൽ 0
# define debug(fmt, args...)		ചെയ്യുക { PX4_INFO("BSON: " fmt, ##args); } ആകുമ്പോൾ(0)
#അല്ലെങ്കിൽ
# define debug(fmt, args...)		ചെയ്യുക { } ആകുമ്പോൾ(0)
#endif

#define CODER_CHECK(_c)		ചെയ്യുക { എങ്കിൽ ((_c)->dead) { PX4_ERR("coder dead"); തിരികെ -1; }} ആകുമ്പോൾ(0)
#define CODER_KILL(_c, _reason)	ചെയ്യുക { PX4_ERR("killed: %s", _reason); (_c)->dead = സത്യം; തിരികെ -1; } ആകുമ്പോൾ(0)

സ്ഥിതം പൂർണ്ണസംഖ്യ
read_x(bson_decoder_t decoder, ശൂന്യം *p, size_t s)
{
	CODER_CHECK(decoder);

	എങ്കിൽ (decoder->fd > -1) {
		പൂർണ്ണസംഖ്യ ret = ::പഠ(decoder->fd, p, s);

		എങ്കിൽ (ret == s) {
			decoder->total_decoded_size += ret;
			തിരികെ 0;
		}

		തിരികെ -1;
	}

	എങ്കിൽ (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		എങ്കിൽ (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		എങ്കിൽ ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		തിരികെ 0;
	}

	debug("no source");
	തിരികെ -1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
read_int8(bson_decoder_t decoder, int8_t *b)
{
	തിരികെ read_x(decoder, b, വലുപ്പം(*b));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
read_int32(bson_decoder_t decoder, int32_t *i)
{
	തിരികെ read_x(decoder, i, വലുപ്പം(*i));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
read_int64(bson_decoder_t decoder, int64_t *i)
{
	തിരികെ read_x(decoder, i, വലുപ്പം(*i));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
read_double(bson_decoder_t decoder, ഇരട്ട *d)
{
	തിരികെ read_x(decoder, d, വലുപ്പം(*d));
}

പൂർണ്ണസംഖ്യ
bson_decoder_init_file(bson_decoder_t decoder, പൂർണ്ണസംഖ്യ fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->കണ്ണി_2.type = BSON_UNDEFINED;

	// read document size
	എങ്കിൽ (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	എങ്കിൽ (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_decoder_init_buf(bson_decoder_t decoder, ശൂന്യം *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	എങ്കിൽ ((buf == nullptr) || (callback == nullptr)) {
		തിരികെ -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = അസത്യം;

	എങ്കിൽ (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} അല്ലെങ്കിൽ {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->കണ്ണി_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	എങ്കിൽ (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	എങ്കിൽ (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	എങ്കിൽ ((decoder->total_document_size > 0) && (decoder->total_document_size > (പൂർണ്ണസംഖ്യ)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	എങ്കിൽ (decoder->കണ്ണി_2.type == BSON_EOO) {
		എങ്കിൽ (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		എങ്കിൽ (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			തിരികെ 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	ആകുമ്പോൾ (decoder->pending > 0) {
		എങ്കിൽ (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	എങ്കിൽ (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->കണ്ണി_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->കണ്ണി_2.type);

	/* EOO is special; it has no name/data following */
	എങ്കിൽ (decoder->കണ്ണി_2.type == BSON_EOO) {
		decoder->കണ്ണി_2.name[0] = '\0';

	} അല്ലെങ്കിൽ എങ്കിൽ ((പൂർണ്ണസംഖ്യ)decoder->കണ്ണി_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = സത്യം;
		തിരികെ -ENODATA;

	} അല്ലെങ്കിൽ {

		/* get the node name */
		nlen = 0;

		വേണ്ടി (;;) {
			എങ്കിൽ (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->കണ്ണി_2.type, decoder->കണ്ണി_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			എങ്കിൽ (read_int8(decoder, (int8_t *)&decoder->കണ്ണി_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			എങ്കിൽ (decoder->കണ്ണി_2.name[nlen] == '\0') {
				നിർത്തുക;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->കണ്ണി_2.name);

		തിരഞ്ഞെടുപ്പ് (decoder->കണ്ണി_2.type) {
		അവസ്ഥ BSON_DOUBLE:
			എങ്കിൽ (read_double(decoder, &decoder->കണ്ണി_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->എണ്ണം_കണ്ണി_double++;
			നിർത്തുക;

		അവസ്ഥ BSON_STRING:
			എങ്കിൽ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->എണ്ണം_കണ്ണി_string++;
			നിർത്തുക;

		അവസ്ഥ BSON_BINDATA:
			എങ്കിൽ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			എങ്കിൽ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->കണ്ണി_2.subtype = (bson_ദ്വിമാന_subtype_t)tbyte;
			decoder->എണ്ണം_കണ്ണി_bindata++;
			നിർത്തുക;

		അവസ്ഥ BSON_BOOL:
			എങ്കിൽ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->കണ്ണി_2.b = (tbyte != 0);
			decoder->എണ്ണം_കണ്ണി_bool++;
			നിർത്തുക;

		അവസ്ഥ BSON_INT32:
			എങ്കിൽ (read_int32(decoder, &decoder->കണ്ണി_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->എണ്ണം_കണ്ണി_int32++;
			നിർത്തുക;

		അവസ്ഥ BSON_INT64:
			എങ്കിൽ (read_int64(decoder, &decoder->കണ്ണി_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->എണ്ണം_കണ്ണി_int64++;
			നിർത്തുക;

		അവസ്ഥ BSON_nullptr:
		അവസ്ഥ BSON_UNDEFINED:
			നിർത്തുക;

		/* XXX currently not supporting other types */
		സ്ഥിരസ്ഥിതി:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	തിരികെ decoder->callback(decoder, &decoder->കണ്ണി_2);
}

പൂർണ്ണസംഖ്യ
bson_decoder_copy_data(bson_decoder_t decoder, ശൂന്യം *buf)
{
	പൂർണ്ണസംഖ്യ result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	എങ്കിൽ (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	തിരികെ 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	തിരികെ decoder->pending;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
write_x(bson_encoder_t encoder, സ്ഥിരം ശൂന്യം *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	എങ്കിൽ (encoder->fd > -1 && encoder->buf == nullptr) {
		പൂർണ്ണസംഖ്യ ret = ::ലിഖ(encoder->fd, p, s);

		എങ്കിൽ (ret == s) {
			encoder->total_document_size += ret;
			തിരികെ 0;
		}

		തിരികെ -1;
	}

	/* do we need to extend the buffer? */
	ആകുമ്പോൾ ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		എങ്കിൽ (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			പൂർണ്ണസംഖ്യ ret = ::ലിഖ(encoder->fd, encoder->buf, encoder->bufpos);

			എങ്കിൽ (ret == (പൂർണ്ണസംഖ്യ)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				എങ്കിൽ ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				നിർത്തുക;

			} അല്ലെങ്കിൽ {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		എങ്കിൽ (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		എങ്കിൽ (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
write_int8(bson_encoder_t encoder, int8_t b)
{
	തിരികെ write_x(encoder, &b, വലുപ്പം(b));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
write_int32(bson_encoder_t encoder, int32_t i)
{
	തിരികെ write_x(encoder, &i, വലുപ്പം(i));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
write_int64(bson_encoder_t encoder, int64_t i)
{
	തിരികെ write_x(encoder, &i, വലുപ്പം(i));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
write_double(bson_encoder_t encoder, ഇരട്ട d)
{
	തിരികെ write_x(encoder, &d, വലുപ്പം(d));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
write_name(bson_encoder_t encoder, സ്ഥിരം അക്ഷരം *name)
{
	size_t len = strlen(name);

	എങ്കിൽ (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	തിരികെ write_x(encoder, name, len + 1);
}

പൂർണ്ണസംഖ്യ
bson_encoder_init_file(bson_encoder_t encoder, പൂർണ്ണസംഖ്യ fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = അസത്യം;
	encoder->total_document_size = 0;

	എങ്കിൽ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_init_buf_file(bson_encoder_t encoder, പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = അസത്യം;
	encoder->realloc_ok = അസത്യം;
	encoder->total_document_size = 0;

	എങ്കിൽ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_init_buf(bson_encoder_t encoder, ശൂന്യം *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	എങ്കിൽ (encoder->buf == nullptr) {
		encoder->realloc_ok = സത്യം;

	} അല്ലെങ്കിൽ {
		encoder->bufsize = bufsize;
	}

	എങ്കിൽ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	എങ്കിൽ (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	എങ്കിൽ (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		പൂർണ്ണസംഖ്യ ret = ::ലിഖ(encoder->fd, encoder->buf, encoder->bufpos);

		എങ്കിൽ (ret == (പൂർണ്ണസംഖ്യ)encoder->bufpos) {
			encoder->total_document_size += ret;

		} അല്ലെങ്കിൽ {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	സ്ഥിരം int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	എങ്കിൽ (encoder->fd > -1) {
		എങ്കിൽ ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::ലിഖ(encoder->fd, &bson_doc_bytes, വലുപ്പം(bson_doc_bytes)) != വലുപ്പം(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} അല്ലെങ്കിൽ എങ്കിൽ (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, വലുപ്പം(bson_doc_bytes));
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	എങ്കിൽ (encoder->fd > -1) {
		തിരികെ -1;
	}

	തിരികെ encoder->bufpos;
}

ശൂന്യം *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	എങ്കിൽ (encoder->fd > -1) {
		തിരികെ nullptr;
	}

	തിരികെ encoder->buf;
}

പൂർണ്ണസംഖ്യ bson_encoder_append_bool(bson_encoder_t encoder, സ്ഥിരം അക്ഷരം *name, തർക്കം value)
{
	CODER_CHECK(encoder);

	എങ്കിൽ (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_append_int32(bson_encoder_t encoder, സ്ഥിരം അക്ഷരം *name, int32_t value)
{
	CODER_CHECK(encoder);

	എങ്കിൽ (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_append_int64(bson_encoder_t encoder, സ്ഥിരം അക്ഷരം *name, int64_t value)
{
	CODER_CHECK(encoder);

	എങ്കിൽ (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_append_double(bson_encoder_t encoder, സ്ഥിരം അക്ഷരം *name, ഇരട്ട value)
{
	CODER_CHECK(encoder);

	എങ്കിൽ (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_append_string(bson_encoder_t encoder, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	എങ്കിൽ (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_append_ദ്വിമാന(bson_encoder_t encoder, സ്ഥിരം അക്ഷരം *name, bson_ദ്വിമാന_subtype_t subtype, size_t size,
			   സ്ഥിരം ശൂന്യം *data)
{
	CODER_CHECK(encoder);

	എങ്കിൽ (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
bson_encoder_append_null(bson_encoder_t encoder, സ്ഥിരം അക്ഷരം *name)
{
	CODER_CHECK(encoder);

	എങ്കിൽ (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	തിരികെ 0;
}
