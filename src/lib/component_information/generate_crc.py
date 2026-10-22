#!/usr/bin/env python

आयात argparse
आयात os
आयात hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    साठी i मध्ये range(256):
        k = i
        साठी j मध्ये range(8):
            जर k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    परत a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    साठी k मध्ये buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    परत crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) इव f:
        साठी n मध्ये iter(निनावी : f.readinto(mv), 0):
            h.update(mv[:n])
    परत h.hexdigest()


with open(filename, 'w') इव outfile:
    outfile.लिख("#pragma once\n")
    outfile.लिख("#include <stdint.h>\n")
    outfile.लिख("namespace component_information {\n")
    साठी filename मध्ये files:
        file_crc = 0
        साठी ओळ मध्ये open(filename, "rb"):
            file_crc = crc_update(ओळ, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.लिख("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.लिख("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.लिख("}\n")
