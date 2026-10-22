आनयन ctypes
तः enum आनयन Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
वर्ग HSE_state(ctypes.c_int):
    HSES_NOT_पूर्णम् = 0
    HSES_FILLED = 1
    HSES_अन्वेषणम् = 2
    HSES_YIELD_चिह्नकम्_BIT = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_सूचकांक = 5
    HSES_YIELD_BR_दीर्घता = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_द्वयङ्काः = 8
    HSES_समाप्तम् = 9


# Constants
FLAG_IS_FINISHING = 0x01
मेलनम्_NOT_प्राप्तम् = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
वर्ग output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.POINTER(ctypes.c_uint8)),
        ("buf_size", ctypes.c_आकार_t),
        ("output_size", ctypes.POINTER(ctypes.c_आकार_t))
    ]


# Functions
परिभाषा सङ्कलय_चिह्नकम्_bit(hse, oi, चिह्नकम्):
    प्रक्षिप_द्वयङ्काः(hse, 1, चिह्नकम्, oi)


परिभाषा प्रक्षिप_द्वयङ्काः(hse, गणनम्, द्वयङ्काः, oi):
    प्रतिज्ञा गणनम् <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    bit_सूचकांक = ctypes.c_uint8(hse.bit_सूचकांक)
    प्रति i अन्तः परिसरः(गणनम् - 1, -1, -1):
        bit = द्वयङ्काः & (1 << i)
        यदि bit:
            current_byte.मानम् |= bit_सूचकांक.मानम्
        bit_सूचकांक.मानम् >>= 1
        यदि bit_सूचकांक.मानम् == 0:
            bit_सूचकांक.मानम् = 0x80
            oi.buf[oi.output_आकार[0]] = current_byte.मानम्
            oi.output_आकार[0] += 1
            current_byte.मानम् = 0
    hse.current_byte = current_byte.मानम्
    hse.bit_सूचकांक = bit_सूचकांक.मानम्


परिभाषा प्रक्षिप_literal_byte(hse, oi):
    processed_विस्थापनम् = hse.मेलनम्_अवलोकनम्_सूचकांक - 1
    निवेशः_विस्थापनम् = get_निवेशः_विस्थापनम्(hse) + processed_विस्थापनम्
    c = hse.बफर_2[निवेशः_विस्थापनम्]
    प्रक्षिप_द्वयङ्काः(hse, 8, c, oi)


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

    परिभाषा __init__(self, गवाक्षः_आकार=8, lookahead_आकार=4):
        पर().__init__()
        self.गवाक्षः_आकार = गवाक्षः_आकार
        self.lookahead_आकार = lookahead_आकार
        self.अन्वेषणम्_सूचकांक = hs_सूचकांक(गवाक्षः_आकार)
        self.बफर_2 = (ctypes.c_uint8 * (2 << गवाक्षः_आकार))()
        self.bit_सूचकांक = 0x80


वर्ग hs_सूचकांक(ctypes.Structure):
    _fields_ = [("index", ctypes.POINTER(ctypes.c_int16))]

    परिभाषा __init__(self, गवाक्षः_आकार):
        पर().__init__()
        self.सूचकांक = (ctypes.c_int16 * (2 << गवाक्षः_आकार))()


वर्ग HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


वर्ग HSE_poll_res(Enum):
    HSER_POLL_रिक्तानि = 0
    HSER_POLL_अधिकम् = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


वर्ग HSE_finish_res(Enum):
    HSER_FINISH_समाप्तम् = 0
    HSER_FINISH_अधिकम् = 1
    HSER_FINISH_ERROR_NULL = -1


परिभाषा is_finishing(hse):
    प्रत्यागम hse.flags & FLAG_IS_FINISHING


परिभाषा can_take_byte(oi):
    प्रत्यागम oi.output_आकार[0] < oi.buf_आकार


परिभाषा get_निवेशः_बफर_आकार(hse):
    प्रत्यागम 1 << hse.गवाक्षः_आकार


परिभाषा get_lookahead_आकार(hse):
    प्रत्यागम 1 << hse.lookahead_आकार


परिभाषा get_निवेशः_विस्थापनम्(hse):
    प्रत्यागम get_निवेशः_बफर_आकार(hse)


