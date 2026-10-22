આયાત ctypes
તઃ enum આયાત Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
વર્ગ HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_શોધ = 2
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
વર્ગ output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    ધકેલો_bits(hse, 1, tag, oi)


def ધકેલો_bits(hse, ગણતરી, bits, oi):
    પ્રતિજ્ઞા ગણતરી <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    માટે i માં range(ગણતરી - 1, -1, -1):
        bit = bits & (1 << i)
        જો bit:
            current_byte.કિંમત |= bit_index.કિંમત
        bit_index.કિંમત >>= 1
        જો bit_index.કિંમત == 0:
            bit_index.કિંમત = 0x80
            oi.buf[oi.output_size[0]] = current_byte.કિંમત
            oi.output_size[0] += 1
            current_byte.કિંમત = 0
    hse.current_byte = current_byte.કિંમત
    hse.bit_index = bit_index.કિંમત


def ધકેલો_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    આગત_offset = get_આગત_offset(hse) + processed_offset
    c = hse.buffer[આગત_offset]
    ધકેલો_bits(hse, 8, c, oi)


# Define necessary structures and enums
વર્ગ heatshrink_encoder(ctypes.Structure):
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
        પરમ().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.શોધ_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


વર્ગ hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        પરમ().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


વર્ગ HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


વર્ગ HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


વર્ગ HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    પાછા hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    પાછા oi.output_size[0] < oi.buf_size


def get_આગત_buffer_size(hse):
    પાછા 1 << hse.window_size


def get_lookahead_size(hse):
    પાછા 1 << hse.lookahead_size


def get_આગત_offset(hse):
    પાછા get_આગત_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, આગત_size):
    જો hse is None અથવા in_buf is None અથવા આગત_size is None:
        પાછા HSE_sink_res.HSER_SINK_ERROR_NULL

    જો is_finishing(hse):
        પાછા HSE_sink_res.HSER_SINK_ERROR_MISUSE

    જો hse.state != HSE_state.HSES_NOT_FULL:
        પાછા HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_આગત_offset(hse) + hse.આગત_size
    ibs = get_આગત_buffer_size(hse)
    rem = ibs - hse.આગત_size
    cp_sz = min(rem, size)

    માટે i માં range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    આગત_size.કિંમત = cp_sz
    hse.આગત_size += cp_sz

    જો cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    પાછા HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.શોધ_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    આગત_offset = get_આગત_offset(hse)
    end = આગત_offset + hse.આગત_size

    માટે i માં range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    જો hse is None અથવા out_buf is None અથવા output_size is None:
        પાછા HSE_poll_res.HSER_POLL_ERROR_NULL

    જો out_buf_size == 0:
        પાછા HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    જ્યારે સાચું:
        in_state = hse.state
        જો in_state == HSE_state.HSES_NOT_FULL:
            પાછા HSE_poll_res.HSER_POLL_EMPTY
        અથવાજો in_state == HSE_state.HSES_DONE:
            પાછા HSE_poll_res.HSER_POLL_EMPTY
        અથવાજો in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_શોધ
        અથવાજો in_state == HSE_state.HSES_શોધ:
            hse.state = st_step_શોધ(hse)
        અથવાજો in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        અથવાજો in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        અથવાજો in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        અથવાજો in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        અથવાજો in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        અથવાજો in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        નહીંતર:
            પાછા HSE_poll_res.HSER_POLL_ERROR_MISUSE

        જો hse.state == in_state:
            જો oi.output_size == oi.buf_size:
                પાછા HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    જો hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    જો hse.state == HSE_state.HSES_DONE:
        પાછા HSE_finish_res.HSER_FINISH_DONE
    પાછા HSE_finish_res.HSER_FINISH_MORE


def st_step_શોધ(hse):
    window_length = get_આગત_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    જો msi > hse.આગત_size - (1 જો fin નહીંતર lookahead_sz):
        પાછા HSE_state.HSES_FLUSH_BITS જો fin નહીંતર HSE_state.HSES_SAVE_BACKLOG

    આગત_offset = get_આગત_offset(hse)
    end = આગત_offset + msi
    start = end - window_length

    max_possible = lookahead_sz જો hse.આગત_size - msi >= lookahead_sz નહીંતર hse.આગત_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    જો match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        પાછા HSE_state.HSES_YIELD_TAG_BIT
    નહીંતર:
        hse.match_pos = match_pos
        hse.match_length = match_length
        પાછા HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.શોધ_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    જ્યારે pos >= start:

        જો buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.શોધ_index.index[pos]
            ચાલુ

        length = 1
        માટે length માં range(1, maxlen):
            જો buf[pos + length] != buf[needlepoint + length]:
                તોડો
        જો length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            જો length == maxlen:
                તોડો  # won't find better

        pos = hse.શોધ_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    જો match_maxlen > (break_even_point // 8):
        પાછા end - match_index, match_maxlen
    પાછા MATCH_NOT_FOUND, 0


def ધકેલો_outgoing_bits(hse, oi):
    જો hse.outgoing_bits_ગણતરી > 8:
        ગણતરી = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_ગણતરી - 8)
    નહીંતર:
        ગણતરી = hse.outgoing_bits_ગણતરી
        bits = hse.outgoing_bits

    જો ગણતરી > 0:
        ધકેલો_bits(hse, ગણતરી, bits, oi)
        hse.outgoing_bits_ગણતરી -= ગણતરી
    પાછા ગણતરી


def st_yield_tag_bit(hse, oi):
    જો can_take_byte(oi):
        જો hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            પાછા HSE_state.HSES_YIELD_LITERAL
        નહીંતર:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_ગણતરી = hse.window_size
            પાછા HSE_state.HSES_YIELD_BR_INDEX
    નહીંતર:
        પાછા HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    જો can_take_byte(oi):
        ધકેલો_literal_byte(hse, oi)
        પાછા HSE_state.HSES_શોધ
    નહીંતર:
        પાછા HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    જો can_take_byte(oi):
        જો ધકેલો_outgoing_bits(hse, oi) > 0:
            પાછા HSE_state.HSES_YIELD_BR_INDEX
        નહીંતર:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_ગણતરી = hse.lookahead_size
            પાછા HSE_state.HSES_YIELD_BR_LENGTH
    નહીંતર:
        પાછા HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    જો can_take_byte(oi):
        જો ધકેલો_outgoing_bits(hse, oi) > 0:
            પાછા HSE_state.HSES_YIELD_BR_LENGTH
        નહીંતર:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            પાછા HSE_state.HSES_શોધ
    નહીંતર:
        પાછા HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    પાછા HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    જો hse.bit_index == 0x80:
        પાછા HSE_state.HSES_DONE
    અથવાજો can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        પાછા HSE_state.HSES_DONE
    નહીંતર:
        પાછા HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    આગત_buf_sz = get_આગત_buffer_size(hse)

    msi = hse.match_scan_index

    rem = આગત_buf_sz - msi  # unprocessed bytes
    shift_sz = આગત_buf_sz + rem

    માટે i માં range(shift_sz):
        hse.buffer[i] = hse.buffer[આગત_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.આગત_size -= આગત_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    આગત_buf = (ctypes.c_uint8 * len(data))()
    માટે i, d માં enumerate(data):
        આગત_buf[i] = d
    in_size = len(આગત_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    જ્યારે sunk < in_size:
        આગત_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, આગત_buf, in_size - sunk, આગત_size)
        આગત_buf = આગત_buf[આગત_size.કિંમત:]
        sunk += આગત_size.કિંમત
        જો sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        જ્યારે poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        જો sunk == in_size:
            heatshrink_encoder_finish(hse)
    પાછા ret
