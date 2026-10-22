ଆମଦାନୀ ctypes
ତଃ enum ଆମଦାନୀ Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
ଶ୍ରେଣୀ HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_ସନ୍ଧାନ = 2
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
ଶ୍ରେଣୀ output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    ଠେଲ_bits(hse, 1, tag, oi)


def ଠେଲ_bits(hse, ଗଣନା_2, bits, oi):
    ପ୍ରତିଜ୍ଞା ଗଣନା_2 <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    ପାଇଁ i ଭିତରେ range(ଗଣନା_2 - 1, -1, -1):
        bit = bits & (1 << i)
        ଯଦି bit:
            current_byte.ମୂଲ୍ୟ |= bit_index.ମୂଲ୍ୟ
        bit_index.ମୂଲ୍ୟ >>= 1
        ଯଦି bit_index.ମୂଲ୍ୟ == 0:
            bit_index.ମୂଲ୍ୟ = 0x80
            oi.buf[oi.output_size[0]] = current_byte.ମୂଲ୍ୟ
            oi.output_size[0] += 1
            current_byte.ମୂଲ୍ୟ = 0
    hse.current_byte = current_byte.ମୂଲ୍ୟ
    hse.bit_index = bit_index.ମୂଲ୍ୟ


def ଠେଲ_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    ନିବେଶ_offset = get_ନିବେଶ_offset(hse) + processed_offset
    c = hse.buffer[ନିବେଶ_offset]
    ଠେଲ_bits(hse, 8, c, oi)


# Define necessary structures and enums
ଶ୍ରେଣୀ heatshrink_encoder(ctypes.Structure):
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
        ପରମ().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.ସନ୍ଧାନ_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


ଶ୍ରେଣୀ hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        ପରମ().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


ଶ୍ରେଣୀ HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


ଶ୍ରେଣୀ HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


ଶ୍ରେଣୀ HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    ଫେରନ୍ତୁ hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    ଫେରନ୍ତୁ oi.output_size[0] < oi.buf_size


def get_ନିବେଶ_buffer_size(hse):
    ଫେରନ୍ତୁ 1 << hse.window_size


def get_lookahead_size(hse):
    ଫେରନ୍ତୁ 1 << hse.lookahead_size


def get_ନିବେଶ_offset(hse):
    ଫେରନ୍ତୁ get_ନିବେଶ_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, ନିବେଶ_size):
    ଯଦି hse is None କିମ୍ବା in_buf is None କିମ୍ବା ନିବେଶ_size is None:
        ଫେରନ୍ତୁ HSE_sink_res.HSER_SINK_ERROR_NULL

    ଯଦି is_finishing(hse):
        ଫେରନ୍ତୁ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    ଯଦି hse.state != HSE_state.HSES_NOT_FULL:
        ଫେରନ୍ତୁ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_ନିବେଶ_offset(hse) + hse.ନିବେଶ_size
    ibs = get_ନିବେଶ_buffer_size(hse)
    rem = ibs - hse.ନିବେଶ_size
    cp_sz = min(rem, size)

    ପାଇଁ i ଭିତରେ range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    ନିବେଶ_size.ମୂଲ୍ୟ = cp_sz
    hse.ନିବେଶ_size += cp_sz

    ଯଦି cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    ଫେରନ୍ତୁ HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.ସନ୍ଧାନ_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    ନିବେଶ_offset = get_ନିବେଶ_offset(hse)
    end = ନିବେଶ_offset + hse.ନିବେଶ_size

    ପାଇଁ i ଭିତରେ range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    ଯଦି hse is None କିମ୍ବା out_buf is None କିମ୍ବା output_size is None:
        ଫେରନ୍ତୁ HSE_poll_res.HSER_POLL_ERROR_NULL

    ଯଦି out_buf_size == 0:
        ଫେରନ୍ତୁ HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    ଯେତେବେଳେ ସତ୍ୟ:
        in_state = hse.state
        ଯଦି in_state == HSE_state.HSES_NOT_FULL:
            ଫେରନ୍ତୁ HSE_poll_res.HSER_POLL_EMPTY
        ନଚେତଯଦି in_state == HSE_state.HSES_DONE:
            ଫେରନ୍ତୁ HSE_poll_res.HSER_POLL_EMPTY
        ନଚେତଯଦି in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_ସନ୍ଧାନ
        ନଚେତଯଦି in_state == HSE_state.HSES_ସନ୍ଧାନ:
            hse.state = st_step_ସନ୍ଧାନ(hse)
        ନଚେତଯଦି in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        ନଚେତଯଦି in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        ନଚେତଯଦି in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        ନଚେତଯଦି in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        ନଚେତଯଦି in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        ନଚେତଯଦି in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        ଅନ୍ୟଥା:
            ଫେରନ୍ତୁ HSE_poll_res.HSER_POLL_ERROR_MISUSE

        ଯଦି hse.state == in_state:
            ଯଦି oi.output_size == oi.buf_size:
                ଫେରନ୍ତୁ HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    ଯଦି hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    ଯଦି hse.state == HSE_state.HSES_DONE:
        ଫେରନ୍ତୁ HSE_finish_res.HSER_FINISH_DONE
    ଫେରନ୍ତୁ HSE_finish_res.HSER_FINISH_MORE


