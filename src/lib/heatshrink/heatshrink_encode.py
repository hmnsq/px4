దిగుమతి ctypes
తః enum దిగుమతి Enum


# Note: this implementation directly follows the heatshrink_encoder.c code
# (it's neither expected to be very efficient, nor is it pythonic)

# Enum
తరగతి HSE_state(ctypes.c_int):
    HSES_NOT_నిండిన = 0
    HSES_FILLED = 1
    HSES_శోధన = 2
    HSES_YIELD_ట్యాగ్_బిట్టు = 3
    HSES_YIELD_LITERAL = 4
    HSES_YIELD_BR_సూచీ = 5
    HSES_YIELD_BR_నిడివి = 6
    HSES_SAVE_BACKLOG = 7
    HSES_FLUSH_బిట్లు = 8
    HSES_పూర్తి = 9


# Constants
FLAG_IS_FINISHING = 0x01
సరిపోలిక_NOT_దొరికింది = 0xFFFF

HEATSHRINK_LITERAL_MARKER = 0x01
HEATSHRINK_BACKREF_MARKER = 0x00


# Structs
తరగతి output_info(ctypes.Structure):
    _fields_ = [
        ("buf", ctypes.సూచిక(ctypes.c_uint8)),
        ("buf_size", ctypes.c_కొలత_t),
        ("output_size", ctypes.సూచిక(ctypes.c_కొలత_t))
    ]


# Functions
నిర్వచనం కూడు_ట్యాగ్_బిట్టు(hse, oi, ట్యాగ్):
    నెట్టు_బిట్లు(hse, 1, ట్యాగ్, oi)


నిర్వచనం నెట్టు_బిట్లు(hse, లెక్క, బిట్లు, oi):
    ప్రతిజ్ఞా లెక్క <= 8
    current_byte = ctypes.c_uint8(hse.current_byte)
    బిట్టు_సూచీ = ctypes.c_uint8(hse.బిట్టు_సూచీ)
    కొరకు i లో శ్రేణి(లెక్క - 1, -1, -1):
        బిట్టు = బిట్లు & (1 << i)
        ఒకవేళ బిట్టు:
            current_byte.విలువ |= బిట్టు_సూచీ.విలువ
        బిట్టు_సూచీ.విలువ >>= 1
        ఒకవేళ బిట్టు_సూచీ.విలువ == 0:
            బిట్టు_సూచీ.విలువ = 0x80
            oi.buf[oi.output_కొలత[0]] = current_byte.విలువ
            oi.output_కొలత[0] += 1
            current_byte.విలువ = 0
    hse.current_byte = current_byte.విలువ
    hse.బిట్టు_సూచీ = బిట్టు_సూచీ.విలువ


నిర్వచనం నెట్టు_literal_byte(hse, oi):
    processed_స్థానభ్రంశం = hse.సరిపోలిక_పరిశీలన_సూచీ - 1
    ఆదానం_స్థానభ్రంశం = get_ఆదానం_స్థానభ్రంశం(hse) + processed_స్థానభ్రంశం
    c = hse.బఫరు_2[ఆదానం_స్థానభ్రంశం]
    నెట్టు_బిట్లు(hse, 8, c, oi)


# Define necessary structures and enums
తరగతి heatshrink_encoder(ctypes.Structure):
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

        ("buffer", ctypes.సూచిక(ctypes.c_uint8)),
    ]

    నిర్వచనం __init__(self, కిటికీ_కొలత=8, lookahead_కొలత=4):
        పర().__init__()
        self.కిటికీ_కొలత = కిటికీ_కొలత
        self.lookahead_కొలత = lookahead_కొలత
        self.శోధన_సూచీ = hs_సూచీ(కిటికీ_కొలత)
        self.బఫరు_2 = (ctypes.c_uint8 * (2 << కిటికీ_కొలత))()
        self.బిట్టు_సూచీ = 0x80


తరగతి hs_సూచీ(ctypes.Structure):
    _fields_ = [("index", ctypes.సూచిక(ctypes.c_int16))]

    నిర్వచనం __init__(self, కిటికీ_కొలత):
        పర().__init__()
        self.సూచీ = (ctypes.c_int16 * (2 << కిటికీ_కొలత))()


