واردکن ctypes
از enum واردکن Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
رده HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_جستجو = 2
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
رده output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    افزودن_bits(hse, 1, tag, oi)


def افزودن_bits(hse, شمار, bits, oi):
    أكد شمار <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    برای i در range(شمار - 1, -1, -1):
        bit = bits & (1 << i)
        اگر bit:
            current_byte.مقدار |= bit_index.مقدار
        bit_index.مقدار >>= 1
        اگر bit_index.مقدار == 0:
            bit_index.مقدار = 0x80
            oi.buf[oi.output_size[0]] = current_byte.مقدار
            oi.output_size[0] += 1
            current_byte.مقدار = 0
    hse.current_byte = current_byte.مقدار
    hse.bit_index = bit_index.مقدار


def افزودن_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    درون_داد_offset = get_درون_داد_offset(hse) + processed_offset
    c = hse.buffer[درون_داد_offset]
    افزودن_bits(hse, 8, c, oi)


# Define necessary structures and enums
رده heatshrink_encoder(ctypes.Structure):
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
        ابر().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.جستجو_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


رده hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        ابر().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


رده HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


رده HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


رده HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    بازگشت hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    بازگشت oi.output_size[0] < oi.buf_size


def get_درون_داد_buffer_size(hse):
    بازگشت 1 << hse.window_size


def get_lookahead_size(hse):
    بازگشت 1 << hse.lookahead_size


def get_درون_داد_offset(hse):
    بازگشت get_درون_داد_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, درون_داد_size):
    اگر hse is None یا in_buf is None یا درون_داد_size is None:
        بازگشت HSE_sink_res.HSER_SINK_ERROR_NULL

    اگر is_finishing(hse):
        بازگشت HSE_sink_res.HSER_SINK_ERROR_MISUSE

    اگر hse.state != HSE_state.HSES_NOT_FULL:
        بازگشت HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_درون_داد_offset(hse) + hse.درون_داد_size
    ibs = get_درون_داد_buffer_size(hse)
    rem = ibs - hse.درون_داد_size
    cp_sz = min(rem, size)

    برای i در range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    درون_داد_size.مقدار = cp_sz
    hse.درون_داد_size += cp_sz

    اگر cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    بازگشت HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.جستجو_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    درون_داد_offset = get_درون_داد_offset(hse)
    end = درون_داد_offset + hse.درون_داد_size

    برای i در range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    اگر hse is None یا out_buf is None یا output_size is None:
        بازگشت HSE_poll_res.HSER_POLL_ERROR_NULL

    اگر out_buf_size == 0:
        بازگشت HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    وقتی درست:
        in_state = hse.state
        اگر in_state == HSE_state.HSES_NOT_FULL:
            بازگشت HSE_poll_res.HSER_POLL_EMPTY
        وگرنهاگر in_state == HSE_state.HSES_DONE:
            بازگشت HSE_poll_res.HSER_POLL_EMPTY
        وگرنهاگر in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_جستجو
        وگرنهاگر in_state == HSE_state.HSES_جستجو:
            hse.state = st_step_جستجو(hse)
        وگرنهاگر in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        وگرنهاگر in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        وگرنهاگر in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        وگرنهاگر in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        وگرنهاگر in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        وگرنهاگر in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        وگرنه:
            بازگشت HSE_poll_res.HSER_POLL_ERROR_MISUSE

        اگر hse.state == in_state:
            اگر oi.output_size == oi.buf_size:
                بازگشت HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    اگر hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    اگر hse.state == HSE_state.HSES_DONE:
        بازگشت HSE_finish_res.HSER_FINISH_DONE
    بازگشت HSE_finish_res.HSER_FINISH_MORE


def st_step_جستجو(hse):
    window_length = get_درون_داد_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    اگر msi > hse.درون_داد_size - (1 اگر fin وگرنه lookahead_sz):
        بازگشت HSE_state.HSES_FLUSH_BITS اگر fin وگرنه HSE_state.HSES_SAVE_BACKLOG

    درون_داد_offset = get_درون_داد_offset(hse)
    end = درون_داد_offset + msi
    start = end - window_length

    max_possible = lookahead_sz اگر hse.درون_داد_size - msi >= lookahead_sz وگرنه hse.درون_داد_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    اگر match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        بازگشت HSE_state.HSES_YIELD_TAG_BIT
    وگرنه:
        hse.match_pos = match_pos
        hse.match_length = match_length
        بازگشت HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.جستجو_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    وقتی pos >= start:

        اگر buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.جستجو_index.index[pos]
            ادامه

        length = 1
        برای length در range(1, maxlen):
            اگر buf[pos + length] != buf[needlepoint + length]:
                بشکن
        اگر length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            اگر length == maxlen:
                بشکن  # won't find better

        pos = hse.جستجو_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    اگر match_maxlen > (break_even_point // 8):
        بازگشت end - match_index, match_maxlen
    بازگشت MATCH_NOT_FOUND, 0


def افزودن_outgoing_bits(hse, oi):
    اگر hse.outgoing_bits_شمار > 8:
        شمار = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_شمار - 8)
    وگرنه:
        شمار = hse.outgoing_bits_شمار
        bits = hse.outgoing_bits

    اگر شمار > 0:
        افزودن_bits(hse, شمار, bits, oi)
        hse.outgoing_bits_شمار -= شمار
    بازگشت شمار


def st_yield_tag_bit(hse, oi):
    اگر can_take_byte(oi):
        اگر hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            بازگشت HSE_state.HSES_YIELD_LITERAL
        وگرنه:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_شمار = hse.window_size
            بازگشت HSE_state.HSES_YIELD_BR_INDEX
    وگرنه:
        بازگشت HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    اگر can_take_byte(oi):
        افزودن_literal_byte(hse, oi)
        بازگشت HSE_state.HSES_جستجو
    وگرنه:
        بازگشت HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    اگر can_take_byte(oi):
        اگر افزودن_outgoing_bits(hse, oi) > 0:
            بازگشت HSE_state.HSES_YIELD_BR_INDEX
        وگرنه:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_شمار = hse.lookahead_size
            بازگشت HSE_state.HSES_YIELD_BR_LENGTH
    وگرنه:
        بازگشت HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    اگر can_take_byte(oi):
        اگر افزودن_outgoing_bits(hse, oi) > 0:
            بازگشت HSE_state.HSES_YIELD_BR_LENGTH
        وگرنه:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            بازگشت HSE_state.HSES_جستجو
    وگرنه:
        بازگشت HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    بازگشت HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    اگر hse.bit_index == 0x80:
        بازگشت HSE_state.HSES_DONE
    وگرنهاگر can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        بازگشت HSE_state.HSES_DONE
    وگرنه:
        بازگشت HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    درون_داد_buf_sz = get_درون_داد_buffer_size(hse)

    msi = hse.match_scan_index

    rem = درون_داد_buf_sz - msi  # unprocessed bytes
    shift_sz = درون_داد_buf_sz + rem

    برای i در range(shift_sz):
        hse.buffer[i] = hse.buffer[درون_داد_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.درون_داد_size -= درون_داد_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    درون_داد_buf = (ctypes.c_uint8 * len(data))()
    برای i, d در enumerate(data):
        درون_داد_buf[i] = d
    in_size = len(درون_داد_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    وقتی sunk < in_size:
        درون_داد_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, درون_داد_buf, in_size - sunk, درون_داد_size)
        درون_داد_buf = درون_داد_buf[درون_داد_size.مقدار:]
        sunk += درون_داد_size.مقدار
        اگر sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        وقتی poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        اگر sunk == in_size:
            heatshrink_encoder_finish(hse)
    بازگشت ret
