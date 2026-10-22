আমদানি ctypes
তঃ enum আমদানি Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
শ্রেণী HSE_state(ctypes.c_int):
    HSES_NOT_FULL = 0
    HSES_FILLED = 1
    HSES_অনুসন্ধান = 2
    HSES_YIELD_TAG_BIT = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_সূচক = 5
    HSES_YIELD_BR_দৈর্ঘ্য = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_BITS = 8
    HSES_DONE = 9


# Constants
FLAG_IS_FINISHING = 0x01
MATCH_NOT_FOUND = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
শ্রেণী output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_আকার_t),
        ("output_size", ctypes.POINTER(ctypes.c_আকার_t))
    ]


# Functions
def add_tag_bit(hse, oi, tag):
    ঠেলো_bits(hse, 1, tag, oi)


def ঠেলো_bits(hse, গুনতি, bits, oi):
    প্রতিজ্ঞা গুনতি <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_সূচক = ctypes.c_uint8(hse.bit_সূচক)
    জন্য i মধ্যে range(গুনতি - 1, -1, -1):
        bit = bits & (1 << i)
        যদি bit:
            current_byte.মান |= bit_সূচক.মান
        bit_সূচক.মান >>= 1
        যদি bit_সূচক.মান == 0:
            bit_সূচক.মান = 0x80
            oi.buf[oi.output_আকার[0]] = current_byte.মান
            oi.output_আকার[0] += 1
            current_byte.মান = 0
    hse.current_byte = current_byte.মান
    hse.bit_সূচক = bit_সূচক.মান


def ঠেলো_literal_byte(hse, oi):
    processed_offset = hse.match_scan_সূচক - 1
    প্রবেশ_offset = get_প্রবেশ_offset(hse) + processed_offset
    c = hse.বাফার_2[প্রবেশ_offset]
    ঠেলো_bits(hse, 8, c, oi)


# Define necessary structures and enums
শ্রেণী heatshrink_encoder(ctypes.Structure):
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

    def __init__(self, window_আকার=8, lookahead_আকার=4):
        পরম().__init__()
        self.window_আকার = window_আকার
        self.lookahead_আকার = lookahead_আকার
        self.অনুসন্ধান_সূচক = hs_সূচক(window_আকার)
        self.বাফার_2 = (ctypes.c_uint8 * (2 << window_আকার))()
        self.bit_সূচক = 0x80


