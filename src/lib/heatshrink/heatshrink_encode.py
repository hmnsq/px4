आनयन ctypes
तः enum आनयन Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
वर्ग HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_खोज = 2
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
वर्ग output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    थप_bits(hse, 1, tag, oi)


def थप_bits(hse, गन्ती, bits, oi):
    प्रतिज्ञा गन्ती <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    लागि i अन्तः range(गन्ती - 1, -1, -1):
        bit = bits & (1 << i)
        यदि bit:
            current_byte.मान |= bit_index.मान
        bit_index.मान >>= 1
        यदि bit_index.मान == 0:
            bit_index.मान = 0x80
            oi.buf[oi.output_size[0]] = current_byte.मान
            oi.output_size[0] += 1
            current_byte.मान = 0
    hse.current_byte = current_byte.मान
    hse.bit_index = bit_index.मान


def थप_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    आगत_offset = get_आगत_offset(hse) + processed_offset
    c = hse.buffer[आगत_offset]
    थप_bits(hse, 8, c, oi)


# Define necessary structures and enums
वर्ग heatshrink_encoder(ctypes.Structure):
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
        self.खोज_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


वर्ग hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        पर().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


वर्ग HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


वर्ग HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


वर्ग HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    फर्काऊ hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    फर्काऊ oi.output_size[0] < oi.buf_size


def get_आगत_buffer_size(hse):
    फर्काऊ 1 << hse.window_size


def get_lookahead_size(hse):
    फर्काऊ 1 << hse.lookahead_size


def get_आगत_offset(hse):
    फर्काऊ get_आगत_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, आगत_size):
    यदि hse is None वा in_buf is None वा आगत_size is None:
        फर्काऊ HSE_sink_res.HSER_SINK_ERROR_NULL

    यदि is_finishing(hse):
        फर्काऊ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    यदि hse.state != HSE_state.HSES_NOT_FULL:
        फर्काऊ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_आगत_offset(hse) + hse.आगत_size
    ibs = get_आगत_buffer_size(hse)
    rem = ibs - hse.आगत_size
    cp_sz = min(rem, size)

    लागि i अन्तः range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    आगत_size.मान = cp_sz
    hse.आगत_size += cp_sz

    यदि cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    फर्काऊ HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.खोज_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    आगत_offset = get_आगत_offset(hse)
    end = आगत_offset + hse.आगत_size

    लागि i अन्तः range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    यदि hse is None वा out_buf is None वा output_size is None:
        फर्काऊ HSE_poll_res.HSER_POLL_ERROR_NULL

    यदि out_buf_size == 0:
        फर्काऊ HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    जबसम्म सत्य:
        in_state = hse.state
        यदि in_state == HSE_state.HSES_NOT_FULL:
            फर्काऊ HSE_poll_res.HSER_POLL_EMPTY
        अथवायदि in_state == HSE_state.HSES_DONE:
            फर्काऊ HSE_poll_res.HSER_POLL_EMPTY
        अथवायदि in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_खोज
        अथवायदि in_state == HSE_state.HSES_खोज:
            hse.state = st_step_खोज(hse)
        अथवायदि in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        अथवायदि in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        अथवायदि in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        अथवायदि in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        अथवायदि in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        अथवायदि in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        अन्यथा:
            फर्काऊ HSE_poll_res.HSER_POLL_ERROR_MISUSE

        यदि hse.state == in_state:
            यदि oi.output_size == oi.buf_size:
                फर्काऊ HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    यदि hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    यदि hse.state == HSE_state.HSES_DONE:
        फर्काऊ HSE_finish_res.HSER_FINISH_DONE
    फर्काऊ HSE_finish_res.HSER_FINISH_MORE


