import ctypes
ta_h enum import Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
class HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_SEARCH = 2
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
class output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    push_bits(hse, 1, tag, oi)


def push_bits(hse, count, bits, oi):
    assert count <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    for i anta_h range(count - 1, -1, -1):
        bit = bits & (1 << i)
        ᱡᱩᱫᱤ bit:
            current_byte.value |= bit_index.value
        bit_index.value >>= 1
        ᱡᱩᱫᱤ bit_index.value == 0:
            bit_index.value = 0x80
            oi.buf[oi.output_size[0]] = current_byte.value
            oi.output_size[0] += 1
            current_byte.value = 0
    hse.current_byte = current_byte.value
    hse.bit_index = bit_index.value


def push_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    input_offset = get_input_offset(hse) + processed_offset
    c = hse.buffer[input_offset]
    push_bits(hse, 8, c, oi)


# Define necessary structures and enums
class heatshrink_encoder(ctypes.Structure):
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
        super().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.search_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


class hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        super().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


class HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


class HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


class HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    ᱨᱩᱣᱟᱹᱲ hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    ᱨᱩᱣᱟᱹᱲ oi.output_size[0] < oi.buf_size


def get_input_buffer_size(hse):
    ᱨᱩᱣᱟᱹᱲ 1 << hse.window_size


def get_lookahead_size(hse):
    ᱨᱩᱣᱟᱹᱲ 1 << hse.lookahead_size


def get_input_offset(hse):
    ᱨᱩᱣᱟᱹᱲ get_input_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, input_size):
    ᱡᱩᱫᱤ hse is None or in_buf is None or input_size is None:
        ᱨᱩᱣᱟᱹᱲ HSE_sink_res.HSER_SINK_ERROR_NULL

    ᱡᱩᱫᱤ is_finishing(hse):
        ᱨᱩᱣᱟᱹᱲ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    ᱡᱩᱫᱤ hse.state != HSE_state.HSES_NOT_FULL:
        ᱨᱩᱣᱟᱹᱲ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_input_offset(hse) + hse.input_size
    ibs = get_input_buffer_size(hse)
    rem = ibs - hse.input_size
    cp_sz = min(rem, size)

    for i anta_h range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    input_size.value = cp_sz
    hse.input_size += cp_sz

    ᱡᱩᱫᱤ cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    ᱨᱩᱣᱟᱹᱲ HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.search_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    input_offset = get_input_offset(hse)
    end = input_offset + hse.input_size

    for i anta_h range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    ᱡᱩᱫᱤ hse is None or out_buf is None or output_size is None:
        ᱨᱩᱣᱟᱹᱲ HSE_poll_res.HSER_POLL_ERROR_NULL

    ᱡᱩᱫᱤ out_buf_size == 0:
        ᱨᱩᱣᱟᱹᱲ HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    ᱡᱚᱠᱷᱚᱱ True:
        in_state = hse.state
        ᱡᱩᱫᱤ in_state == HSE_state.HSES_NOT_FULL:
            ᱨᱩᱣᱟᱹᱲ HSE_poll_res.HSER_POLL_EMPTY
        elif in_state == HSE_state.HSES_DONE:
            ᱨᱩᱣᱟᱹᱲ HSE_poll_res.HSER_POLL_EMPTY
        elif in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_SEARCH
        elif in_state == HSE_state.HSES_SEARCH:
            hse.state = st_step_search(hse)
        elif in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        elif in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        elif in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        elif in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        elif in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        elif in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        ᱵᱟᱝ:
            ᱨᱩᱣᱟᱹᱲ HSE_poll_res.HSER_POLL_ERROR_MISUSE

        ᱡᱩᱫᱤ hse.state == in_state:
            ᱡᱩᱫᱤ oi.output_size == oi.buf_size:
                ᱨᱩᱣᱟᱹᱲ HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    ᱡᱩᱫᱤ hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    ᱡᱩᱫᱤ hse.state == HSE_state.HSES_DONE:
        ᱨᱩᱣᱟᱹᱲ HSE_finish_res.HSER_FINISH_DONE
    ᱨᱩᱣᱟᱹᱲ HSE_finish_res.HSER_FINISH_MORE


