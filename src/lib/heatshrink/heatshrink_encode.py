आनयन ctypes
तः enum आनयन Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
वग्ग HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_अन्वेषणम् = 2
    HSES_YIELD_TAG_BIT = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_INDEX = 5
    HSES_YIELD_BR_LENGTH = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_BITS = 8
    HSES_DONE = 9


# Constants
FLAG_IS_FINISHING = 0x01
MATCH_NOT_FOUND = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
वग्ग output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    प्रक्षिप_bits(hse, 1, tag, oi)


def प्रक्षिप_bits(hse, गणनम्, bits, oi):
    प्रतिज्ञा गणनम् <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    अत्थाय i अन्तः range(गणनम् - 1, -1, -1):
        bit = bits & (1 << i)
        सचे bit:
            current_byte.मानम् |= bit_index.मानम्
        bit_index.मानम् >>= 1
        सचे bit_index.मानम् == 0:
            bit_index.मानम् = 0x80
            oi.buf[oi.output_size[0]] = current_byte.मानम्
            oi.output_size[0] += 1
            current_byte.मानम् = 0
    hse.current_byte = current_byte.मानम्
    hse.bit_index = bit_index.मानम्


def प्रक्षिप_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    निवेशः_offset = get_निवेशः_offset(hse) + processed_offset
    c = hse.buffer[निवेशः_offset]
    प्रक्षिप_bits(hse, 8, c, oi)


# Define necessary structures and enums
वग्ग heatshrink_encoder(ctypes.Structure):
    _fields_ = [
        ("input_size", ctypes.c_uint16),
        ("match_scan_index", ctypes.c_uint16),
        ("match_length", ctypes.c_uint16),
        ("match_pos", ctypes.c_uint16),
        ("outgoing_bits", ctypes.c_uint16),
        ("outgoing_bits_count", ctypes.c_uint8),
        ("flags", ctypes.c_uint8),
        ("state", ctypes.c_uint8),
        ("current_byte", ctypes.c_uint8),
        ("bit_index", ctypes.c_uint8),

        ("buffer", ctypes.POINTER(ctypes.c_uint8)),
    ]

    def __init__(self, window_size=8, lookahead_size=4):
        पर().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.अन्वेषणम्_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


वग्ग hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        पर().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


वग्ग HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


वग्ग HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


वग्ग HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    पटिनिवत्त hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    पटिनिवत्त oi.output_size[0] < oi.buf_size


def get_निवेशः_buffer_size(hse):
    पटिनिवत्त 1 << hse.window_size


def get_lookahead_size(hse):
    पटिनिवत्त 1 << hse.lookahead_size


def get_निवेशः_offset(hse):
    पटिनिवत्त get_निवेशः_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, निवेशः_size):
    सचे hse is None वा in_buf is None वा निवेशः_size is None:
        पटिनिवत्त HSE_sink_res.HSER_SINK_ERROR_NULL

    सचे is_finishing(hse):
        पटिनिवत्त HSE_sink_res.HSER_SINK_ERROR_MISUSE

    सचे hse.state != HSE_state.HSES_NOT_FULL:
        पटिनिवत्त HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_निवेशः_offset(hse) + hse.निवेशः_size
    ibs = get_निवेशः_buffer_size(hse)
    rem = ibs - hse.निवेशः_size
    cp_sz = min(rem, size)

    अत्थाय i अन्तः range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    निवेशः_size.मानम् = cp_sz
    hse.निवेशः_size += cp_sz

    सचे cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    पटिनिवत्त HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.अन्वेषणम्_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    निवेशः_offset = get_निवेशः_offset(hse)
    end = निवेशः_offset + hse.निवेशः_size

    अत्थाय i अन्तः range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    सचे hse is None वा out_buf is None वा output_size is None:
        पटिनिवत्त HSE_poll_res.HSER_POLL_ERROR_NULL

    सचे out_buf_size == 0:
        पटिनिवत्त HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    याव सच्च:
        in_state = hse.state
        सचे in_state == HSE_state.HSES_NOT_FULL:
            पटिनिवत्त HSE_poll_res.HSER_POLL_EMPTY
        नोचेद्यदि in_state == HSE_state.HSES_DONE:
            पटिनिवत्त HSE_poll_res.HSER_POLL_EMPTY
        नोचेद्यदि in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_अन्वेषणम्
        नोचेद्यदि in_state == HSE_state.HSES_अन्वेषणम्:
            hse.state = st_step_अन्वेषणम्(hse)
        नोचेद्यदि in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        नोचेद्यदि in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        नोचेद्यदि in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        नोचेद्यदि in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        नोचेद्यदि in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        नोचेद्यदि in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        अञ्ञथा:
            पटिनिवत्त HSE_poll_res.HSER_POLL_ERROR_MISUSE

        सचे hse.state == in_state:
            सचे oi.output_size == oi.buf_size:
                पटिनिवत्त HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    सचे hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    सचे hse.state == HSE_state.HSES_DONE:
        पटिनिवत्त HSE_finish_res.HSER_FINISH_DONE
    पटिनिवत्त HSE_finish_res.HSER_FINISH_MORE