తరగతి HSE_sink_res(Enum):
    HSER_SINK_OK = 0
    HSER_SINK_ERROR_NULL = -1
    HSER_SINK_ERROR_MISUSE = -2


తరగతి HSE_poll_res(Enum):
    HSER_POLL_ఖాళీలు = 0
    HSER_POLL_మరిన్ని = 1
    HSER_POLL_ERROR_NULL = -1
    HSER_POLL_ERROR_MISUSE = -2


తరగతి HSE_ముగింపు_res(Enum):
    HSER_ముగింపు_పూర్తి = 0
    HSER_ముగింపు_మరిన్ని = 1
    HSER_ముగింపు_ERROR_NULL = -1


నిర్వచనం is_finishing(hse):
    తిరిగి hse.flags & FLAG_IS_FINISHING


నిర్వచనం can_take_byte(oi):
    తిరిగి oi.output_కొలత[0] < oi.buf_కొలత


నిర్వచనం get_ఆదానం_బఫరు_కొలత(hse):
    తిరిగి 1 << hse.కిటికీ_కొలత


నిర్వచనం get_lookahead_కొలత(hse):
    తిరిగి 1 << hse.lookahead_కొలత


నిర్వచనం get_ఆదానం_స్థానభ్రంశం(hse):
    తిరిగి get_ఆదానం_బఫరు_కొలత(hse)


నిర్వచనం heatshrink_encoder_sink(hse, లోపల_buf, కొలత, ఆదానం_కొలత):
    ఒకవేళ hse అయిన ఏమీలేదు లేదా లోపల_buf అయిన ఏమీలేదు లేదా ఆదానం_కొలత అయిన ఏమీలేదు:
        తిరిగి HSE_sink_res.HSER_SINK_ERROR_NULL

    ఒకవేళ is_finishing(hse):
        తిరిగి HSE_sink_res.HSER_SINK_ERROR_MISUSE

    ఒకవేళ hse.state != HSE_state.HSES_NOT_నిండిన:
        తిరిగి HSE_sink_res.HSER_SINK_ERROR_MISUSE

    write_స్థానభ్రంశం_2 = get_ఆదానం_స్థానభ్రంశం(hse) + hse.ఆదానం_కొలత
    ibs = get_ఆదానం_బఫరు_కొలత(hse)
    rem = ibs - hse.ఆదానం_కొలత
    cp_sz = min(rem, కొలత)

    కొరకు i లో శ్రేణి(cp_sz):
        hse.బఫరు_2[write_స్థానభ్రంశం_2 + i] = లోపల_buf[i]
    ఆదానం_కొలత.విలువ = cp_sz
    hse.ఆదానం_కొలత += cp_sz

    ఒకవేళ cp_sz == rem:
        hse.state = HSE_state.HSES_FILLED

    తిరిగి HSE_sink_res.HSER_SINK_OK


నిర్వచనం do_indexing(hse):
    # Build an index array I that contains flattened linked lists
    # for the previous instances of every byte in the buffer.

    hsi = hse.శోధన_సూచీ
    చివరి = [0xffff] * 256
    buf = hse.బఫరు_2
    సూచీ = hsi.సూచీ
    ఆదానం_స్థానభ్రంశం = get_ఆదానం_స్థానభ్రంశం(hse)
    చివర = ఆదానం_స్థానభ్రంశం + hse.ఆదానం_కొలత

    కొరకు i లో శ్రేణి(0, చివర):
        v = buf[i]
        lv = చివరి[v]
        సూచీ[i] = lv
        చివరి[v] = i