परिभाषा heatshrink_encoder_sink(hse, अन्तर्_buf, आकार, निवेशः_आकार):
    यदि hse अस्ति किमपि_न वा अन्तर्_buf अस्ति किमपि_न वा निवेशः_आकार अस्ति किमपि_न:
        प्रत्यागम HSE_sink_res.HSER_SINK_ERROR_NULL

    यदि is_finishing(hse):
        प्रत्यागम HSE_sink_res.HSER_SINK_ERROR_MISUSE

    यदि hse.state != HSE_state.HSES_NOT_पूर्णम्:
        प्रत्यागम HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_विस्थापनम्_2 = get_निवेशः_विस्थापनम्(hse) + hse.निवेशः_आकार
    ibs = get_निवेशः_बफर_आकार(hse)
    rem = ibs - hse.निवेशः_आकार
    cp_sz = min(rem, आकार)

    प्रति i अन्तः परिसरः(cp_sz):
        hse.बफर_2[write_विस्थापनम्_2 + i] = अन्तर्_buf[i]
    निवेशः_आकार.मानम् = cp_sz
    hse.निवेशः_आकार += cp_sz

    यदि cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    प्रत्यागम HSE_sink_res.HSER_SINK_OK


परिभाषा do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.अन्वेषणम्_सूचकांक
    चरमः = [0xffff] * 256
    buf = hse.बफर_2
    सूचकांक = hsi.सूचकांक
    निवेशः_विस्थापनम् = get_निवेशः_विस्थापनम्(hse)
    अन्तः_2 = निवेशः_विस्थापनम् + hse.निवेशः_आकार

    प्रति i अन्तः परिसरः(0, अन्तः_2):
        v = buf[i]
        lv = चरमः[v]
        सूचकांक[i] = lv
        चरमः[v] = i


परिभाषा heatshrink_encoder_poll(hse, बहिर्गमनम्_buf, बहिर्गमनम्_buf_आकार, output_आकार):
    यदि hse अस्ति किमपि_न वा बहिर्गमनम्_buf अस्ति किमपि_न वा output_आकार अस्ति किमपि_न:
        प्रत्यागम HSE_poll_res.HSER_POLL_ERROR_NULL

    यदि बहिर्गमनम्_buf_आकार == 0:
        प्रत्यागम HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_आकार[0] = 0
    oi = output_info()
    oi.buf = बहिर्गमनम्_buf
    oi.buf_आकार = बहिर्गमनम्_buf_आकार
    oi.output_आकार = output_आकार

    यावत् सत्य:
        अन्तर्_state = hse.state
        यदि अन्तर्_state == HSE_state.HSES_NOT_पूर्णम्:
            प्रत्यागम HSE_poll_res.HSER_POLL_रिक्तानि
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_समाप्तम्:
            प्रत्यागम HSE_poll_res.HSER_POLL_रिक्तानि
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_अन्वेषणम्
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_अन्वेषणम्:
            hse.state = st_चरणः_अन्वेषणम्(hse)
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_YIELD_चिह्नकम्_BIT:
            hse.state = st_yield_चिह्नकम्_bit(hse, oi)
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_YIELD_BR_सूचकांक:
            hse.state = st_yield_br_सूचकांक(hse, oi)
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_YIELD_BR_दीर्घता:
            hse.state = st_yield_br_दीर्घता(hse, oi)
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        नोचेद्यदि अन्तर्_state == HSE_state.HSES_FLUSH_द्वयङ्काः:
            hse.state = st_flush_bit_बफर(hse, oi)
        अन्यथा:
            प्रत्यागम HSE_poll_res.HSER_POLL_ERROR_MISUSE

        यदि hse.state == अन्तर्_state:
            यदि oi.output_आकार == oi.buf_आकार:
                प्रत्यागम HSE_poll_res.HSER_POLL_अधिकम्


परिभाषा heatshrink_encoder_finish(hse):
    hse.flags |= FLAG_IS_FINISHING
    यदि hse.state == HSE_state.HSES_NOT_पूर्णम्:
        hse.state = HSE_state.HSES_FILLED
    यदि hse.state == HSE_state.HSES_समाप्तम्:
        प्रत्यागम HSE_finish_res.HSER_FINISH_समाप्तम्
    प्रत्यागम HSE_finish_res.HSER_FINISH_अधिकम्


