ಆನಯನ ctypes
ತಃ enum ಆನಯನ Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
ವರ್ಗ HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_ಹುಡುಕು = 2
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
ವರ್ಗ output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    ತಳ್ಳು_bits(hse, 1, tag, oi)


def ತಳ್ಳು_bits(hse, ಎಣಿಕೆ, bits, oi):
    ಪ್ರತಿಜ್ಞಾ ಎಣಿಕೆ <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    ಪ್ರತಿ i ಅನ್ತಃ range(ಎಣಿಕೆ - 1, -1, -1):
        bit = bits & (1 << i)
        ಯದಿ bit:
            current_byte.ಮೌಲ್ಯ |= bit_index.ಮೌಲ್ಯ
        bit_index.ಮೌಲ್ಯ >>= 1
        ಯದಿ bit_index.ಮೌಲ್ಯ == 0:
            bit_index.ಮೌಲ್ಯ = 0x80
            oi.buf[oi.output_size[0]] = current_byte.ಮೌಲ್ಯ
            oi.output_size[0] += 1
            current_byte.ಮೌಲ್ಯ = 0
    hse.current_byte = current_byte.ಮೌಲ್ಯ
    hse.bit_index = bit_index.ಮೌಲ್ಯ


def ತಳ್ಳು_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    ಒಳಹರಿವು_offset = get_ಒಳಹರಿವು_offset(hse) + processed_offset
    c = hse.buffer[ಒಳಹರಿವು_offset]
    ತಳ್ಳು_bits(hse, 8, c, oi)


# Define necessary structures and enums
ವರ್ಗ heatshrink_encoder(ctypes.Structure):
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
        ಪರ().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.ಹುಡುಕು_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


ವರ್ಗ hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        ಪರ().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


ವರ್ಗ HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


ವರ್ಗ HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


ವರ್ಗ HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    ಪ್ರತ್ಯಾಗಮ hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    ಪ್ರತ್ಯಾಗಮ oi.output_size[0] < oi.buf_size


def get_ಒಳಹರಿವು_buffer_size(hse):
    ಪ್ರತ್ಯಾಗಮ 1 << hse.window_size


def get_lookahead_size(hse):
    ಪ್ರತ್ಯಾಗಮ 1 << hse.lookahead_size


def get_ಒಳಹರಿವು_offset(hse):
    ಪ್ರತ್ಯಾಗಮ get_ಒಳಹರಿವು_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, ಒಳಹರಿವು_size):
    ಯದಿ hse is None ವಾ in_buf is None ವಾ ಒಳಹರಿವು_size is None:
        ಪ್ರತ್ಯಾಗಮ HSE_sink_res.HSER_SINK_ERROR_NULL

    ಯದಿ is_finishing(hse):
        ಪ್ರತ್ಯಾಗಮ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    ಯದಿ hse.state != HSE_state.HSES_NOT_FULL:
        ಪ್ರತ್ಯಾಗಮ HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_ಒಳಹರಿವು_offset(hse) + hse.ಒಳಹರಿವು_size
    ibs = get_ಒಳಹರಿವು_buffer_size(hse)
    rem = ibs - hse.ಒಳಹರಿವು_size
    cp_sz = min(rem, size)

    ಪ್ರತಿ i ಅನ್ತಃ range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    ಒಳಹರಿವು_size.ಮೌಲ್ಯ = cp_sz
    hse.ಒಳಹರಿವು_size += cp_sz

    ಯದಿ cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    ಪ್ರತ್ಯಾಗಮ HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.ಹುಡುಕು_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    ಒಳಹರಿವು_offset = get_ಒಳಹರಿವು_offset(hse)
    end = ಒಳಹರಿವು_offset + hse.ಒಳಹರಿವು_size

    ಪ್ರತಿ i ಅನ್ತಃ range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    ಯದಿ hse is None ವಾ out_buf is None ವಾ output_size is None:
        ಪ್ರತ್ಯಾಗಮ HSE_poll_res.HSER_POLL_ERROR_NULL

    ಯದಿ out_buf_size == 0:
        ಪ್ರತ್ಯಾಗಮ HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    ಯಾವತ್ ಸತ್ಯ:
        in_state = hse.state
        ಯದಿ in_state == HSE_state.HSES_NOT_FULL:
            ಪ್ರತ್ಯಾಗಮ HSE_poll_res.HSER_POLL_EMPTY
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_DONE:
            ಪ್ರತ್ಯಾಗಮ HSE_poll_res.HSER_POLL_EMPTY
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_ಹುಡುಕು
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_ಹುಡುಕು:
            hse.state = st_step_ಹುಡುಕು(hse)
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        ನೋಚೇದ್ಯದಿ in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        ಅನ್ಯಥಾ:
            ಪ್ರತ್ಯಾಗಮ HSE_poll_res.HSER_POLL_ERROR_MISUSE

        ಯದಿ hse.state == in_state:
            ಯದಿ oi.output_size == oi.buf_size:
                ಪ್ರತ್ಯಾಗಮ HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    ಯದಿ hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    ಯದಿ hse.state == HSE_state.HSES_DONE:
        ಪ್ರತ್ಯಾಗಮ HSE_finish_res.HSER_FINISH_DONE
    ಪ್ರತ್ಯಾಗಮ HSE_finish_res.HSER_FINISH_MORE