నిర్వచనం heatshrink_encoder_poll(hse, బహిర్గతం_buf, బహిర్గతం_buf_కొలత, output_కొలత):
    ఒకవేళ hse అయిన ఏమీలేదు లేదా బహిర్గతం_buf అయిన ఏమీలేదు లేదా output_కొలత అయిన ఏమీలేదు:
        తిరిగి HSE_poll_res.HSER_POLL_ERROR_NULL

    ఒకవేళ బహిర్గతం_buf_కొలత == 0:
        తిరిగి HSE_poll_res.HSER_POLL_ERROR_MISUSE

    output_కొలత[0] = 0
    oi = output_info()
    oi.buf = బహిర్గతం_buf
    oi.buf_కొలత = బహిర్గతం_buf_కొలత
    oi.output_కొలత = output_కొలత

    అయితే నిజం:
        లోపల_state = hse.state
        ఒకవేళ లోపల_state == HSE_state.HSES_NOT_నిండిన:
            తిరిగి HSE_poll_res.HSER_POLL_ఖాళీలు
        లేదావేళ లోపల_state == HSE_state.HSES_పూర్తి:
            తిరిగి HSE_poll_res.HSER_POLL_ఖాళీలు
        లేదావేళ లోపల_state == HSE_state.HSES_FILLED:
            do_indexing(hse)
            hse.state = HSE_state.HSES_శోధన
        లేదావేళ లోపల_state == HSE_state.HSES_శోధన:
            hse.state = st_అడుగు_శోధన(hse)
        లేదావేళ లోపల_state == HSE_state.HSES_YIELD_ట్యాగ్_బిట్టు:
            hse.state = st_yield_ట్యాగ్_బిట్టు(hse, oi)
        లేదావేళ లోపల_state == HSE_state.HSES_YIELD_LITERAL:
            hse.state = st_yield_literal(hse, oi)
        లేదావేళ లోపల_state == HSE_state.HSES_YIELD_BR_సూచీ:
            hse.state = st_yield_br_సూచీ(hse, oi)
        లేదావేళ లోపల_state == HSE_state.HSES_YIELD_BR_నిడివి:
            hse.state = st_yield_br_నిడివి(hse, oi)
        లేదావేళ లోపల_state == HSE_state.HSES_SAVE_BACKLOG:
            hse.state = st_save_backlog(hse)
        లేదావేళ లోపల_state == HSE_state.HSES_FLUSH_బిట్లు:
            hse.state = st_flush_బిట్టు_బఫరు(hse, oi)
        లేకపోతే:
            తిరిగి HSE_poll_res.HSER_POLL_ERROR_MISUSE

        ఒకవేళ hse.state == లోపల_state:
            ఒకవేళ oi.output_కొలత == oi.buf_కొలత:
                తిరిగి HSE_poll_res.HSER_POLL_మరిన్ని


నిర్వచనం heatshrink_encoder_ముగింపు(hse):
    hse.flags |= FLAG_IS_FINISHING
    ఒకవేళ hse.state == HSE_state.HSES_NOT_నిండిన:
        hse.state = HSE_state.HSES_FILLED
    ఒకవేళ hse.state == HSE_state.HSES_పూర్తి:
        తిరిగి HSE_ముగింపు_res.HSER_ముగింపు_పూర్తి
    తిరిగి HSE_ముగింపు_res.HSER_ముగింపు_మరిన్ని


నిర్వచనం st_అడుగు_శోధన(hse):
    కిటికీ_నిడివి = get_ఆదానం_బఫరు_కొలత(hse)
    lookahead_sz = get_lookahead_కొలత(hse)
    msi = hse.సరిపోలిక_పరిశీలన_సూచీ

    fin = is_finishing(hse)
    ఒకవేళ msi > hse.ఆదానం_కొలత - (1 ఒకవేళ fin లేకపోతే lookahead_sz):
        తిరిగి HSE_state.HSES_FLUSH_బిట్లు ఒకవేళ fin లేకపోతే HSE_state.HSES_SAVE_BACKLOG

    ఆదానం_స్థానభ్రంశం = get_ఆదానం_స్థానభ్రంశం(hse)
    చివర = ఆదానం_స్థానభ్రంశం + msi
    ప్రారంభం = చివర - కిటికీ_నిడివి

    max_possible = lookahead_sz ఒకవేళ hse.ఆదానం_కొలత - msi >= lookahead_sz లేకపోతే hse.ఆదానం_కొలత - msi

    సరిపోలిక_pos, సరిపోలిక_నిడివి = కనుగొను_longest_సరిపోలిక(hse, ప్రారంభం, చివర, max_possible)

    ఒకవేళ సరిపోలిక_pos == సరిపోలిక_NOT_దొరికింది:
        hse.సరిపోలిక_పరిశీలన_సూచీ += 1
        hse.సరిపోలిక_నిడివి = 0
        తిరిగి HSE_state.HSES_YIELD_ట్యాగ్_బిట్టు
    లేకపోతే:
        hse.సరిపోలిక_pos = సరిపోలిక_pos
        hse.సరిపోలిక_నిడివి = సరిపోలిక_నిడివి
        తిరిగి HSE_state.HSES_YIELD_ట్యాగ్_బిట్టు


