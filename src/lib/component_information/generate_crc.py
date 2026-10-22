#!/usr/bin/env python

இறக்குமதி argparse
இறக்குமதி os
இறக்குமதி hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    க்கு i உள் range(256):
        k = i
        க்கு j உள் range(8):
            எனில் k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    திரும்பு a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    க்கு k உள் buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    திரும்பு crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) இவ f:
        க்கு n உள் iter(பெயரிலி : f.readinto(mv), 0):
            h.update(mv[:n])
    திரும்பு h.hexdigest()


with open(filename, 'w') இவ outfile:
    outfile.லிக("#pragma once\n")
    outfile.லிக("#include <stdint.h>\n")
    outfile.லிக("namespace component_information {\n")
    க்கு filename உள் files:
        file_crc = 0
        க்கு வரி உள் open(filename, "rb"):
            file_crc = crc_update(வரி, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.லிக("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.லிக("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.லிக("}\n")