def st_step_खोज(hse):
    window_length = get_आगत_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    यदि msi > hse.आगत_size - (1 यदि fin अन्यथा lookahead_sz):
        फर्काऊ HSE_state.HSES_FLUSH_BITS यदि fin अन्यथा HSE_state.HSES_SAVE_BACKLOG

    आगत_offset = get_आगत_offset(hse)
    end = आगत_offset + msi
    start = end - window_length

    max_possible = lookahead_sz यदि hse.आगत_size - msi >= lookahead_sz अन्यथा hse.आगत_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    यदि match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        फर्काऊ HSE_state.HSES_YIELD_TAG_BIT
    अन्यथा:
        hse.match_pos = match_pos
        hse.match_length = match_length
        फर्काऊ HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.खोज_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    जबसम्म pos >= start:

        यदि buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.खोज_index.index[pos]
            जारी

        length = 1
        लागि length अन्तः range(1, maxlen):
            यदि buf[pos + length] != buf[needlepoint + length]:
                रोक
        यदि length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            यदि length == maxlen:
                रोक  # won't find better

        pos = hse.खोज_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    यदि match_maxlen > (break_even_point // 8):
        फर्काऊ end - match_index, match_maxlen
    फर्काऊ MATCH_NOT_FOUND, 0


def थप_outgoing_bits(hse, oi):
    यदि hse.outgoing_bits_गन्ती > 8:
        गन्ती = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_गन्ती - 8)
    अन्यथा:
        गन्ती = hse.outgoing_bits_गन्ती
        bits = hse.outgoing_bits

    यदि गन्ती > 0:
        थप_bits(hse, गन्ती, bits, oi)
        hse.outgoing_bits_गन्ती -= गन्ती
    फर्काऊ गन्ती


def st_yield_tag_bit(hse, oi):
    यदि can_take_byte(oi):
        यदि hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            फर्काऊ HSE_state.HSES_YIELD_LITERAL
        अन्यथा:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_गन्ती = hse.window_size
            फर्काऊ HSE_state.HSES_YIELD_BR_INDEX
    अन्यथा:
        फर्काऊ HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    यदि can_take_byte(oi):
        थप_literal_byte(hse, oi)
        फर्काऊ HSE_state.HSES_खोज
    अन्यथा:
        फर्काऊ HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    यदि can_take_byte(oi):
        यदि थप_outgoing_bits(hse, oi) > 0:
            फर्काऊ HSE_state.HSES_YIELD_BR_INDEX
        अन्यथा:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_गन्ती = hse.lookahead_size
            फर्काऊ HSE_state.HSES_YIELD_BR_LENGTH
    अन्यथा:
        फर्काऊ HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    यदि can_take_byte(oi):
        यदि थप_outgoing_bits(hse, oi) > 0:
            फर्काऊ HSE_state.HSES_YIELD_BR_LENGTH
        अन्यथा:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            फर्काऊ HSE_state.HSES_खोज
    अन्यथा:
        फर्काऊ HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    फर्काऊ HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    यदि hse.bit_index == 0x80:
        फर्काऊ HSE_state.HSES_DONE
    अथवायदि can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        फर्काऊ HSE_state.HSES_DONE
    अन्यथा:
        फर्काऊ HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    आगत_buf_sz = get_आगत_buffer_size(hse)

    msi = hse.match_scan_index

    rem = आगत_buf_sz - msi  # unprocessed bytes
    shift_sz = आगत_buf_sz + rem

    लागि i अन्तः range(shift_sz):
        hse.buffer[i] = hse.buffer[आगत_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.आगत_size -= आगत_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    आगत_buf = (ctypes.c_uint8 * len(data))()
    लागि i, d अन्तः enumerate(data):
        आगत_buf[i] = d
    in_size = len(आगत_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    जबसम्म sunk < in_size:
        आगत_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, आगत_buf, in_size - sunk, आगत_size)
        आगत_buf = आगत_buf[आगत_size.मान:]
        sunk += आगत_size.मान
        यदि sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        जबसम्म poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        यदि sunk == in_size:
            heatshrink_encoder_finish(hse)
    फर्काऊ ret