def st_step_अन्वेषणम्(hse):
    window_length = get_निवेशः_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    सचे msi > hse.निवेशः_size - (1 सचे fin अञ्ञथा lookahead_sz):
        पटिनिवत्त HSE_state.HSES_FLUSH_BITS सचे fin अञ्ञथा HSE_state.HSES_SAVE_BACKLOG

    निवेशः_offset = get_निवेशः_offset(hse)
    end = निवेशः_offset + msi
    start = end - window_length

    max_possible = lookahead_sz सचे hse.निवेशः_size - msi >= lookahead_sz अञ्ञथा hse.निवेशः_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    सचे match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        पटिनिवत्त HSE_state.HSES_YIELD_TAG_BIT
    अञ्ञथा:
        hse.match_pos = match_pos
        hse.match_length = match_length
        पटिनिवत्त HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.अन्वेषणम्_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    याव pos >= start:

        सचे buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.अन्वेषणम्_index.index[pos]
            अनुवर्त

        length = 1
        अत्थाय length अन्तः range(1, maxlen):
            सचे buf[pos + length] != buf[needlepoint + length]:
                भङ्ग
        सचे length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            सचे length == maxlen:
                भङ्ग  # won't find better

        pos = hse.अन्वेषणम्_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    सचे match_maxlen > (break_even_point // 8):
        पटिनिवत्त end - match_index, match_maxlen
    पटिनिवत्त MATCH_NOT_FOUND, 0


def प्रक्षिप_outgoing_bits(hse, oi):
    सचे hse.outgoing_bits_गणनम् > 8:
        गणनम् = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_गणनम् - 8)
    अञ्ञथा:
        गणनम् = hse.outgoing_bits_गणनम्
        bits = hse.outgoing_bits

    सचे गणनम् > 0:
        प्रक्षिप_bits(hse, गणनम्, bits, oi)
        hse.outgoing_bits_गणनम् -= गणनम्
    पटिनिवत्त गणनम्


def st_yield_tag_bit(hse, oi):
    सचे can_take_byte(oi):
        सचे hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            पटिनिवत्त HSE_state.HSES_YIELD_LITERAL
        अञ्ञथा:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_गणनम् = hse.window_size
            पटिनिवत्त HSE_state.HSES_YIELD_BR_INDEX
    अञ्ञथा:
        पटिनिवत्त HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    सचे can_take_byte(oi):
        प्रक्षिप_literal_byte(hse, oi)
        पटिनिवत्त HSE_state.HSES_अन्वेषणम्
    अञ्ञथा:
        पटिनिवत्त HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    सचे can_take_byte(oi):
        सचे प्रक्षिप_outgoing_bits(hse, oi) > 0:
            पटिनिवत्त HSE_state.HSES_YIELD_BR_INDEX
        अञ्ञथा:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_गणनम् = hse.lookahead_size
            पटिनिवत्त HSE_state.HSES_YIELD_BR_LENGTH
    अञ्ञथा:
        पटिनिवत्त HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    सचे can_take_byte(oi):
        सचे प्रक्षिप_outgoing_bits(hse, oi) > 0:
            पटिनिवत्त HSE_state.HSES_YIELD_BR_LENGTH
        अञ्ञथा:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            पटिनिवत्त HSE_state.HSES_अन्वेषणम्
    अञ्ञथा:
        पटिनिवत्त HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    पटिनिवत्त HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    सचे hse.bit_index == 0x80:
        पटिनिवत्त HSE_state.HSES_DONE
    नोचेद्यदि can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        पटिनिवत्त HSE_state.HSES_DONE
    अञ्ञथा:
        पटिनिवत्त HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    निवेशः_buf_sz = get_निवेशः_buffer_size(hse)

    msi = hse.match_scan_index

    rem = निवेशः_buf_sz - msi  # unprocessed bytes
    shift_sz = निवेशः_buf_sz + rem

    अत्थाय i अन्तः range(shift_sz):
        hse.buffer[i] = hse.buffer[निवेशः_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.निवेशः_size -= निवेशः_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    निवेशः_buf = (ctypes.c_uint8 * len(data))()
    अत्थाय i, d अन्तः enumerate(data):
        निवेशः_buf[i] = d
    in_size = len(निवेशः_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    याव sunk < in_size:
        निवेशः_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, निवेशः_buf, in_size - sunk, निवेशः_size)
        निवेशः_buf = निवेशः_buf[निवेशः_size.मानम्:]
        sunk += निवेशः_size.मानम्
        सचे sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        याव poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        सचे sunk == in_size:
            heatshrink_encoder_finish(hse)
    पटिनिवत्त ret
