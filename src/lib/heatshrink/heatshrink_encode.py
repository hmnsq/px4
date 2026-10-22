இறக்குமதி ctypes
தஃ enum இறக்குமதி Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
வகுப்பு HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_தேடல் = 2
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
வகுப்பு output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_size_t),
        ("output_size", ctypes.POINTER(ctypes.c_size_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    தள்ளு_bits(hse, 1, tag, oi)


def தள்ளு_bits(hse, மொத்தம், bits, oi):
    ப்ரதிஜ்ஞா மொத்தம் <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_index = ctypes.c_uint8(hse.bit_index)
    க்கு i உள் range(மொத்தம் - 1, -1, -1):
        bit = bits & (1 << i)
        எனில் bit:
            current_byte.மதிப்பு |= bit_index.மதிப்பு
        bit_index.மதிப்பு >>= 1
        எனில் bit_index.மதிப்பு == 0:
            bit_index.மதிப்பு = 0x80
            oi.buf[oi.output_size[0]] = current_byte.மதிப்பு
            oi.output_size[0] += 1
            current_byte.மதிப்பு = 0
    hse.current_byte = current_byte.மதிப்பு
    hse.bit_index = bit_index.மதிப்பு


def தள்ளு_literal_byte(hse, oi):
    processed_offset = hse.match_scan_index - 1
    உள்ளீடு_offset = get_உள்ளீடு_offset(hse) + processed_offset
    c = hse.buffer[உள்ளீடு_offset]
    தள்ளு_bits(hse, 8, c, oi)


# Define necessary structures and enums
வகுப்பு heatshrink_encoder(ctypes.Structure):
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
        மேல்().__init__()
        self.window_size = window_size
        self.lookahead_size = lookahead_size
        self.தேடல்_index = hs_index(window_size)
        self.buffer = (ctypes.c_uint8 * (2 << window_size))()
        self.bit_index = 0x80


வகுப்பு hs_index(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_size):
        மேல்().__init__()
        self.index = (ctypes.c_int16 * (2 << window_size))()


வகுப்பு HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


வகுப்பு HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


வகுப்பு HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    திரும்பு hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    திரும்பு oi.output_size[0] < oi.buf_size


def get_உள்ளீடு_buffer_size(hse):
    திரும்பு 1 << hse.window_size


def get_lookahead_size(hse):
    திரும்பு 1 << hse.lookahead_size


def get_உள்ளீடு_offset(hse):
    திரும்பு get_உள்ளீடு_buffer_size(hse)


def heatshrink_encoder_sink(hse, in_buf, size, உள்ளீடு_size):
    எனில் hse is None அல்லது in_buf is None அல்லது உள்ளீடு_size is None:
        திரும்பு HSE_sink_res.HSER_SINK_ERROR_NULL

    எனில் is_finishing(hse):
        திரும்பு HSE_sink_res.HSER_SINK_ERROR_MISUSE

    எனில் hse.state != HSE_state.HSES_NOT_FULL:
        திரும்பு HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_உள்ளீடு_offset(hse) + hse.உள்ளீடு_size
    ibs = get_உள்ளீடு_buffer_size(hse)
    rem = ibs - hse.உள்ளீடு_size
    cp_sz = min(rem, size)

    க்கு i உள் range(cp_sz):
        hse.buffer[write_offset + i] = in_buf[i]
    உள்ளீடு_size.மதிப்பு = cp_sz
    hse.உள்ளீடு_size += cp_sz

    எனில் cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    திரும்பு HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.தேடல்_index
    last = [0xffff] * 256
    buf = hse.buffer
    index = hsi.index
    உள்ளீடு_offset = get_உள்ளீடு_offset(hse)
    end = உள்ளீடு_offset + hse.உள்ளீடு_size

    க்கு i உள் range(0, end):
        v = buf[i]
        lv = last[v]
        index[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size):
    எனில் hse is None அல்லது out_buf is None அல்லது output_size is None:
        திரும்பு HSE_poll_res.HSER_POLL_ERROR_NULL

    எனில் out_buf_size == 0:
        திரும்பு HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_size[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_size = out_buf_size
    oi.output_size = output_size

    போது உண்மை:
        in_state = hse.state
        எனில் in_state == HSE_state.HSES_NOT_FULL:
            திரும்பு HSE_poll_res.HSER_POLL_EMPTY
        அல்லதெனில் in_state == HSE_state.HSES_DONE:
            திரும்பு HSE_poll_res.HSER_POLL_EMPTY
        அல்லதெனில் in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_தேடல்
        அல்லதெனில் in_state == HSE_state.HSES_தேடல்:
            hse.state = st_step_தேடல்(hse)
        அல்லதெனில் in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        அல்லதெனில் in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        அல்லதெனில் in_state == HSE_state.HSES_YIELD_BR_INDEX:
            hse.state = st_yield_br_index(hse, oi)
        அல்லதெனில் in_state == HSE_state.HSES_YIELD_BR_LENGTH:
            hse.state = st_yield_br_length(hse, oi)
        அல்லதெனில் in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        அல்லதெனில் in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_buffer(hse, oi)
        இல்லையெனில்:
            திரும்பு HSE_poll_res.HSER_POLL_ERROR_MISUSE

        எனில் hse.state == in_state:
            எனில் oi.output_size == oi.buf_size:
                திரும்பு HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    எனில் hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    எனில் hse.state == HSE_state.HSES_DONE:
        திரும்பு HSE_finish_res.HSER_FINISH_DONE
    திரும்பு HSE_finish_res.HSER_FINISH_MORE


def st_step_தேடல்(hse):
    window_length = get_உள்ளீடு_buffer_size(hse)
    lookahead_sz = get_lookahead_size(hse)
    msi = hse.match_scan_index

    fin = is_finishing(hse)
    எனில் msi > hse.உள்ளீடு_size - (1 எனில் fin இல்லையெனில் lookahead_sz):
        திரும்பு HSE_state.HSES_FLUSH_BITS எனில் fin இல்லையெனில் HSE_state.HSES_SAVE_BACKLOG

    உள்ளீடு_offset = get_உள்ளீடு_offset(hse)
    end = உள்ளீடு_offset + msi
    start = end - window_length

    max_possible = lookahead_sz எனில் hse.உள்ளீடு_size - msi >= lookahead_sz இல்லையெனில் hse.உள்ளீடு_size - msi

    match_pos, match_length = find_longest_match(hse, start, end, max_possible)

    எனில் match_pos == MATCH_NOT_FOUND:
        hse.match_scan_index += 1
        hse.match_length = 0
        திரும்பு HSE_state.HSES_YIELD_TAG_BIT
    இல்லையெனில்:
        hse.match_pos = match_pos
        hse.match_length = match_length
        திரும்பு HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.buffer

    match_maxlen = 0
    match_index = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.தேடல்_index.index[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    போது pos >= start:

        எனில் buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.தேடல்_index.index[pos]
            தொடர்

        length = 1
        க்கு length உள் range(1, maxlen):
            எனில் buf[pos + length] != buf[needlepoint + length]:
                நிறுத்து
        எனில் length > match_maxlen:
            match_maxlen = length
            match_index = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            எனில் length == maxlen:
                நிறுத்து  # won't find better

        pos = hse.தேடல்_index.index[pos]

    break_even_point = 1 + hse.window_size + hse.lookahead_size

    எனில் match_maxlen > (break_even_point // 8):
        திரும்பு end - match_index, match_maxlen
    திரும்பு MATCH_NOT_FOUND, 0


def தள்ளு_outgoing_bits(hse, oi):
    எனில் hse.outgoing_bits_மொத்தம் > 8:
        மொத்தம் = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_மொத்தம் - 8)
    இல்லையெனில்:
        மொத்தம் = hse.outgoing_bits_மொத்தம்
        bits = hse.outgoing_bits

    எனில் மொத்தம் > 0:
        தள்ளு_bits(hse, மொத்தம், bits, oi)
        hse.outgoing_bits_மொத்தம் -= மொத்தம்
    திரும்பு மொத்தம்


def st_yield_tag_bit(hse, oi):
    எனில் can_take_byte(oi):
        எனில் hse.match_length == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            திரும்பு HSE_state.HSES_YIELD_LITERAL
        இல்லையெனில்:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_மொத்தம் = hse.window_size
            திரும்பு HSE_state.HSES_YIELD_BR_INDEX
    இல்லையெனில்:
        திரும்பு HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    எனில் can_take_byte(oi):
        தள்ளு_literal_byte(hse, oi)
        திரும்பு HSE_state.HSES_தேடல்
    இல்லையெனில்:
        திரும்பு HSE_state.HSES_YIELD_LITERAL


def st_yield_br_index(hse, oi):
    எனில் can_take_byte(oi):
        எனில் தள்ளு_outgoing_bits(hse, oi) > 0:
            திரும்பு HSE_state.HSES_YIELD_BR_INDEX
        இல்லையெனில்:
            hse.outgoing_bits = hse.match_length - 1
            hse.outgoing_bits_மொத்தம் = hse.lookahead_size
            திரும்பு HSE_state.HSES_YIELD_BR_LENGTH
    இல்லையெனில்:
        திரும்பு HSE_state.HSES_YIELD_BR_INDEX


def st_yield_br_length(hse, oi):
    எனில் can_take_byte(oi):
        எனில் தள்ளு_outgoing_bits(hse, oi) > 0:
            திரும்பு HSE_state.HSES_YIELD_BR_LENGTH
        இல்லையெனில்:
            hse.match_scan_index += hse.match_length
            hse.match_length = 0
            திரும்பு HSE_state.HSES_தேடல்
    இல்லையெனில்:
        திரும்பு HSE_state.HSES_YIELD_BR_LENGTH


def st_save_backlog(hse):
    save_backlog(hse)
    திரும்பு HSE_state.HSES_NOT_FULL


def st_flush_bit_buffer(hse, oi):
    எனில் hse.bit_index == 0x80:
        திரும்பு HSE_state.HSES_DONE
    அல்லதெனில் can_take_byte(oi):
        oi.buf[oi.output_size[0]] = hse.current_byte
        oi.output_size[0] += 1
        திரும்பு HSE_state.HSES_DONE
    இல்லையெனில்:
        திரும்பு HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    உள்ளீடு_buf_sz = get_உள்ளீடு_buffer_size(hse)

    msi = hse.match_scan_index

    rem = உள்ளீடு_buf_sz - msi  # unprocessed bytes
    shift_sz = உள்ளீடு_buf_sz + rem

    க்கு i உள் range(shift_sz):
        hse.buffer[i] = hse.buffer[உள்ளீடு_buf_sz - rem + i]

    hse.match_scan_index = 0
    hse.உள்ளீடு_size -= உள்ளீடு_buf_sz - rem


def encode(data, window_size, lookahead_size):
    hse = heatshrink_encoder(window_size, lookahead_size)
    உள்ளீடு_buf = (ctypes.c_uint8 * len(data))()
    க்கு i, d உள் enumerate(data):
        உள்ளீடு_buf[i] = d
    in_size = len(உள்ளீடு_buf)

    out_buf_size = 4 * in_size  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_size)()

    sunk = 0
    ret = []
    போது sunk < in_size:
        உள்ளீடு_size = ctypes.c_size_t(in_size)
        heatshrink_encoder_sink(hse, உள்ளீடு_buf, in_size - sunk, உள்ளீடு_size)
        உள்ளீடு_buf = உள்ளீடு_buf[உள்ளீடு_size.மதிப்பு:]
        sunk += உள்ளீடு_size.மதிப்பு
        எனில் sunk == in_size:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        போது poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_size = (ctypes.c_size_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_size, output_size)
            ret += list(out_buf)[0:output_size[0]]

        எனில் sunk == in_size:
            heatshrink_encoder_finish(hse)
    திரும்பு ret
