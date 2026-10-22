आयात ctypes
तः enum आयात Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
वर्ग HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_शोध = 2
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
    ढकला_bits(hse, 1, tag, oi)


def ढकला_bits(hse, मोजणी, bits, oi):
    प्रतिज्ञा मोजणी <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    साठी i मध्ये range(मोजणी - 1, -1, -1):
        bit = bits & (1 << i)
        जर bit:
            current_byte.मूल्य |= bit_index.मूल्य
        bit_index.मूल्य >>= 1
        जर bit_index.मूल्य == 0:
            bit_index.मूल्य = 0x80
            oi.buf[oi.output_size[0]] = current_byte.मूल्य
            oi.output_size[0] += 1
            current_byte.मूल्य = 0
    hse.current_byte = current_byte.मूल्य
    hse.bit_index = bit_index.मूल्य


def ढकला_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    आदान_offset = get_आदान_offset(hse) + processed_offset
    c = hse.buffer[आदान_offset]
    ढकला_bits(hse, 8, c, oi)


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
        परम().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.शोध_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


वर्ग hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        परम().__init__()
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
    परत hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    परत oi.output_size[0] < oi.buf_size


def get_आदान_buffer_size(hse):
    परत 1 << hse.window_size


def get_lookahead_size(hse):
    परत 1 << hse.lookahead_size


def get_आदान_offset(hse):
    परत get_आदान_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, आदान_size):
    जर hse is None किंवा in_buf is None किंवा आदान_size is None:
        परत HSE_sink_res.HSER_SINK_ERROR_NULL

    जर is_finishing(hse):
        परत HSE_sink_res.HSER_SINK_ERROR_MISUSE

    जर hse.state != HSE_state.HSES_NOT_FULL:
        परत HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_आदान_offset(hse) + hse.आदान_size
    ibs = get_आदान_buffer_size(hse)
    rem = ibs - hse.आदान_size
    cp_sz = min(rem, size)

    साठी i मध्ये range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    आदान_size.मूल्य = cp_sz
    hse.आदान_size += cp_sz

    जर cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    परत HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.शोध_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    आदान_offset = get_आदान_offset(hse)
    end = आदान_offset + hse.आदान_size

    साठी i मध्ये range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    जर hse is None किंवा out_buf is None किंवा output_size is None:
        परत HSE_poll_res.HSER_POLL_ERROR_NULL

    जर out_buf_size == 0:
        परत HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    पर्यंत सत्य:
        in_state = hse.state
        जर in_state == HSE_state.HSES_NOT_FULL:
            परत HSE_poll_res.HSER_POLL_EMPTY
        किंवाजर in_state == HSE_state.HSES_DONE:
            परत HSE_poll_res.HSER_POLL_EMPTY
        किंवाजर in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_शोध
        किंवाजर in_state == HSE_state.HSES_शोध:
            hse.state = st_step_शोध(hse)
        किंवाजर in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        किंवाजर in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        किंवाजर in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        किंवाजर in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        किंवाजर in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        किंवाजर in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        अन्यथा:
            परत HSE_poll_res.HSER_POLL_ERROR_MISUSE

        जर hse.state == in_state:
            जर oi.output_size == oi.buf_size:
                परत HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    जर hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    जर hse.state == HSE_state.HSES_DONE:
        परत HSE_finish_res.HSER_FINISH_DONE
    परत HSE_finish_res.HSER_FINISH_MORE


def st_step_शोध(hse):
    window_length = get_आदान_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    जर msi > hse.आदान_size - (1 जर fin अन्यथा lookahead_sz):
        परत HSE_state.HSES_FLUSH_BITS जर fin अन्यथा HSE_state.HSES_SAVE_BACKLOG

    आदान_offset = get_आदान_offset(hse)
    end = आदान_offset + msi
    start = end - window_length

    max_possible = lookahead_sz जर hse.आदान_size - msi >= lookahead_sz अन्यथा hse.आदान_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    जर match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        परत HSE_state.HSES_YIELD_TAG_BIT
    अन्यथा:
        hse.match_pos = match_pos
        hse.match_length = match_length
        परत HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.शोध_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    पर्यंत pos >= start:

        जर buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.शोध_index.index[pos]
            सुरूठेवा

        length = 1
        साठी length मध्ये range(1, maxlen):
            जर buf[pos + length] != buf[needlepoint + length]:
                थांबा
        जर length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            जर length == maxlen:
                थांबा  # won't find better

        pos = hse.शोध_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    जर match_maxlen > (break_even_point // 8):
        परत end - match_index, match_maxlen
    परत MATCH_NOT_FOUND, 0


def ढकला_outgoing_bits(hse, oi):
    जर hse.outgoing_bits_मोजणी > 8:
        मोजणी = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_मोजणी - 8)
    अन्यथा:
        मोजणी = hse.outgoing_bits_मोजणी
        bits = hse.outgoing_bits

    जर मोजणी > 0:
        ढकला_bits(hse, मोजणी, bits, oi)
        hse.outgoing_bits_मोजणी -= मोजणी
    परत मोजणी


def st_yield_tag_bit(hse, oi):
    जर can_take_byte(oi):
        जर hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            परत HSE_state.HSES_YIELD_LITERAL
        अन्यथा:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_मोजणी = hse.window_size
            परत HSE_state.HSES_YIELD_BR_INDEX
    अन्यथा:
        परत HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    जर can_take_byte(oi):
        ढकला_literal_byte(hse, oi)
        परत HSE_state.HSES_शोध
    अन्यथा:
        परत HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    जर can_take_byte(oi):
        जर ढकला_outgoing_bits(hse, oi) > 0:
            परत HSE_state.HSES_YIELD_BR_INDEX
        अन्यथा:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_मोजणी = hse.lookahead_size
            परत HSE_state.HSES_YIELD_BR_LENGTH
    अन्यथा:
        परत HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    जर can_take_byte(oi):
        जर ढकला_outgoing_bits(hse, oi) > 0:
            परत HSE_state.HSES_YIELD_BR_LENGTH
        अन्यथा:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            परत HSE_state.HSES_शोध
    अन्यथा:
        परत HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    परत HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    जर hse.bit_index == 0x80:
        परत HSE_state.HSES_DONE
    किंवाजर can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        परत HSE_state.HSES_DONE
    अन्यथा:
        परत HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    आदान_buf_sz = get_आदान_buffer_size(hse)

    msi = hse.match_scan_index

    rem = आदान_buf_sz - msi  # unprocessed bytes
    shift_sz = आदान_buf_sz + rem

    साठी i मध्ये range(shift_sz):
        hse.buffer[i] = hse.buffer[आदान_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.आदान_size -= आदान_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    आदान_buf = (ctypes.c_uint8 * len(data))()
    साठी i, d मध्ये enumerate(data):
        आदान_buf[i] = d
    in_size = len(आदान_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    पर्यंत sunk < in_size:
        आदान_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, आदान_buf, in_size - sunk, आदान_size)
        आदान_buf = आदान_buf[आदान_size.मूल्य:]
        sunk += आदान_size.मूल्य
        जर sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        पर्यंत poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        जर sunk == in_size:
            heatshrink_encoder_finish(hse)
    परत ret
