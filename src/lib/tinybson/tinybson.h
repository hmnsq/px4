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
* @file tinybson.h
*
* A simple subset SAX-style BSON parser and generator. See http://bsonspec.org
*
* Some types and defines taken from the standalone BSON parser/generator
* in the Mongo C connector.
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _TINYBSON_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _TINYBSON_H

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>

/** subset of the BSON node types we might care about */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
	BSON_EOO       = 0,
	BSON_DOUBLE    = 1,
	BSON_STRING    = 2,
	BSON_OBJECT    = 3,
	BSON_ARRAY     = 4,
	BSON_BINDATA   = 5,
	BSON_UNDEFINED = 6,
	BSON_BOOL      = 8,
	BSON_DATE      = 9,
	BSON_nullptr   = 10,
	BSON_INT32     = 16,
	BSON_TIMESTAMP = 17,
	BSON_INT64     = 18
} bson_type_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ bson_ਦੁਆਧਾਰੀ_subtype {
	BSON_BIN_ਦੁਆਧਾਰੀ = 0,
	BSON_BIN_USER = 128
} bson_ਦੁਆਧਾਰੀ_subtype_t;

/**
 * Maximum node name length.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BSON_MAXNAME		32

/**
 * Buffer growth increment when writing to a buffer.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BSON_BUF_INCREMENT	128

/**
 * Node structure passed to the callback.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ bson_ਗੰਢ_s {
	ਅੱਖਰ			name[BSON_MAXNAME];
	bson_type_t		type;
	bson_ਦੁਆਧਾਰੀ_subtype_t	subtype;
	ਸੰਘ {
		ਦੁੱਗਣਾ  d;   // bson type 1
		bool    b;   // bson type 8
		int32_t i32; // bson type 16
		int64_t i64; // bson type 18
	};
} *bson_ਗੰਢ_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ bson_decoder_s *bson_decoder_t;

/**
 * Node callback.
 *
 * The node callback function's return value is returned by bson_decoder_next.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ	(* bson_decoder_callback)(bson_decoder_t decoder, bson_ਗੰਢ_t ਗੰਢ_2);

ਬਣਤਰ bson_decoder_s {
	/* file reader state */
	ਪੂਰਨ			fd{-1};

	/* buffer reader state */
	uint8_t			*buf{nullptr};
	size_t			bufsize{0};
	ਬਿਨਾਨਿਸ਼ਾਨੀ		bufpos{0};

	bool			dead{false};
	bson_decoder_callback	callback;
	ਬਿਨਾਨਿਸ਼ਾਨੀ		nesting{0};
	ਬਣਤਰ bson_ਗੰਢ_s	ਗੰਢ_2 {};
	int32_t			pending{0};

	int32_t                 ਕੁੱਲ_document_ਮਾਪ{0};
	int32_t                 ਕੁੱਲ_decoded_ਮਾਪ{0};

	uint16_t                ਗਿਣਤੀ_ਗੰਢ_double{0};
	uint16_t                ਗਿਣਤੀ_ਗੰਢ_string{0};
	uint16_t                ਗਿਣਤੀ_ਗੰਢ_bindata{0};
	uint16_t                ਗਿਣਤੀ_ਗੰਢ_bool{0};
	uint16_t                ਗਿਣਤੀ_ਗੰਢ_int32{0};
	uint16_t                ਗਿਣਤੀ_ਗੰਢ_int64{0};
};

/**
 * Initialise the decoder to read from a file.
 *
 * @param decoder		Decoder state structure to be initialised.
 * @param fd			File to read BSON data from.
 * @param callback		Callback to be invoked by bson_decoder_next
 * @return			Zero on success.
 */
__EXPORT ਪੂਰਨ bson_decoder_init_file(bson_decoder_t decoder, ਪੂਰਨ fd, bson_decoder_callback callback);

/**
 * Initialise the decoder to read from a buffer in memory.
 *
 * @param decoder		Decoder state structure to be initialised.
 * @param buf			Buffer to read from.
 * @param bufsize		Size of the buffer (BSON object may be smaller).  May be
 *				passed as zero if the buffer size should be extracted from the
 *				BSON header only.
 * @param callback		Callback to be invoked by bson_decoder_next
 * @return			Zero on success.
 */
__EXPORT ਪੂਰਨ bson_decoder_init_buf(bson_decoder_t decoder, ਖਾਲੀ *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ bufsize, bson_decoder_callback callback);

