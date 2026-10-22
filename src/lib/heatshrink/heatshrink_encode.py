ഇറക്കുമതി ctypes
തഃ enum ഇറക്കുമതി Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
വർഗ്ഗം HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_തിരയൽ = 2
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
വർഗ്ഗം output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    തള്ളുക_bits(hse, 1, tag, oi)


def തള്ളുക_bits(hse, എണ്ണം, bits, oi):
    പ്രതിജ്ഞാ എണ്ണം <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    വേണ്ടി i ഇൽ range(എണ്ണം - 1, -1, -1):
        bit = bits & (1 << i)
        എങ്കിൽ bit:
            current_byte.മൂല്യം |= bit_index.മൂല്യം
        bit_index.മൂല്യം >>= 1
        എങ്കിൽ bit_index.മൂല്യം == 0:
            bit_index.മൂല്യം = 0x80
            oi.buf[oi.output_size[0]] = current_byte.മൂല്യം
            oi.output_size[0] += 1
            current_byte.മൂല്യം = 0
    hse.current_byte = current_byte.മൂല്യം
    hse.bit_index = bit_index.മൂല്യം


def തള്ളുക_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    നിവേശം_offset = get_നിവേശം_offset(hse) + processed_offset
    c = hse.buffer[നിവേശം_offset]
    തള്ളുക_bits(hse, 8, c, oi)


# Define necessary structures and enums
വർഗ്ഗം heatshrink_encoder(ctypes.Structure):
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
        പരം().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.തിരയൽ_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


വർഗ്ഗം hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        പരം().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


വർഗ്ഗം HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


വർഗ്ഗം HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


വർഗ്ഗം HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    തിരികെ hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    തിരികെ oi.output_size[0] < oi.buf_size


def get_നിവേശം_buffer_size(hse):
    തിരികെ 1 << hse.window_size


def get_lookahead_size(hse):
    തിരികെ 1 << hse.lookahead_size


def get_നിവേശം_offset(hse):
    തിരികെ get_നിവേശം_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, നിവേശം_size):
    എങ്കിൽ hse is None അഥവാ in_buf is None അഥവാ നിവേശം_size is None:
        തിരികെ HSE_sink_res.HSER_SINK_ERROR_NULL

    എങ്കിൽ is_finishing(hse):
        തിരികെ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    എങ്കിൽ hse.state != HSE_state.HSES_NOT_FULL:
        തിരികെ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_നിവേശം_offset(hse) + hse.നിവേശം_size
    ibs = get_നിവേശം_buffer_size(hse)
    rem = ibs - hse.നിവേശം_size
    cp_sz = min(rem, size)

    വേണ്ടി i ഇൽ range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    നിവേശം_size.മൂല്യം = cp_sz
    hse.നിവേശം_size += cp_sz

    എങ്കിൽ cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    തിരികെ HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.തിരയൽ_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    നിവേശം_offset = get_നിവേശം_offset(hse)
    end = നിവേശം_offset + hse.നിവേശം_size

    വേണ്ടി i ഇൽ range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    എങ്കിൽ hse is None അഥവാ out_buf is None അഥവാ output_size is None:
        തിരികെ HSE_poll_res.HSER_POLL_ERROR_NULL

    എങ്കിൽ out_buf_size == 0:
        തിരികെ HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    ആകുമ്പോൾ സത്യം:
        in_state = hse.state
        എങ്കിൽ in_state == HSE_state.HSES_NOT_FULL:
            തിരികെ HSE_poll_res.HSER_POLL_EMPTY
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_DONE:
            തിരികെ HSE_poll_res.HSER_POLL_EMPTY
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_തിരയൽ
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_തിരയൽ:
            hse.state = st_step_തിരയൽ(hse)
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        അല്ലെങ്കിലെങ്കിൽ in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        അല്ലെങ്കിൽ:
            തിരികെ HSE_poll_res.HSER_POLL_ERROR_MISUSE

        എങ്കിൽ hse.state == in_state:
            എങ്കിൽ oi.output_size == oi.buf_size:
                തിരികെ HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    എങ്കിൽ hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    എങ്കിൽ hse.state == HSE_state.HSES_DONE:
        തിരികെ HSE_finish_res.HSER_FINISH_DONE
    തിരികെ HSE_finish_res.HSER_FINISH_MORE