def st_step_ସନ୍ଧାନ(hse):
    window_length = get_ନିବେଶ_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    ଯଦି msi > hse.ନିବେଶ_size - (1 ଯଦି fin ଅନ୍ୟଥା lookahead_sz):
        ଫେରନ୍ତୁ HSE_state.HSES_FLUSH_BITS ଯଦି fin ଅନ୍ୟଥା HSE_state.HSES_SAVE_BACKLOG

    ନିବେଶ_offset = get_ନିବେଶ_offset(hse)
    end = ନିବେଶ_offset + msi
    start = end - window_length

    max_possible = lookahead_sz ଯଦି hse.ନିବେଶ_size - msi >= lookahead_sz ଅନ୍ୟଥା hse.ନିବେଶ_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    ଯଦି match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        ଫେରନ୍ତୁ HSE_state.HSES_YIELD_TAG_BIT
    ଅନ୍ୟଥା:
        hse.match_pos = match_pos
        hse.match_length = match_length
        ଫେରନ୍ତୁ HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.ସନ୍ଧାନ_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    ଯେତେବେଳେ pos >= start:

        ଯଦି buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.ସନ୍ଧାନ_index.index[pos]
            ଜାରି

        length = 1
        ପାଇଁ length ଭିତରେ range(1, maxlen):
            ଯଦି buf[pos + length] != buf[needlepoint + length]:
                ଭାଙ୍ଗ
        ଯଦି length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            ଯଦି length == maxlen:
                ଭାଙ୍ଗ  # won't find better

        pos = hse.ସନ୍ଧାନ_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    ଯଦି match_maxlen > (break_even_point // 8):
        ଫେରନ୍ତୁ end - match_index, match_maxlen
    ଫେରନ୍ତୁ MATCH_NOT_FOUND, 0


def ଠେଲ_outgoing_bits(hse, oi):
    ଯଦି hse.outgoing_bits_ଗଣନା > 8:
        ଗଣନା_2 = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_ଗଣନା - 8)
    ଅନ୍ୟଥା:
        ଗଣନା_2 = hse.outgoing_bits_ଗଣନା
        bits = hse.outgoing_bits

    ଯଦି ଗଣନା_2 > 0:
        ଠେଲ_bits(hse, ଗଣନା_2, bits, oi)
        hse.outgoing_bits_ଗଣନା -= ଗଣନା_2
    ଫେରନ୍ତୁ ଗଣନା_2


def st_yield_tag_bit(hse, oi):
    ଯଦି can_take_byte(oi):
        ଯଦି hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            ଫେରନ୍ତୁ HSE_state.HSES_YIELD_LITERAL
        ଅନ୍ୟଥା:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_ଗଣନା = hse.window_size
            ଫେରନ୍ତୁ HSE_state.HSES_YIELD_BR_INDEX
    ଅନ୍ୟଥା:
        ଫେରନ୍ତୁ HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    ଯଦି can_take_byte(oi):
        ଠେଲ_literal_byte(hse, oi)
        ଫେରନ୍ତୁ HSE_state.HSES_ସନ୍ଧାନ
    ଅନ୍ୟଥା:
        ଫେରନ୍ତୁ HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    ଯଦି can_take_byte(oi):
        ଯଦି ଠେଲ_outgoing_bits(hse, oi) > 0:
            ଫେରନ୍ତୁ HSE_state.HSES_YIELD_BR_INDEX
        ଅନ୍ୟଥା:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_ଗଣନା = hse.lookahead_size
            ଫେରନ୍ତୁ HSE_state.HSES_YIELD_BR_LENGTH
    ଅନ୍ୟଥା:
        ଫେରନ୍ତୁ HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    ଯଦି can_take_byte(oi):
        ଯଦି ଠେଲ_outgoing_bits(hse, oi) > 0:
            ଫେରନ୍ତୁ HSE_state.HSES_YIELD_BR_LENGTH
        ଅନ୍ୟଥା:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            ଫେରନ୍ତୁ HSE_state.HSES_ସନ୍ଧାନ
    ଅନ୍ୟଥା:
        ଫେରନ୍ତୁ HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    ଫେରନ୍ତୁ HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    ଯଦି hse.bit_index == 0x80:
        ଫେରନ୍ତୁ HSE_state.HSES_DONE
    ନଚେତଯଦି can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        ଫେରନ୍ତୁ HSE_state.HSES_DONE
    ଅନ୍ୟଥା:
        ଫେରନ୍ତୁ HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    ନିବେଶ_buf_sz = get_ନିବେଶ_buffer_size(hse)

    msi = hse.match_scan_index

    rem = ନିବେଶ_buf_sz - msi  # unprocessed bytes
    shift_sz = ନିବେଶ_buf_sz + rem

    ପାଇଁ i ଭିତରେ range(shift_sz):
        hse.buffer[i] = hse.buffer[ନିବେଶ_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.ନିବେଶ_size -= ନିବେଶ_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    ନିବେଶ_buf = (ctypes.c_uint8 * len(data))()
    ପାଇଁ i, d ଭିତରେ enumerate(data):
        ନିବେଶ_buf[i] = d
    in_size = len(ନିବେଶ_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    ଯେତେବେଳେ sunk < in_size:
        ନିବେଶ_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, ନିବେଶ_buf, in_size - sunk, ନିବେଶ_size)
        ନିବେଶ_buf = ନିବେଶ_buf[ନିବେଶ_size.ମୂଲ୍ୟ:]
        sunk += ନିବେଶ_size.ମୂଲ୍ୟ
        ଯଦି sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        ଯେତେବେଳେ poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        ଯଦି sunk == in_size:
            heatshrink_encoder_finish(hse)
    ଫେରନ୍ତୁ ret