def st_step_ಹುಡುಕು(hse):
    window_length = get_ಒಳಹರಿವು_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    ಯದಿ msi > hse.ಒಳಹರಿವು_size - (1 ಯದಿ fin ಅನ್ಯಥಾ lookahead_sz):
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_FLUSH_BITS ಯದಿ fin ಅನ್ಯಥಾ HSE_state.HSES_SAVE_BACKLOG

    ಒಳಹರಿವು_offset = get_ಒಳಹರಿವು_offset(hse)
    end = ಒಳಹರಿವು_offset + msi
    start = end - window_length

    max_possible = lookahead_sz ಯದಿ hse.ಒಳಹರಿವು_size - msi >= lookahead_sz ಅನ್ಯಥಾ hse.ಒಳಹರಿವು_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    ಯದಿ match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_TAG_BIT
    ಅನ್ಯಥಾ:
        hse.match_pos = match_pos
        hse.match_length = match_length
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.ಹುಡುಕು_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    ಯಾವತ್ pos >= start:

        ಯದಿ buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.ಹುಡುಕು_index.index[pos]
            ಅನುವರ್ತ

        length = 1
        ಪ್ರತಿ length ಅನ್ತಃ range(1, maxlen):
            ಯದಿ buf[pos + length] != buf[needlepoint + length]:
                ಭಙ್ಗ
        ಯದಿ length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            ಯದಿ length == maxlen:
                ಭಙ್ಗ  # won't find better

        pos = hse.ಹುಡುಕು_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    ಯದಿ match_maxlen > (break_even_point // 8):
        ಪ್ರತ್ಯಾಗಮ end - match_index, match_maxlen
    ಪ್ರತ್ಯಾಗಮ MATCH_NOT_FOUND, 0


def ತಳ್ಳು_outgoing_bits(hse, oi):
    ಯದಿ hse.outgoing_bits_ಎಣಿಕೆ > 8:
        ಎಣಿಕೆ = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_ಎಣಿಕೆ - 8)
    ಅನ್ಯಥಾ:
        ಎಣಿಕೆ = hse.outgoing_bits_ಎಣಿಕೆ
        bits = hse.outgoing_bits

    ಯದಿ ಎಣಿಕೆ > 0:
        ತಳ್ಳು_bits(hse, ಎಣಿಕೆ, bits, oi)
        hse.outgoing_bits_ಎಣಿಕೆ -= ಎಣಿಕೆ
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ


def st_yield_tag_bit(hse, oi):
    ಯದಿ can_take_byte(oi):
        ಯದಿ hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_LITERAL
        ಅನ್ಯಥಾ:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_ಎಣಿಕೆ = hse.window_size
            ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_BR_INDEX
    ಅನ್ಯಥಾ:
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    ಯದಿ can_take_byte(oi):
        ತಳ್ಳು_literal_byte(hse, oi)
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_ಹುಡುಕು
    ಅನ್ಯಥಾ:
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    ಯದಿ can_take_byte(oi):
        ಯದಿ ತಳ್ಳು_outgoing_bits(hse, oi) > 0:
            ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_BR_INDEX
        ಅನ್ಯಥಾ:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_ಎಣಿಕೆ = hse.lookahead_size
            ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_BR_LENGTH
    ಅನ್ಯಥಾ:
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    ಯದಿ can_take_byte(oi):
        ಯದಿ ತಳ್ಳು_outgoing_bits(hse, oi) > 0:
            ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_BR_LENGTH
        ಅನ್ಯಥಾ:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_ಹುಡುಕು
    ಅನ್ಯಥಾ:
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    ಯದಿ hse.bit_index == 0x80:
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_DONE
    ನೋಚೇದ್ಯದಿ can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_DONE
    ಅನ್ಯಥಾ:
        ಪ್ರತ್ಯಾಗಮ HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    ಒಳಹರಿವು_buf_sz = get_ಒಳಹರಿವು_buffer_size(hse)

    msi = hse.match_scan_index

    rem = ಒಳಹರಿವು_buf_sz - msi  # unprocessed bytes
    shift_sz = ಒಳಹರಿವು_buf_sz + rem

    ಪ್ರತಿ i ಅನ್ತಃ range(shift_sz):
        hse.buffer[i] = hse.buffer[ಒಳಹರಿವು_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.ಒಳಹರಿವು_size -= ಒಳಹರಿವು_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    ಒಳಹರಿವು_buf = (ctypes.c_uint8 * len(data))()
    ಪ್ರತಿ i, d ಅನ್ತಃ enumerate(data):
        ಒಳಹರಿವು_buf[i] = d
    in_size = len(ಒಳಹರಿವು_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    ಯಾವತ್ sunk < in_size:
        ಒಳಹರಿವು_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, ಒಳಹರಿವು_buf, in_size - sunk, ಒಳಹರಿವು_size)
        ಒಳಹರಿವು_buf = ಒಳಹರಿವು_buf[ಒಳಹರಿವು_size.ಮೌಲ್ಯ:]
        sunk += ಒಳಹರಿವು_size.ಮೌಲ್ಯ
        ಯದಿ sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        ಯಾವತ್ poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        ಯದಿ sunk == in_size:
            heatshrink_encoder_finish(hse)
    ಪ್ರತ್ಯಾಗಮ ret
