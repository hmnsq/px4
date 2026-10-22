#!/usr/bin/env python

ഇറക്കുമതി argparse
ഇറക്കുമതി os
ഇറക്കുമതി hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    വേണ്ടി i ഇൽ range(256):
        k = i
        വേണ്ടി j ഇൽ range(8):
            എങ്കിൽ k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    തിരികെ a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    വേണ്ടി k ഇൽ buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    തിരികെ crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) ഇവ f:
        വേണ്ടി n ഇൽ iter(അജ്ഞാത : f.readinto(mv), 0):
            h.update(mv[:n])
    തിരികെ h.hexdigest()


with open(filename, 'w') ഇവ outfile:
    outfile.ലിഖ("#pragma once\n")
    outfile.ലിഖ("#include <stdint.h>\n")
    outfile.ലിഖ("namespace component_information {\n")
    വേണ്ടി filename ഇൽ files:
        file_crc = 0
        വേണ്ടി വരി ഇൽ open(filename, "rb"):
            file_crc = crc_update(വരി, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.ലിഖ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.ലിഖ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.ലിഖ("}\n")