শ্রেণী hs_সূচক(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    def __init__(self, window_আকার):
        পরম().__init__()
        self.সূচক = (ctypes.c_int16 * (2 << window_আকার))()


শ্রেণী HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


শ্রেণী HSE_poll_res(Enum):
    HSER_POLL_EMPTY = 0
    HSER_POLL_MORE = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


শ্রেণী HSE_finish_res(Enum):
    HSER_FINISH_DONE = 0
    HSER_FINISH_MORE = 1
    HSER_FINISH_ERROR_NULL = -1


def is_finishing(hse):
    ফেরত hse.flags & FLAG_IS_FINISHING


def can_take_byte(oi):
    ফেরত oi.output_আকার[0] < oi.buf_আকার


def get_প্রবেশ_বাফার_আকার(hse):
    ফেরত 1 << hse.window_আকার


def get_lookahead_আকার(hse):
    ফেরত 1 << hse.lookahead_আকার


def get_প্রবেশ_offset(hse):
    ফেরত get_প্রবেশ_বাফার_আকার(hse)


def heatshrink_encoder_sink(hse, in_buf, আকার_2, প্রবেশ_আকার):
    যদি hse is None অথবা in_buf is None অথবা প্রবেশ_আকার is None:
        ফেরত HSE_sink_res.HSER_SINK_ERROR_NULL

    যদি is_finishing(hse):
        ফেরত HSE_sink_res.HSER_SINK_ERROR_MISUSE

    যদি hse.state != HSE_state.HSES_NOT_FULL:
        ফেরত HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_offset = get_প্রবেশ_offset(hse) + hse.প্রবেশ_আকার
    ibs = get_প্রবেশ_বাফার_আকার(hse)
    rem = ibs - hse.প্রবেশ_আকার
    cp_sz = min(rem, আকার_2)

    জন্য i মধ্যে range(cp_sz):
        hse.বাফার_2[write_offset + i] = in_buf[i]
    প্রবেশ_আকার.মান = cp_sz
    hse.প্রবেশ_আকার += cp_sz

    যদি cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    ফেরত HSE_sink_res.HSER_SINK_OK


def do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.অনুসন্ধান_সূচক
    last = [0xffff] * 256
    buf = hse.বাফার_2
    সূচক = hsi.সূচক
    প্রবেশ_offset = get_প্রবেশ_offset(hse)
    end = প্রবেশ_offset + hse.প্রবেশ_আকার

    জন্য i মধ্যে range(0, end):
        v = buf[i]
        lv = last[v]
        সূচক[i] = lv
        last[v] = i


def heatshrink_encoder_poll(hse, out_buf, out_buf_আকার, output_আকার):
    যদি hse is None অথবা out_buf is None অথবা output_আকার is None:
        ফেরত HSE_poll_res.HSER_POLL_ERROR_NULL

    যদি out_buf_আকার == 0:
        ফেরত HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_আকার[0] = 0
    oi = output_info()
    oi.buf = out_buf
    oi.buf_আকার = out_buf_আকার
    oi.output_আকার = output_আকার

    যখন সত্য:
        in_state = hse.state
        যদি in_state == HSE_state.HSES_NOT_FULL:
            ফেরত HSE_poll_res.HSER_POLL_EMPTY
        নতুবা_যদি in_state == HSE_state.HSES_DONE:
            ফেরত HSE_poll_res.HSER_POLL_EMPTY
        নতুবা_যদি in_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_অনুসন্ধান
        নতুবা_যদি in_state == HSE_state.HSES_অনুসন্ধান:
            hse.state = st_step_অনুসন্ধান(hse)
        নতুবা_যদি in_state == HSE_state.HSES_YIELD_TAG_BIT:
            hse.state = st_yield_tag_bit(hse, oi)
        নতুবা_যদি in_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        নতুবা_যদি in_state == HSE_state.HSES_YIELD_BR_সূচক:
            hse.state = st_yield_br_সূচক(hse, oi)
        নতুবা_যদি in_state == HSE_state.HSES_YIELD_BR_দৈর্ঘ্য:
            hse.state = st_yield_br_দৈর্ঘ্য(hse, oi)
        নতুবা_যদি in_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        নতুবা_যদি in_state == HSE_state.HSES_FLUSH_BITS:
            hse.state = st_flush_bit_বাফার(hse, oi)
        অন্যথা:
            ফেরত HSE_poll_res.HSER_POLL_ERROR_MISUSE

        যদি hse.state == in_state:
            যদি oi.output_আকার == oi.buf_আকার:
                ফেরত HSE_poll_res.HSER_POLL_MORE


def heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    যদি hse.state == HSE_state.HSES_NOT_FULL:
        hse.state = HSE_state.HSES_FILLED
    যদি hse.state == HSE_state.HSES_DONE:
        ফেরত HSE_finish_res.HSER_FINISH_DONE
    ফেরত HSE_finish_res.HSER_FINISH_MORE


def st_step_অনুসন্ধান(hse):
    window_দৈর্ঘ্য = get_প্রবেশ_বাফার_আকার(hse)
    lookahead_sz = get_lookahead_আকার(hse)
    msi = hse.match_scan_সূচক

    fin = is_finishing(hse)
    যদি msi > hse.প্রবেশ_আকার - (1 যদি fin অন্যথা lookahead_sz):
        ফেরত HSE_state.HSES_FLUSH_BITS যদি fin অন্যথা HSE_state.HSES_SAVE_BACKLOG

    প্রবেশ_offset = get_প্রবেশ_offset(hse)
    end = প্রবেশ_offset + msi
    start = end - window_দৈর্ঘ্য

    max_possible = lookahead_sz যদি hse.প্রবেশ_আকার - msi >= lookahead_sz অন্যথা hse.প্রবেশ_আকার - msi

    match_pos, match_দৈর্ঘ্য = find_longest_match(hse, start, end, max_possible)

    যদি match_pos == MATCH_NOT_FOUND:
        hse.match_scan_সূচক += 1
        hse.match_দৈর্ঘ্য = 0
        ফেরত HSE_state.HSES_YIELD_TAG_BIT
    অন্যথা:
        hse.match_pos = match_pos
        hse.match_দৈর্ঘ্য = match_দৈর্ঘ্য
        ফেরত HSE_state.HSES_YIELD_TAG_BIT


def find_longest_match(hse, start, end, maxlen):
    buf = hse.বাফার_2

    match_maxlen = 0
    match_সূচক = MATCH_NOT_FOUND

    needlepoint = end

    pos = hse.অনুসন্ধান_সূচক.সূচক[end]
    buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
    যখন pos >= start:

        যদি buf[pos + match_maxlen] != buf_needlepoint_maxlen:
            pos = hse.অনুসন্ধান_সূচক.সূচক[pos]
            চালু

        দৈর্ঘ্য = 1
        জন্য দৈর্ঘ্য মধ্যে range(1, maxlen):
            যদি buf[pos + দৈর্ঘ্য] != buf[needlepoint + দৈর্ঘ্য]:
                বিরতি
        যদি দৈর্ঘ্য > match_maxlen:
            match_maxlen = দৈর্ঘ্য
            match_সূচক = pos
            buf_needlepoint_maxlen = buf[needlepoint + match_maxlen]
            যদি দৈর্ঘ্য == maxlen:
                বিরতি  # won't find better

        pos = hse.অনুসন্ধান_সূচক.সূচক[pos]

    break_even_point = 1 + hse.window_আকার + hse.lookahead_আকার

    যদি match_maxlen > (break_even_point // 8):
        ফেরত end - match_সূচক, match_maxlen
    ফেরত MATCH_NOT_FOUND, 0


def ঠেলো_outgoing_bits(hse, oi):
    যদি hse.outgoing_bits_গুনতি > 8:
        গুনতি = 8
        bits = hse.outgoing_bits >> (hse.outgoing_bits_গুনতি - 8)
    অন্যথা:
        গুনতি = hse.outgoing_bits_গুনতি
        bits = hse.outgoing_bits

    যদি গুনতি > 0:
        ঠেলো_bits(hse, গুনতি, bits, oi)
        hse.outgoing_bits_গুনতি -= গুনতি
    ফেরত গুনতি


def st_yield_tag_bit(hse, oi):
    যদি can_take_byte(oi):
        যদি hse.match_দৈর্ঘ্য == 0:
            add_tag_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            ফেরত HSE_state.HSES_YIELD_LITERAL
        অন্যথা:
            add_tag_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_bits = hse.match_pos - 1
            hse.outgoing_bits_গুনতি = hse.window_আকার
            ফেরত HSE_state.HSES_YIELD_BR_সূচক
    অন্যথা:
        ফেরত HSE_state.HSES_YIELD_TAG_BIT


def st_yield_literal(hse, oi):
    যদি can_take_byte(oi):
        ঠেলো_literal_byte(hse, oi)
        ফেরত HSE_state.HSES_অনুসন্ধান
    অন্যথা:
        ফেরত HSE_state.HSES_YIELD_LITERAL


def st_yield_br_সূচক(hse, oi):
    যদি can_take_byte(oi):
        যদি ঠেলো_outgoing_bits(hse, oi) > 0:
            ফেরত HSE_state.HSES_YIELD_BR_সূচক
        অন্যথা:
            hse.outgoing_bits = hse.match_দৈর্ঘ্য - 1
            hse.outgoing_bits_গুনতি = hse.lookahead_আকার
            ফেরত HSE_state.HSES_YIELD_BR_দৈর্ঘ্য
    অন্যথা:
        ফেরত HSE_state.HSES_YIELD_BR_সূচক


def st_yield_br_দৈর্ঘ্য(hse, oi):
    যদি can_take_byte(oi):
        যদি ঠেলো_outgoing_bits(hse, oi) > 0:
            ফেরত HSE_state.HSES_YIELD_BR_দৈর্ঘ্য
        অন্যথা:
            hse.match_scan_সূচক += hse.match_দৈর্ঘ্য
            hse.match_দৈর্ঘ্য = 0
            ফেরত HSE_state.HSES_অনুসন্ধান
    অন্যথা:
        ফেরত HSE_state.HSES_YIELD_BR_দৈর্ঘ্য


def st_save_backlog(hse):
    save_backlog(hse)
    ফেরত HSE_state.HSES_NOT_FULL


def st_flush_bit_বাফার(hse, oi):
    যদি hse.bit_সূচক == 0x80:
        ফেরত HSE_state.HSES_DONE
    নতুবা_যদি can_take_byte(oi):
        oi.buf[oi.output_আকার[0]] = hse.current_byte
        oi.output_আকার[0] += 1
        ফেরত HSE_state.HSES_DONE
    অন্যথা:
        ফেরত HSE_state.HSES_FLUSH_BITS


def save_backlog(hse):
    প্রবেশ_buf_sz = get_প্রবেশ_বাফার_আকার(hse)

    msi = hse.match_scan_সূচক

    rem = প্রবেশ_buf_sz - msi  # unprocessed bytes
    shift_sz = প্রবেশ_buf_sz + rem

    জন্য i মধ্যে range(shift_sz):
        hse.বাফার_2[i] = hse.বাফার_2[প্রবেশ_buf_sz - rem + i]

    hse.match_scan_সূচক = 0
    hse.প্রবেশ_আকার -= প্রবেশ_buf_sz - rem


def encode(তথ্য, window_আকার, lookahead_আকার):
    hse = heatshrink_encoder(window_আকার, lookahead_আকার)
    প্রবেশ_buf = (ctypes.c_uint8 * len(তথ্য))()
    জন্য i, d মধ্যে enumerate(তথ্য):
        প্রবেশ_buf[i] = d
    in_আকার = len(প্রবেশ_buf)

    out_buf_আকার = 4 * in_আকার  # set output buffer size a bit larger
    out_buf = (ctypes.c_uint8 * out_buf_আকার)()

    sunk = 0
    ret = []
    যখন sunk < in_আকার:
        প্রবেশ_আকার = ctypes.c_আকার_t(in_আকার)
        heatshrink_encoder_sink(hse, প্রবেশ_buf, in_আকার - sunk, প্রবেশ_আকার)
        প্রবেশ_buf = প্রবেশ_buf[প্রবেশ_আকার.মান:]
        sunk += প্রবেশ_আকার.মান
        যদি sunk == in_আকার:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_MORE
        যখন poll_res == HSE_poll_res.HSER_POLL_MORE:
            output_আকার = (ctypes.c_আকার_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, out_buf, out_buf_আকার, output_আকার)
            ret += list(out_buf)[0:output_আকার[0]]

        যদি sunk == in_আকার:
            heatshrink_encoder_finish(hse)
    ফেরত ret
