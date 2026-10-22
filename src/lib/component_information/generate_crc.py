#!/usr/bin/env python

ಆನಯನ argparse
ಆನಯನ os
ಆನಯನ hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    ಪ್ರತಿ i ಅನ್ತಃ range(256):
        k = i
        ಪ್ರತಿ j ಅನ್ತಃ range(8):
            ಯದಿ k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    ಪ್ರತ್ಯಾಗಮ a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    ಪ್ರತಿ k ಅನ್ತಃ buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    ಪ್ರತ್ಯಾಗಮ crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) ಇವ f:
        ಪ್ರತಿ n ಅನ್ತಃ iter(ಅನಾಮನ್ : f.readinto(mv), 0):
            h.update(mv[:n])
    ಪ್ರತ್ಯಾಗಮ h.hexdigest()


with open(filename, 'w') ಇವ outfile:
    outfile.ಲಿಖ("#pragma once\n")
    outfile.ಲಿಖ("#include <stdint.h>\n")
    outfile.ಲಿಖ("namespace component_information {\n")
    ಪ್ರತಿ filename ಅನ್ತಃ files:
        file_crc = 0
        ಪ್ರತಿ ಸಾಲು ಅನ್ತಃ open(filename, "rb"):
            file_crc = crc_update(ಸಾಲು, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.ಲಿಖ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.ಲಿಖ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.ಲಿಖ("}\n")
