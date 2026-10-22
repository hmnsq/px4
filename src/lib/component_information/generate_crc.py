#!/usr/bin/env python

ಆಮದು argparse
ಆಮದು os
ಆಮದು hashlib

parser = argparse.ವಾದ_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.ಕೂಡಿಸು_ವಾದ('--output', metavar='checksums.h', help='output file')
parser.ಕೂಡಿಸು_ವಾದ('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

ವಾದಗಳು = parser.parse_ವಾದಗಳು()
filename = ವಾದಗಳು.output
files = ವಾದಗಳು.files

ವ್ಯಾಖ್ಯಾನ create_ಕೋಷ್ಟಕ():
    a = []
    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(256):
        k = i
        ಗಾಗಿ j ಒಳಗೆ ಶ್ರೇಣಿ(8):
            ಆದರೆ k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    ಹಿಂತಿರುಗಿ a

# MAVLink's CRC32
ವ್ಯಾಖ್ಯಾನ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_update(buf, ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಕೋಷ್ಟಕ, ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ):
    ಗಾಗಿ k ಒಳಗೆ buf:
        ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 8) ^ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಕೋಷ್ಟಕ[(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & 0xff) ^ k]
    ಹಿಂತಿರುಗಿ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ

ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಕೋಷ್ಟಕ = create_ಕೋಷ್ಟಕ()

ವ್ಯಾಖ್ಯಾನ sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    ಜೊತೆ open(filename, 'rb', buffering=0) ಇವ f:
        ಗಾಗಿ n ಒಳಗೆ iter(ಅನಾಮಿಕ : f.readinto(mv), 0):
            h.update(mv[:n])
    ಹಿಂತಿರುಗಿ h.hexdigest()


ಜೊತೆ open(filename, 'w') ಇವ outfile:
    outfile.ಲಿಖ("#pragma once\n")
    outfile.ಲಿಖ("#include <stdint.h>\n")
    outfile.ಲಿಖ("namespace component_information {\n")
    ಗಾಗಿ filename ಒಳಗೆ files:
        file_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = 0
        ಗಾಗಿ ಸಾಲು ಒಳಗೆ open(filename, "rb"):
            file_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_update(ಸಾಲು, ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಕೋಷ್ಟಕ, file_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ)
        file_sha256 = sha256sum(filename)

        basename = os.ಮಾರ್ಗ.basename(filename)
        identifier = basename.ವಿಭಜಿಸು('.')[0]
        outfile.ಲಿಖ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ))
        outfile.ಲಿಖ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.ಲಿಖ("}\n")
