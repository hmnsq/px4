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

#जर 0
# define debug(fmt, args...)		करा { PX4_INFO("BSON: " fmt, ##args); } पर्यंत(0)
#अन्यथा
# define debug(fmt, args...)		करा { } पर्यंत(0)
#endif

#define CODER_CHECK(_c)		करा { जर ((_c)->dead) { PX4_ERR("coder dead"); परत -1; }} पर्यंत(0)
#define CODER_KILL(_c, _reason)	करा { PX4_ERR("killed: %s", _reason); (_c)->dead = सत्य; परत -1; } पर्यंत(0)

स्थिर पूर्णांक
read_x(bson_decoder_t decoder, शून्य *p, माप_प्रकार s)
{
	CODER_CHECK(decoder);

	जर (decoder->fd > -1) {
		पूर्णांक ret = ::पठ(decoder->fd, p, s);

		जर (ret == s) {
			decoder->total_decoded_size += ret;
			परत 0;
		}

		परत -1;
	}

	जर (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		जर (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		जर ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		स_नकल(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		परत 0;
	}

	debug("no source");
	परत -1;
}

स्थिर पूर्णांक
read_int8(bson_decoder_t decoder, int8_t *b)
{
	परत read_x(decoder, b, आकार(*b));
}

स्थिर पूर्णांक
read_int32(bson_decoder_t decoder, int32_t *i)
{
	परत read_x(decoder, i, आकार(*i));
}

स्थिर पूर्णांक
read_int64(bson_decoder_t decoder, int64_t *i)
{
	परत read_x(decoder, i, आकार(*i));
}

स्थिर पूर्णांक
read_double(bson_decoder_t decoder, द्विगुण *d)
{
	परत read_x(decoder, d, आकार(*d));
}

पूर्णांक
bson_decoder_init_file(bson_decoder_t decoder, पूर्णांक fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->गाठ_2.type = BSON_UNDEFINED;

	// read document size
	जर (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	जर (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	परत 0;
}

पूर्णांक
bson_decoder_init_buf(bson_decoder_t decoder, शून्य *buf, अचिन्हित bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	जर ((buf == nullptr) || (callback == nullptr)) {
		परत -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = असत्य;

	जर (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} अन्यथा {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->गाठ_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	जर (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	जर (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	जर ((decoder->total_document_size > 0) && (decoder->total_document_size > (पूर्णांक)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	परत 0;
}

पूर्णांक
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	अचिन्हित nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	जर (decoder->गाठ_2.type == BSON_EOO) {
		जर (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		जर (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			परत 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	पर्यंत (decoder->pending > 0) {
		जर (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	जर (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->गाठ_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->गाठ_2.type);

	/* EOO is special; it has no name/data following */
	जर (decoder->गाठ_2.type == BSON_EOO) {
		decoder->गाठ_2.name[0] = '\0';

	} अन्यथा जर ((पूर्णांक)decoder->गाठ_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = सत्य;
		परत -ENODATA;

	} अन्यथा {

		/* get the node name */
		nlen = 0;

		साठी (;;) {
			जर (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->गाठ_2.type, decoder->गाठ_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			जर (read_int8(decoder, (int8_t *)&decoder->गाठ_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			जर (decoder->गाठ_2.name[nlen] == '\0') {
				थांबा;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->गाठ_2.name);

		निवड (decoder->गाठ_2.type) {
		स्थिती BSON_DOUBLE:
			जर (read_double(decoder, &decoder->गाठ_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->मोजणी_गाठ_double++;
			थांबा;

		स्थिती BSON_STRING:
			जर (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->मोजणी_गाठ_string++;
			थांबा;

		स्थिती BSON_BINDATA:
			जर (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			जर (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->गाठ_2.subtype = (bson_द्विमान_subtype_t)tbyte;
			decoder->मोजणी_गाठ_bindata++;
			थांबा;

		स्थिती BSON_BOOL:
			जर (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->गाठ_2.b = (tbyte != 0);
			decoder->मोजणी_गाठ_bool++;
			थांबा;

		स्थिती BSON_INT32:
			जर (read_int32(decoder, &decoder->गाठ_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->मोजणी_गाठ_int32++;
			थांबा;

		स्थिती BSON_INT64:
			जर (read_int64(decoder, &decoder->गाठ_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->मोजणी_गाठ_int64++;
			थांबा;

		स्थिती BSON_nullptr:
		स्थिती BSON_UNDEFINED:
			थांबा;

		/* XXX currently not supporting other types */
		पूर्वनिर्धारित:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	परत decoder->callback(decoder, &decoder->गाठ_2);
}

पूर्णांक
bson_decoder_copy_data(bson_decoder_t decoder, शून्य *buf)
{
	पूर्णांक result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	जर (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	परत 0;
}

माप_प्रकार
bson_decoder_data_pending(bson_decoder_t decoder)
{
	परत decoder->pending;
}

स्थिर पूर्णांक
write_x(bson_encoder_t encoder, स्थिरांक शून्य *p, माप_प्रकार s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	जर (encoder->fd > -1 && encoder->buf == nullptr) {
		पूर्णांक ret = ::लिख(encoder->fd, p, s);

		जर (ret == s) {
			encoder->total_document_size += ret;
			परत 0;
		}

		परत -1;
	}

	/* do we need to extend the buffer? */
	पर्यंत ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		जर (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			पूर्णांक ret = ::लिख(encoder->fd, encoder->buf, encoder->bufpos);

			जर (ret == (पूर्णांक)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				जर ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				थांबा;

			} अन्यथा {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, त्रुटि_सं, म_त्रुटि(त्रुटि_सं));
				CODER_KILL(encoder, "file write error");
			}
		}

		जर (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)पुनः_स्मृति(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		जर (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	स_नकल(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	परत 0;
}

स्थिर पूर्णांक
write_int8(bson_encoder_t encoder, int8_t b)
{
	परत write_x(encoder, &b, आकार(b));
}

स्थिर पूर्णांक
write_int32(bson_encoder_t encoder, int32_t i)
{
	परत write_x(encoder, &i, आकार(i));
}

स्थिर पूर्णांक
write_int64(bson_encoder_t encoder, int64_t i)
{
	परत write_x(encoder, &i, आकार(i));
}

स्थिर पूर्णांक
write_double(bson_encoder_t encoder, द्विगुण d)
{
	परत write_x(encoder, &d, आकार(d));
}

स्थिर पूर्णांक
write_name(bson_encoder_t encoder, स्थिरांक अक्षर *name)
{
	माप_प्रकार len = म_माप(name);

	जर (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	परत write_x(encoder, name, len + 1);
}

पूर्णांक
bson_encoder_init_file(bson_encoder_t encoder, पूर्णांक fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = असत्य;
	encoder->total_document_size = 0;

	जर (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	परत 0;
}

पूर्णांक
bson_encoder_init_buf_file(bson_encoder_t encoder, पूर्णांक fd, शून्य *buf, अचिन्हित bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = असत्य;
	encoder->realloc_ok = असत्य;
	encoder->total_document_size = 0;

	जर (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	परत 0;
}

पूर्णांक
bson_encoder_init_buf(bson_encoder_t encoder, शून्य *buf, अचिन्हित bufsize)
{
	encoder->buf = (uint8_t *)buf;

	जर (encoder->buf == nullptr) {
		encoder->realloc_ok = सत्य;

	} अन्यथा {
		encoder->bufsize = bufsize;
	}

	जर (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	परत 0;
}

पूर्णांक
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	जर (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	जर (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		पूर्णांक ret = ::लिख(encoder->fd, encoder->buf, encoder->bufpos);

		जर (ret == (पूर्णांक)encoder->bufpos) {
			encoder->total_document_size += ret;

		} अन्यथा {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	स्थिरांक int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	जर (encoder->fd > -1) {
		जर ((lseek(encoder->fd, 0, शुरू_से) != 0)
		    || (::लिख(encoder->fd, &bson_doc_bytes, आकार(bson_doc_bytes)) != आकार(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} अन्यथा जर (encoder->buf != nullptr) {
		/* update buffer length */
		स_नकल(encoder->buf, &bson_doc_bytes, आकार(bson_doc_bytes));
	}

	परत 0;
}

पूर्णांक
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	जर (encoder->fd > -1) {
		परत -1;
	}

	परत encoder->bufpos;
}

शून्य *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	जर (encoder->fd > -1) {
		परत nullptr;
	}

	परत encoder->buf;
}

पूर्णांक bson_encoder_append_bool(bson_encoder_t encoder, स्थिरांक अक्षर *name, तार्किक value)
{
	CODER_CHECK(encoder);

	जर (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	परत 0;
}

पूर्णांक
bson_encoder_append_int32(bson_encoder_t encoder, स्थिरांक अक्षर *name, int32_t value)
{
	CODER_CHECK(encoder);

	जर (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	परत 0;
}

पूर्णांक
bson_encoder_append_int64(bson_encoder_t encoder, स्थिरांक अक्षर *name, int64_t value)
{
	CODER_CHECK(encoder);

	जर (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	परत 0;
}

पूर्णांक
bson_encoder_append_double(bson_encoder_t encoder, स्थिरांक अक्षर *name, द्विगुण value)
{
	CODER_CHECK(encoder);

	जर (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	परत 0;
}

पूर्णांक
bson_encoder_append_string(bson_encoder_t encoder, स्थिरांक अक्षर *name, स्थिरांक अक्षर *string)
{
	माप_प्रकार len;

	CODER_CHECK(encoder);

	len = म_माप(string) + 1;	/* include trailing nul */

	जर (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	परत 0;
}

पूर्णांक
bson_encoder_append_द्विमान(bson_encoder_t encoder, स्थिरांक अक्षर *name, bson_द्विमान_subtype_t subtype, माप_प्रकार size,
			   स्थिरांक शून्य *data)
{
	CODER_CHECK(encoder);

	जर (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	परत 0;
}

पूर्णांक
bson_encoder_append_null(bson_encoder_t encoder, स्थिरांक अक्षर *name)
{
	CODER_CHECK(encoder);

	जर (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	परत 0;
}