परिभाषा st_चरणः_अन्वेषणम्(hse):
    गवाक्षः_दीर्घता = get_निवेशः_बफर_आकार(hse)
    lookahead_sz = get_lookahead_आकार(hse)
    msi = hse.मेलनम्_अवलोकनम्_सूचकांक

    fin = is_finishing(hse)
    यदि msi > hse.निवेशः_आकार - (1 यदि fin अन्यथा lookahead_sz):
        प्रत्यागम HSE_state.HSES_FLUSH_द्वयङ्काः यदि fin अन्यथा HSE_state.HSES_SAVE_BACKLOG

    निवेशः_विस्थापनम् = get_निवेशः_विस्थापनम्(hse)
    अन्तः_2 = निवेशः_विस्थापनम् + msi
    आरम्भः = अन्तः_2 - गवाक्षः_दीर्घता

    max_possible = lookahead_sz यदि hse.निवेशः_आकार - msi >= lookahead_sz अन्यथा hse.निवेशः_आकार - msi

    मेलनम्_pos, मेलनम्_दीर्घता = find_longest_मेलनम्(hse, आरम्भः, अन्तः_2, max_possible)

    यदि मेलनम्_pos == मेलनम्_NOT_प्राप्तम्:
        hse.मेलनम्_अवलोकनम्_सूचकांक += 1
        hse.मेलनम्_दीर्घता = 0
        प्रत्यागम HSE_state.HSES_YIELD_चिह्नकम्_BIT
    अन्यथा:
        hse.मेलनम्_pos = मेलनम्_pos
        hse.मेलनम्_दीर्घता = मेलनम्_दीर्घता
        प्रत्यागम HSE_state.HSES_YIELD_चिह्नकम्_BIT