def st_step_തിരയൽ(hse):
    window_length = get_നിവേശം_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    എങ്കിൽ msi > hse.നിവേശം_size - (1 എങ്കിൽ fin അല്ലെങ്കിൽ lookahead_sz):
        തിരികെ HSE_state.HSES_FLUSH_BITS എങ്കിൽ fin അല്ലെങ്കിൽ HSE_state.HSES_SAVE_BACKLOG

    നിവേശം_offset = get_നിവേശം_offset(hse)
    end = നിവേശം_offset + msi
    start = end - window_length

    max_possible = lookahead_sz എങ്കിൽ hse.നിവേശം_size - msi >= lookahead_sz അല്ലെങ്കിൽ hse.നിവേശം_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    എങ്കിൽ match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        തിരികെ HSE_state.HSES_YIELD_TAG_BIT
    അല്ലെങ്കിൽ:
        hse.match_pos = match_pos
        hse.match_length = match_length
        തിരികെ HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.തിരയൽ_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    ആകുമ്പോൾ pos >= start:

        എങ്കിൽ buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.തിരയൽ_index.index[pos]
            തുടരുക

        length = 1
        വേണ്ടി length ഇൽ range(1, maxlen):
            എങ്കിൽ buf[pos + length] != buf[needlepoint + length]:
                നിർത്തുക
        എങ്കിൽ length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            എങ്കിൽ length == maxlen:
                നിർത്തുക  # won't find better

        pos = hse.തിരയൽ_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    എങ്കിൽ match_maxlen > (break_even_point // 8):
        തിരികെ end - match_index, match_maxlen
    തിരികെ MATCH_NOT_FOUND, 0


def തള്ളുക_outgoing_bits(hse, oi):
    എങ്കിൽ hse.outgoing_bits_എണ്ണം > 8:
        എണ്ണം = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_എണ്ണം - 8)
    അല്ലെങ്കിൽ:
        എണ്ണം = hse.outgoing_bits_എണ്ണം
        bits = hse.outgoing_bits

    എങ്കിൽ എണ്ണം > 0:
        തള്ളുക_bits(hse, എണ്ണം, bits, oi)
        hse.outgoing_bits_എണ്ണം -= എണ്ണം
    തിരികെ എണ്ണം


def st_yield_tag_bit(hse, oi):
    എങ്കിൽ can_take_byte(oi):
        എങ്കിൽ hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            തിരികെ HSE_state.HSES_YIELD_LITERAL
        അല്ലെങ്കിൽ:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_എണ്ണം = hse.window_size
            തിരികെ HSE_state.HSES_YIELD_BR_INDEX
    അല്ലെങ്കിൽ:
        തിരികെ HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    എങ്കിൽ can_take_byte(oi):
        തള്ളുക_literal_byte(hse, oi)
        തിരികെ HSE_state.HSES_തിരയൽ
    അല്ലെങ്കിൽ:
        തിരികെ HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    എങ്കിൽ can_take_byte(oi):
        എങ്കിൽ തള്ളുക_outgoing_bits(hse, oi) > 0:
            തിരികെ HSE_state.HSES_YIELD_BR_INDEX
        അല്ലെങ്കിൽ:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_എണ്ണം = hse.lookahead_size
            തിരികെ HSE_state.HSES_YIELD_BR_LENGTH
    അല്ലെങ്കിൽ:
        തിരികെ HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    എങ്കിൽ can_take_byte(oi):
        എങ്കിൽ തള്ളുക_outgoing_bits(hse, oi) > 0:
            തിരികെ HSE_state.HSES_YIELD_BR_LENGTH
        അല്ലെങ്കിൽ:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            തിരികെ HSE_state.HSES_തിരയൽ
    അല്ലെങ്കിൽ:
        തിരികെ HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    തിരികെ HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    എങ്കിൽ hse.bit_index == 0x80:
        തിരികെ HSE_state.HSES_DONE
    അല്ലെങ്കിലെങ്കിൽ can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        തിരികെ HSE_state.HSES_DONE
    അല്ലെങ്കിൽ:
        തിരികെ HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    നിവേശം_buf_sz = get_നിവേശം_buffer_size(hse)

    msi = hse.match_scan_index

    rem = നിവേശം_buf_sz - msi  # unprocessed bytes
    shift_sz = നിവേശം_buf_sz + rem

    വേണ്ടി i ഇൽ range(shift_sz):
        hse.buffer[i] = hse.buffer[നിവേശം_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.നിവേശം_size -= നിവേശം_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    നിവേശം_buf = (ctypes.c_uint8 * len(data))()
    വേണ്ടി i, d ഇൽ enumerate(data):
        നിവേശം_buf[i] = d
    in_size = len(നിവേശം_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    ആകുമ്പോൾ sunk < in_size:
        നിവേശം_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, നിവേശം_buf, in_size - sunk, നിവേശം_size)
        നിവേശം_buf = നിവേശം_buf[നിവേശം_size.മൂല്യം:]
        sunk += നിവേശം_size.മൂല്യം
        എങ്കിൽ sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        ആകുമ്പോൾ poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        എങ്കിൽ sunk == in_size:
            heatshrink_encoder_finish(hse)
    തിരികെ ret
