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

#నిర్వచితం_కాకపోతే _TINYBSON_H
#నిర్వచించు _TINYBSON_H

#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <stdbool.h>

/** subset of the BSON node types we might care about */
రకనిర్వచనం గణన {
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

రకనిర్వచనం గణన bson_ద్విమాన_subtype {
	BSON_BIN_ద్విమాన = 0,
	BSON_BIN_USER = 128
} bson_ద్విమాన_subtype_t;

/**
 * Maximum node name length.
 */
#నిర్వచించు BSON_MAXNAME		32

/**
 * Buffer growth increment when writing to a buffer.
 */
#నిర్వచించు BSON_BUF_INCREMENT	128

/**
 * Node structure passed to the callback.
 */
రకనిర్వచనం నిర్మాణం bson_కణుపు_s {
	అక్షరం			name[BSON_MAXNAME];
	bson_type_t		type;
	bson_ద్విమాన_subtype_t	subtype;
	సంఘం {
		ద్విగుణ  d;   // bson type 1
		bool    b;   // bson type 8
		int32_t i32; // bson type 16
		int64_t i64; // bson type 18
	};
} *bson_కణుపు_t;

రకనిర్వచనం నిర్మాణం bson_decoder_s *bson_decoder_t;

/**
 * Node callback.
 *
 * The node callback function's return value is returned by bson_decoder_next.
 */
రకనిర్వచనం పూర్ణాంకం	(* bson_decoder_callback)(bson_decoder_t decoder, bson_కణుపు_t కణుపు_2);

నిర్మాణం bson_decoder_s {
	/* file reader state */
	పూర్ణాంకం			fd{-1};

	/* buffer reader state */
	uint8_t			*buf{nullptr};
	size_t			bufsize{0};
	గుర్తులేని		bufpos{0};

	bool			dead{false};
	bson_decoder_callback	callback;
	గుర్తులేని		nesting{0};
	నిర్మాణం bson_కణుపు_s	కణుపు_2 {};
	int32_t			pending{0};

	int32_t                 మొత్తం_document_కొలత{0};
	int32_t                 మొత్తం_decoded_కొలత{0};

	uint16_t                లెక్క_కణుపు_double{0};
	uint16_t                లెక్క_కణుపు_string{0};
	uint16_t                లెక్క_కణుపు_bindata{0};
	uint16_t                లెక్క_కణుపు_bool{0};
	uint16_t                లెక్క_కణుపు_int32{0};
	uint16_t                లెక్క_కణుపు_int64{0};
};

/**
 * Initialise the decoder to read from a file.
 *
 * @param decoder		Decoder state structure to be initialised.
 * @param fd			File to read BSON data from.
 * @param callback		Callback to be invoked by bson_decoder_next
 * @return			Zero on success.
 */
__EXPORT పూర్ణాంకం bson_decoder_init_file(bson_decoder_t decoder, పూర్ణాంకం fd, bson_decoder_callback callback);

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
__EXPORT పూర్ణాంకం bson_decoder_init_buf(bson_decoder_t decoder, శూన్య *buf, గుర్తులేని bufsize, bson_decoder_callback callback);

/**
 * Process the next node from the stream and invoke the callback.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 * @return			-1 if parsing encountered an error, 0 if the BSON stream has ended,
 *				otherwise the return value from the callback.
 */
__EXPORT పూర్ణాంకం bson_decoder_తదుపరి(bson_decoder_t decoder);

/**
 * Copy node data.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 */
__EXPORT పూర్ణాంకం bson_decoder_copy_దత్తాంశం(bson_decoder_t decoder, శూన్య *buf);

/**
 * Report copyable data size.
 *
 * @param decoder		Decoder state, must have been initialised with bson_decoder_init.
 */
__EXPORT size_t bson_decoder_దత్తాంశం_pending(bson_decoder_t decoder);

/**
 * Encoder state structure.
 */
రకనిర్వచనం నిర్మాణం bson_encoder_s {
	/* file writer state */
	పూర్ణాంకం		fd{-1};

	/* buffer writer state */
	uint8_t		*buf{nullptr};
	గుర్తులేని	bufsize{0};
	గుర్తులేని	bufpos{0};

	bool		realloc_ok{false};
	bool		dead{false};

	int32_t        మొత్తం_document_కొలత{0};

} *bson_encoder_t;

/**
 * Initialze the encoder for writing to a file.
 *
 * @param encoder		Encoder state structure to be initialised.
 * @param fd			File to write to.
 * @return			Zero on success.
 */
__EXPORT పూర్ణాంకం bson_encoder_init_file(bson_encoder_t encoder, పూర్ణాంకం fd);

/**
 * Initialze the encoder for writing to a file.
 *
 * @param encoder		Encoder state structure to be initialised.
 * @param fd			File to write to.
 * @param buf			Buffer pointer to use, can't be nullptr
 * @param bufsize		Supplied buffer size
 * @return			Zero on success.
 */
__EXPORT పూర్ణాంకం bson_encoder_init_buf_file(bson_encoder_t encoder, పూర్ణాంకం fd, శూన్య *buf, గుర్తులేని bufsize);

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
__EXPORT పూర్ణాంకం bson_encoder_init_buf(bson_encoder_t encoder, శూన్య *buf, గుర్తులేని bufsize);

/**
 * Finalise the encoded stream.
 *
 * @param encoder		The encoder to finalise.
 */
__EXPORT పూర్ణాంకం bson_encoder_fini(bson_encoder_t encoder);

/**
 * Fetch the size of the encoded object; only valid for buffer operations.
 */
__EXPORT పూర్ణాంకం bson_encoder_buf_కొలత(bson_encoder_t encoder);

/**
 * Get a pointer to the encoded object buffer.
 *
 * Note that if the buffer was allocated by the encoder, it is the caller's responsibility
 * to free this buffer.
 */
__EXPORT శూన్య *bson_encoder_buf_దత్తాంశం(bson_encoder_t encoder);

/**
 * Append a boolean to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT పూర్ణాంకం bson_encoder_append_bool(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, bool విలువ);

/**
 * Append an int32 to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT పూర్ణాంకం bson_encoder_append_int32(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, int32_t విలువ);

/**
 * Append an int64 to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT పూర్ణాంకం bson_encoder_append_int64(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, int64_t విలువ);

/**
 * Append a double to the encoded stream
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param value			Value to be encoded.
 */
__EXPORT పూర్ణాంకం bson_encoder_append_double(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, ద్విగుణ విలువ);

/**
 * Append a string to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param string		Nul-terminated C string.
 */
__EXPORT పూర్ణాంకం bson_encoder_append_string(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *string);

/**
 * Append a binary blob to the encoded stream.
 *
 * @param encoder		Encoder state.
 * @param name			Node name.
 * @param subtype		Binary data subtype.
 * @param size			Data size.
 * @param data			Buffer containing data to be encoded.
 */
__EXPORT పూర్ణాంకం bson_encoder_append_ద్విమాన(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name, bson_ద్విమాన_subtype_t subtype,
					size_t కొలత, స్థిరరాశి శూన్య *దత్తాంశం);

/**
 * Append a BSON null (type 0x0A). Name only; no value payload.
 */
__EXPORT పూర్ణాంకం bson_encoder_append_null(bson_encoder_t encoder, స్థిరరాశి అక్షరం *name);


#అయితే_ముగింపు