परिभाषा find_longest_मेलनम्(hse, आरम्भः, अन्तः_2, maxlen):
    buf = hse.बफर_2

    मेलनम्_maxlen = 0
    मेलनम्_सूचकांक = मेलनम्_NOT_प्राप्तम्

    needlepoint = अन्तः_2

    pos = hse.अन्वेषणम्_सूचकांक.सूचकांक[अन्तः_2]
    buf_needlepoint_maxlen = buf[needlepoint + मेलनम्_maxlen]
    यावत् pos >= आरम्भः:

        यदि buf[pos + मेलनम्_maxlen] != buf_needlepoint_maxlen:
            pos = hse.अन्वेषणम्_सूचकांक.सूचकांक[pos]
            अनुवर्त

        दीर्घता_2 = 1
        प्रति दीर्घता_2 अन्तः परिसरः(1, maxlen):
            यदि buf[pos + दीर्घता_2] != buf[needlepoint + दीर्घता_2]:
                भङ्ग
        यदि दीर्घता_2 > मेलनम्_maxlen:
            मेलनम्_maxlen = दीर्घता_2
            मेलनम्_सूचकांक = pos
            buf_needlepoint_maxlen = buf[needlepoint + मेलनम्_maxlen]
            यदि दीर्घता_2 == maxlen:
                भङ्ग  # won't find better

        pos = hse.अन्वेषणम्_सूचकांक.सूचकांक[pos]

    break_even_point = 1 + hse.गवाक्षः_आकार + hse.lookahead_आकार

    यदि मेलनम्_maxlen > (break_even_point // 8):
        प्रत्यागम अन्तः_2 - मेलनम्_सूचकांक, मेलनम्_maxlen
    प्रत्यागम मेलनम्_NOT_प्राप्तम्, 0


परिभाषा प्रक्षिप_outgoing_द्वयङ्काः(hse, oi):
    यदि hse.outgoing_द्वयङ्काः_गणनम् > 8:
        गणनम् = 8
        द्वयङ्काः = hse.outgoing_द्वयङ्काः >> (hse.outgoing_द्वयङ्काः_गणनम् - 8)
    अन्यथा:
        गणनम् = hse.outgoing_द्वयङ्काः_गणनम्
        द्वयङ्काः = hse.outgoing_द्वयङ्काः

    यदि गणनम् > 0:
        प्रक्षिप_द्वयङ्काः(hse, गणनम्, द्वयङ्काः, oi)
        hse.outgoing_द्वयङ्काः_गणनम् -= गणनम्
    प्रत्यागम गणनम्


परिभाषा st_yield_चिह्नकम्_bit(hse, oi):
    यदि can_take_byte(oi):
        यदि hse.मेलनम्_दीर्घता == 0:
            सङ्कलय_चिह्नकम्_bit(hse, oi, HEATSHRINK_LITERAL_MARKER)
            प्रत्यागम HSE_state.HSES_YIELD_LITERAL
        अन्यथा:
            सङ्कलय_चिह्नकम्_bit(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_द्वयङ्काः = hse.मेलनम्_pos - 1
            hse.outgoing_द्वयङ्काः_गणनम् = hse.गवाक्षः_आकार
            प्रत्यागम HSE_state.HSES_YIELD_BR_सूचकांक
    अन्यथा:
        प्रत्यागम HSE_state.HSES_YIELD_चिह्नकम्_BIT


परिभाषा st_yield_literal(hse, oi):
    यदि can_take_byte(oi):
        प्रक्षिप_literal_byte(hse, oi)
        प्रत्यागम HSE_state.HSES_अन्वेषणम्
    अन्यथा:
        प्रत्यागम HSE_state.HSES_YIELD_LITERAL


परिभाषा st_yield_br_सूचकांक(hse, oi):
    यदि can_take_byte(oi):
        यदि प्रक्षिप_outgoing_द्वयङ्काः(hse, oi) > 0:
            प्रत्यागम HSE_state.HSES_YIELD_BR_सूचकांक
        अन्यथा:
            hse.outgoing_द्वयङ्काः = hse.मेलनम्_दीर्घता - 1
            hse.outgoing_द्वयङ्काः_गणनम् = hse.lookahead_आकार
            प्रत्यागम HSE_state.HSES_YIELD_BR_दीर्घता
    अन्यथा:
        प्रत्यागम HSE_state.HSES_YIELD_BR_सूचकांक


परिभाषा st_yield_br_दीर्घता(hse, oi):
    यदि can_take_byte(oi):
        यदि प्रक्षिप_outgoing_द्वयङ्काः(hse, oi) > 0:
            प्रत्यागम HSE_state.HSES_YIELD_BR_दीर्घता
        अन्यथा:
            hse.मेलनम्_अवलोकनम्_सूचकांक += hse.मेलनम्_दीर्घता
            hse.मेलनम्_दीर्घता = 0
            प्रत्यागम HSE_state.HSES_अन्वेषणम्
    अन्यथा:
        प्रत्यागम HSE_state.HSES_YIELD_BR_दीर्घता


परिभाषा st_save_backlog(hse):
    save_backlog(hse)
    प्रत्यागम HSE_state.HSES_NOT_पूर्णम्


परिभाषा st_flush_bit_बफर(hse, oi):
    यदि hse.bit_सूचकांक == 0x80:
        प्रत्यागम HSE_state.HSES_समाप्तम्
    नोचेद्यदि can_take_byte(oi):
        oi.buf[oi.output_आकार[0]] = hse.current_byte
        oi.output_आकार[0] += 1
        प्रत्यागम HSE_state.HSES_समाप्तम्
    अन्यथा:
        प्रत्यागम HSE_state.HSES_FLUSH_द्वयङ्काः


परिभाषा save_backlog(hse):
    निवेशः_buf_sz = get_निवेशः_बफर_आकार(hse)

    msi = hse.मेलनम्_अवलोकनम्_सूचकांक

    rem = निवेशः_buf_sz - msi  # unprocessed bytes
    सरणम्_sz = निवेशः_buf_sz + rem

    प्रति i अन्तः परिसरः(सरणम्_sz):
        hse.बफर_2[i] = hse.बफर_2[निवेशः_buf_sz - rem + i]

    hse.मेलनम्_अवलोकनम्_सूचकांक = 0
    hse.निवेशः_आकार -= निवेशः_buf_sz - rem


परिभाषा encode(दत्त, गवाक्षः_आकार, lookahead_आकार):
    hse = heatshrink_encoder(गवाक्षः_आकार, lookahead_आकार)
    निवेशः_buf = (ctypes.c_uint8 * दीर्घता(दत्त))()
    प्रति i, d अन्तः enumerate(दत्त):
        निवेशः_buf[i] = d
    अन्तर्_आकार = दीर्घता(निवेशः_buf)

    बहिर्गमनम्_buf_आकार = 4 * अन्तर्_आकार  # set output buffer size a bit larger
    बहिर्गमनम्_buf = (ctypes.c_uint8 * बहिर्गमनम्_buf_आकार)()

    sunk = 0
    ret = []
    यावत् sunk < अन्तर्_आकार:
        निवेशः_आकार = ctypes.c_आकार_t(अन्तर्_आकार)
        heatshrink_encoder_sink(hse, निवेशः_buf, अन्तर्_आकार - sunk, निवेशः_आकार)
        निवेशः_buf = निवेशः_buf[निवेशः_आकार.मानम्:]
        sunk += निवेशः_आकार.मानम्
        यदि sunk == अन्तर्_आकार:
            heatshrink_encoder_finish(hse)

        poll_res = HSE_poll_res.HSER_POLL_अधिकम्
        यावत् poll_res == HSE_poll_res.HSER_POLL_अधिकम्:
            output_आकार = (ctypes.c_आकार_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, बहिर्गमनम्_buf, बहिर्गमनम्_buf_आकार, output_आकार)
            ret += list(बहिर्गमनम्_buf)[0:output_आकार[0]]

        यदि sunk == अन्तर्_आकार:
            heatshrink_encoder_finish(hse)
    प्रत्यागम ret
