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

#ifndef _TINYBSON_H
#define _TINYBSON_H

#অন্তর্ভাব <sys/types.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <stdbool.h>

/** subset of the BSON node types we might care about */
প্রকারলেখ গণনা {
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

প্রকারলেখ গণনা bson_দ্বিমিক_subtype {
	BSON_BIN_দ্বিমিক = 0,
	BSON_BIN_USER = 128
} bson_দ্বিমিক_subtype_t;

/**
 * Maximum node name length.
 */
#define BSON_MAXNAME		32

/**
 * Buffer growth increment when writing to a buffer.
 */
#define BSON_BUF_INCREMENT	128

/**
 * Node structure passed to the callback.
 */
প্রকারলেখ কাঠামো bson_গ্রন্থি_s {
	অক্ষর			name[BSON_MAXNAME];
	bson_type_t		type;
	bson_দ্বিমিক_subtype_t	subtype;
	সংঘ {
		দ্বিগুণ  d;   // bson type 1
		bool    b;   // bson type 8
		int32_t i32; // bson type 16
		int64_t i64; // bson type 18
	};
} *bson_গ্রন্থি_t;

প্রকারলেখ কাঠামো bson_decoder_s *bson_decoder_t;

/**
 * Node callback.
 *
 * The node callback function's return value is returned by bson_decoder_next.
 */
প্রকারলেখ পূর্ণসংখ্যা	(* bson_decoder_callback)(bson_decoder_t decoder, bson_গ্রন্থি_t গ্রন্থি_2);

কাঠামো bson_decoder_s {
	/* file reader state */
	পূর্ণসংখ্যা			fd{-1};

	/* buffer reader state */
	uint8_t			*buf{nullptr};
	size_t			bufsize{0};
	unsigned		bufpos{0};

	bool			dead{false};
	bson_decoder_callback	callback;
	unsigned		nesting{0};
	কাঠামো bson_গ্রন্থি_s	গ্রন্থি_2 {};
	int32_t			pending{0};

	int32_t                 যোগ_document_আকার{0};
	int32_t                 যোগ_decoded_আকার{0};

	uint16_t                গুনতি_গ্রন্থি_double{0};
	uint16_t                গুনতি_গ্রন্থি_string{0};
	uint16_t                গুনতি_গ্রন্থি_bindata{0};
	uint16_t                গুনতি_গ্রন্থি_bool{0};
	uint16_t                গুনতি_গ্রন্থি_int32{0};
	uint16_t                গুনতি_গ্রন্থি_int64{0};
};

/**
 * Initialise the decoder to read from a file.
 *
 * @param decoder		Decoder state structure to be initialised.
 * @param fd			File to read BSON data from.
 * @param callback		Callback to be invoked by bson_decoder_next
 * @return			Zero on success.
 */
__EXPORT পূর্ণসংখ্যা bson_decoder_init_file(bson_decoder_t decoder, পূর্ণসংখ্যা fd, bson_decoder_callback callback);

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
__EXPORT পূর্ণসংখ্যা bson_decoder_init_buf(bson_decoder_t decoder, শূন্য *buf, unsigned bufsize, bson_decoder_callback callback);

/**
 * Process the next node from the stream and invoke the callback.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 * @return			-1 if parsing encountered an error, 0 if the BSON stream has ended,
 *				otherwise the return value from the callback.
 */
__EXPORT পূর্ণসংখ্যা bson_decoder_পরবর্তী(bson_decoder_t decoder);

/**
 * Copy node data.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 */
__EXPORT পূর্ণসংখ্যা bson_decoder_copy_তথ্য(bson_decoder_t decoder, শূন্য *buf);

/**
 * Report copyable data size.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 */
__EXPORT size_t bson_decoder_তথ্য_pending(bson_decoder_t decoder);

/**
 * Encoder state structure.
 */
প্রকারলেখ কাঠামো bson_encoder_s {
	/* file writer state */
	পূর্ণসংখ্যা		fd{-1};

	/* buffer writer state */
	uint8_t		*buf{nullptr};
	unsigned	bufsize{0};
	unsigned	bufpos{0};

	bool		realloc_ok{false};
	bool		dead{false};

	int32_t        যোগ_document_আকার{0};

} *bson_encoder_t;

/**
 * Initialze the encoder for writing to a file.
 *
 * @param encoder		Encoder state structure to be initialised.
 * @param fd			File to write to.
 * @return			Zero on success.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_init_file(bson_encoder_t encoder, পূর্ণসংখ্যা fd);

/**
 * Initialze the encoder for writing to a file.
 *
 * @param encoder		Encoder state structure to be initialised.
 * @param fd			File to write to.
 * @param buf			Buffer pointer to use, can't be nullptr
 * @param bufsize		Supplied buffer size
 * @return			Zero on success.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_init_buf_file(bson_encoder_t encoder, পূর্ণসংখ্যা fd, শূন্য *buf, unsigned bufsize);

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
__EXPORT পূর্ণসংখ্যা bson_encoder_init_buf(bson_encoder_t encoder, শূন্য *buf, unsigned bufsize);

/**
 * Finalise the encoded stream.
 *
 * @param encoder		The encoder to finalise.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_fini(bson_encoder_t encoder);

/**
 * Fetch the size of the encoded object; only valid for buffer operations.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_buf_আকার(bson_encoder_t encoder);

/**
 * Get a pointer to the encoded object buffer.
 *
 * Note that if the buffer was allocated by the encoder, it is the caller's responsibility
 * to free this buffer.
 */
__EXPORT শূন্য *bson_encoder_buf_তথ্য(bson_encoder_t encoder);

/**
 * Append a boolean to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_append_bool(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, bool মান);

/**
 * Append an int32 to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_append_int32(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, int32_t মান);

/**
 * Append an int64 to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_append_int64(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, int64_t মান);

/**
 * Append a double to the encoded stream
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_append_double(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, দ্বিগুণ মান);

/**
 * Append a string to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param string		Nul-terminated C string.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_append_string(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *string);

/**
 * Append a binary blob to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param subtype		Binary data subtype.
 * @param size			Data size.
 * @param data			Buffer containing data to be encoded.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_append_দ্বিমিক(bson_encoder_t encoder, ধ্রুবক অক্ষর *name, bson_দ্বিমিক_subtype_t subtype,
					size_t আকার_2, ধ্রুবক শূন্য *তথ্য);

/**
 * Append a BSON null (type 0x0A). Name only; no value payload.
 */
__EXPORT পূর্ণসংখ্যা bson_encoder_append_null(bson_encoder_t encoder, ধ্রুবক অক্ষর *name);


#endif