def st_step_search(hse):
    window_length = get_input_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    ᱡᱩᱫᱤ msi > hse.input_size - (1 ᱡᱩᱫᱤ fin ᱵᱟᱝ lookahead_sz):
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_FLUSH_BITS ᱡᱩᱫᱤ fin ᱵᱟᱝ HSE_state.HSES_SAVE_BACKLOG

    input_offset = get_input_offset(hse)
    end = input_offset + msi
    start = end - window_length

    max_possible = lookahead_sz ᱡᱩᱫᱤ hse.input_size - msi >= lookahead_sz ᱵᱟᱝ hse.input_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    ᱡᱩᱫᱤ match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_TAG_BIT
    ᱵᱟᱝ:
        hse.match_pos = match_pos
        hse.match_length = match_length
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.search_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    ᱡᱚᱠᱷᱚᱱ pos >= start:

        ᱡᱩᱫᱤ buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.search_index.index[pos]
            continue

        length = 1
        for length anta_h range(1, maxlen):
            ᱡᱩᱫᱤ buf[pos + length] != buf[needlepoint + length]:
                bha_nga
        ᱡᱩᱫᱤ length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            ᱡᱩᱫᱤ length == maxlen:
                bha_nga  # won't find better

        pos = hse.search_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    ᱡᱩᱫᱤ match_maxlen > (break_even_point // 8):
        ᱨᱩᱣᱟᱹᱲ end - match_index, match_maxlen
    ᱨᱩᱣᱟᱹᱲ MATCH_NOT_FOUND, 0


def push_outgoing_bits(hse, oi):
    ᱡᱩᱫᱤ hse.outgoing_bits_count > 8:
        count = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_count - 8)
    ᱵᱟᱝ:
        count = hse.outgoing_bits_count
        bits = hse.outgoing_bits

    ᱡᱩᱫᱤ count > 0:
        push_bits(hse, count, bits, oi)
        hse.outgoing_bits_count -= count
    ᱨᱩᱣᱟᱹᱲ count


def st_yield_tag_bit(hse, oi):
    ᱡᱩᱫᱤ can_take_byte(oi):
        ᱡᱩᱫᱤ hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_LITERAL
        ᱵᱟᱝ:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_count = hse.window_size
            ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_BR_INDEX
    ᱵᱟᱝ:
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    ᱡᱩᱫᱤ can_take_byte(oi):
        push_literal_byte(hse, oi)
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_SEARCH
    ᱵᱟᱝ:
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    ᱡᱩᱫᱤ can_take_byte(oi):
        ᱡᱩᱫᱤ push_outgoing_bits(hse, oi) > 0:
            ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_BR_INDEX
        ᱵᱟᱝ:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_count = hse.lookahead_size
            ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_BR_LENGTH
    ᱵᱟᱝ:
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    ᱡᱩᱫᱤ can_take_byte(oi):
        ᱡᱩᱫᱤ push_outgoing_bits(hse, oi) > 0:
            ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_BR_LENGTH
        ᱵᱟᱝ:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_SEARCH
    ᱵᱟᱝ:
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    ᱡᱩᱫᱤ hse.bit_index == 0x80:
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_DONE
    elif can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_DONE
    ᱵᱟᱝ:
        ᱨᱩᱣᱟᱹᱲ HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    input_buf_sz = get_input_buffer_size(hse)

    msi = hse.match_scan_index

    rem = input_buf_sz - msi  # unprocessed bytes
    shift_sz = input_buf_sz + rem

    for i anta_h range(shift_sz):
        hse.buffer[i] = hse.buffer[input_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.input_size -= input_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    input_buf = (ctypes.c_uint8 * len(data))()
    for i, d anta_h enumerate(data):
        input_buf[i] = d
    in_size = len(input_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    ᱡᱚᱠᱷᱚᱱ sunk < in_size:
        input_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, input_buf, in_size - sunk, input_size)
        input_buf = input_buf[input_size.value:]
        sunk += input_size.value
        ᱡᱩᱫᱤ sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        ᱡᱚᱠᱷᱚᱱ poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        ᱡᱩᱫᱤ sunk == in_size:
            heatshrink_encoder_finish(hse)
    ᱨᱩᱣᱟᱹᱲ ret