నిర్వచనం కనుగొను_longest_సరిపోలిక(hse, ప్రారంభం, చివర, maxlen):
    buf = hse.బఫరు_2

    సరిపోలిక_maxlen = 0
    సరిపోలిక_సూచీ = సరిపోలిక_NOT_దొరికింది

    needlepoint = చివర

    pos = hse.శోధన_సూచీ.సూచీ[చివర]
    buf_needlepoint_maxlen = buf[needlepoint + సరిపోలిక_maxlen]
    అయితే pos >= ప్రారంభం:

        ఒకవేళ buf[pos + సరిపోలిక_maxlen] != buf_needlepoint_maxlen:
            pos = hse.శోధన_సూచీ.సూచీ[pos]
            కొనసాగు

        నిడివి = 1
        కొరకు నిడివి లో శ్రేణి(1, maxlen):
            ఒకవేళ buf[pos + నిడివి] != buf[needlepoint + నిడివి]:
                ఆపు
        ఒకవేళ నిడివి > సరిపోలిక_maxlen:
            సరిపోలిక_maxlen = నిడివి
            సరిపోలిక_సూచీ = pos
            buf_needlepoint_maxlen = buf[needlepoint + సరిపోలిక_maxlen]
            ఒకవేళ నిడివి == maxlen:
                ఆపు  # won't find better

        pos = hse.శోధన_సూచీ.సూచీ[pos]

    break_even_బిందువు = 1 + hse.కిటికీ_కొలత + hse.lookahead_కొలత

    ఒకవేళ సరిపోలిక_maxlen > (break_even_బిందువు // 8):
        తిరిగి చివర - సరిపోలిక_సూచీ, సరిపోలిక_maxlen
    తిరిగి సరిపోలిక_NOT_దొరికింది, 0


నిర్వచనం నెట్టు_outgoing_బిట్లు(hse, oi):
    ఒకవేళ hse.outgoing_బిట్లు_లెక్క > 8:
        లెక్క = 8
        బిట్లు = hse.outgoing_బిట్లు >> (hse.outgoing_బిట్లు_లెక్క - 8)
    లేకపోతే:
        లెక్క = hse.outgoing_బిట్లు_లెక్క
        బిట్లు = hse.outgoing_బిట్లు

    ఒకవేళ లెక్క > 0:
        నెట్టు_బిట్లు(hse, లెక్క, బిట్లు, oi)
        hse.outgoing_బిట్లు_లెక్క -= లెక్క
    తిరిగి లెక్క


నిర్వచనం st_yield_ట్యాగ్_బిట్టు(hse, oi):
    ఒకవేళ can_take_byte(oi):
        ఒకవేళ hse.సరిపోలిక_నిడివి == 0:
            కూడు_ట్యాగ్_బిట్టు(hse, oi, HEATSHRINK_LITERAL_MARKER)
            తిరిగి HSE_state.HSES_YIELD_LITERAL
        లేకపోతే:
            కూడు_ట్యాగ్_బిట్టు(hse, oi, HEATSHRINK_BACKREF_MARKER)
            hse.outgoing_బిట్లు = hse.సరిపోలిక_pos - 1
            hse.outgoing_బిట్లు_లెక్క = hse.కిటికీ_కొలత
            తిరిగి HSE_state.HSES_YIELD_BR_సూచీ
    లేకపోతే:
        తిరిగి HSE_state.HSES_YIELD_ట్యాగ్_బిట్టు


నిర్వచనం st_yield_literal(hse, oi):
    ఒకవేళ can_take_byte(oi):
        నెట్టు_literal_byte(hse, oi)
        తిరిగి HSE_state.HSES_శోధన
    లేకపోతే:
        తిరిగి HSE_state.HSES_YIELD_LITERAL


నిర్వచనం st_yield_br_సూచీ(hse, oi):
    ఒకవేళ can_take_byte(oi):
        ఒకవేళ నెట్టు_outgoing_బిట్లు(hse, oi) > 0:
            తిరిగి HSE_state.HSES_YIELD_BR_సూచీ
        లేకపోతే:
            hse.outgoing_బిట్లు = hse.సరిపోలిక_నిడివి - 1
            hse.outgoing_బిట్లు_లెక్క = hse.lookahead_కొలత
            తిరిగి HSE_state.HSES_YIELD_BR_నిడివి
    లేకపోతే:
        తిరిగి HSE_state.HSES_YIELD_BR_సూచీ


నిర్వచనం st_yield_br_నిడివి(hse, oi):
    ఒకవేళ can_take_byte(oi):
        ఒకవేళ నెట్టు_outgoing_బిట్లు(hse, oi) > 0:
            తిరిగి HSE_state.HSES_YIELD_BR_నిడివి
        లేకపోతే:
            hse.సరిపోలిక_పరిశీలన_సూచీ += hse.సరిపోలిక_నిడివి
            hse.సరిపోలిక_నిడివి = 0
            తిరిగి HSE_state.HSES_శోధన
    లేకపోతే:
        తిరిగి HSE_state.HSES_YIELD_BR_నిడివి


నిర్వచనం st_save_backlog(hse):
    save_backlog(hse)
    తిరిగి HSE_state.HSES_NOT_నిండిన


నిర్వచనం st_flush_బిట్టు_బఫరు(hse, oi):
    ఒకవేళ hse.బిట్టు_సూచీ == 0x80:
        తిరిగి HSE_state.HSES_పూర్తి
    లేదావేళ can_take_byte(oi):
        oi.buf[oi.output_కొలత[0]] = hse.current_byte
        oi.output_కొలత[0] += 1
        తిరిగి HSE_state.HSES_పూర్తి
    లేకపోతే:
        తిరిగి HSE_state.HSES_FLUSH_బిట్లు


నిర్వచనం save_backlog(hse):
    ఆదానం_buf_sz = get_ఆదానం_బఫరు_కొలత(hse)

    msi = hse.సరిపోలిక_పరిశీలన_సూచీ

    rem = ఆదానం_buf_sz - msi  # unprocessed bytes
    జరుపు_sz = ఆదానం_buf_sz + rem

    కొరకు i లో శ్రేణి(జరుపు_sz):
        hse.బఫరు_2[i] = hse.బఫరు_2[ఆదానం_buf_sz - rem + i]

    hse.సరిపోలిక_పరిశీలన_సూచీ = 0
    hse.ఆదానం_కొలత -= ఆదానం_buf_sz - rem


నిర్వచనం encode(దత్తాంశం, కిటికీ_కొలత, lookahead_కొలత):
    hse = heatshrink_encoder(కిటికీ_కొలత, lookahead_కొలత)
    ఆదానం_buf = (ctypes.c_uint8 * పొడవు(దత్తాంశం))()
    కొరకు i, d లో enumerate(దత్తాంశం):
        ఆదానం_buf[i] = d
    లోపల_కొలత = పొడవు(ఆదానం_buf)

    బహిర్గతం_buf_కొలత = 4 * లోపల_కొలత  # set output buffer size a bit larger
    బహిర్గతం_buf = (ctypes.c_uint8 * బహిర్గతం_buf_కొలత)()

    sunk = 0
    ret = []
    అయితే sunk < లోపల_కొలత:
        ఆదానం_కొలత = ctypes.c_కొలత_t(లోపల_కొలత)
        heatshrink_encoder_sink(hse, ఆదానం_buf, లోపల_కొలత - sunk, ఆదానం_కొలత)
        ఆదానం_buf = ఆదానం_buf[ఆదానం_కొలత.విలువ:]
        sunk += ఆదానం_కొలత.విలువ
        ఒకవేళ sunk == లోపల_కొలత:
            heatshrink_encoder_ముగింపు(hse)

        poll_res = HSE_poll_res.HSER_POLL_మరిన్ని
        అయితే poll_res == HSE_poll_res.HSER_POLL_మరిన్ని:
            output_కొలత = (ctypes.c_కొలత_t * 1)()
            poll_res = heatshrink_encoder_poll(hse, బహిర్గతం_buf, బహిర్గతం_buf_కొలత, output_కొలత)
            ret += list(బహిర్గతం_buf)[0:output_కొలత[0]]

        ఒకవేళ sunk == లోపల_కొలత:
            heatshrink_encoder_ముగింపు(hse)
    తిరిగి ret