/**
 * Process the next node from the stream and invoke the callback.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 * @return			-1 if parsing encountered an error, 0 if the BSON stream has ended,
 *				otherwise the return value from the callback.
 */
__EXPORT ਪੂਰਨ bson_decoder_ਅਗਲਾ(bson_decoder_t decoder);

/**
 * Copy node data.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 */
__EXPORT ਪੂਰਨ bson_decoder_copy_ਅੰਕੜੇ(bson_decoder_t decoder, ਖਾਲੀ *buf);

/**
 * Report copyable data size.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 */
__EXPORT size_t bson_decoder_ਅੰਕੜੇ_pending(bson_decoder_t decoder);

/**
 * Encoder state structure.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ bson_encoder_s {
	/* file writer state */
	ਪੂਰਨ		fd{-1};

	/* buffer writer state */
	uint8_t		*buf{nullptr};
	ਬਿਨਾਨਿਸ਼ਾਨੀ	bufsize{0};
	ਬਿਨਾਨਿਸ਼ਾਨੀ	bufpos{0};

	bool		realloc_ok{false};
	bool		dead{false};

	int32_t        ਕੁੱਲ_document_ਮਾਪ{0};

} *bson_encoder_t;

/**
 * Initialze the encoder for writing to a file.
 *
 * @param encoder		Encoder state structure to be initialised.
 * @param fd			File to write to.
 * @return			Zero on success.
 */
__EXPORT ਪੂਰਨ bson_encoder_init_file(bson_encoder_t encoder, ਪੂਰਨ fd);

/**
 * Initialze the encoder for writing to a file.
 *
 * @param encoder		Encoder state structure to be initialised.
 * @param fd			File to write to.
 * @param buf			Buffer pointer to use, can't be nullptr
 * @param bufsize		Supplied buffer size
 * @return			Zero on success.
 */
__EXPORT ਪੂਰਨ bson_encoder_init_buf_file(bson_encoder_t encoder, ਪੂਰਨ fd, ਖਾਲੀ *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ bufsize);

/**
 * Initialze the encoder for writing to a buffer.
 *
 * @param encoder		Encoder state structure to be initialised.
 * @param buf			Buffer pointer to use, or nullptr if the buffer
 *				should be allocated by the encoder.
 * @param bufsize		Maximum buffer size, or zero for no limit. If
 *				the buffer is supplied, the size of the supplied buffer.
 * @return			Zero on success.
 */
__EXPORT ਪੂਰਨ bson_encoder_init_buf(bson_encoder_t encoder, ਖਾਲੀ *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ bufsize);

/**
 * Finalise the encoded stream.
 *
 * @param encoder		The encoder to finalise.
 */
__EXPORT ਪੂਰਨ bson_encoder_fini(bson_encoder_t encoder);

/**
 * Fetch the size of the encoded object; only valid for buffer operations.
 */
__EXPORT ਪੂਰਨ bson_encoder_buf_ਮਾਪ(bson_encoder_t encoder);

/**
 * Get a pointer to the encoded object buffer.
 *
 * Note that if the buffer was allocated by the encoder, it is the caller's responsibility
 * to free this buffer.
 */
__EXPORT ਖਾਲੀ *bson_encoder_buf_ਅੰਕੜੇ(bson_encoder_t encoder);

/**
 * Append a boolean to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT ਪੂਰਨ bson_encoder_append_bool(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, bool ਮੁੱਲ);

/**
 * Append an int32 to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT ਪੂਰਨ bson_encoder_append_int32(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, int32_t ਮੁੱਲ);

/**
 * Append an int64 to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT ਪੂਰਨ bson_encoder_append_int64(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, int64_t ਮੁੱਲ);

/**
 * Append a double to the encoded stream
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT ਪੂਰਨ bson_encoder_append_double(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, ਦੁੱਗਣਾ ਮੁੱਲ);

/**
 * Append a string to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param string		Nul-terminated C string.
 */
__EXPORT ਪੂਰਨ bson_encoder_append_string(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *string);

/**
 * Append a binary blob to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param subtype		Binary data subtype.
 * @param size			Data size.
 * @param data			Buffer containing data to be encoded.
 */
__EXPORT ਪੂਰਨ bson_encoder_append_ਦੁਆਧਾਰੀ(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, bson_ਦੁਆਧਾਰੀ_subtype_t subtype,
					size_t ਮਾਪ, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ);

/**
 * Append a BSON null (type 0x0A). Name only; no value payload.
 */
__EXPORT ਪੂਰਨ bson_encoder_append_null(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name);


#ਜੇ_ਅੰਤ
