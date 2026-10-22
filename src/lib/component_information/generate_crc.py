#!/usr/bin/env python

ଆମଦାନୀ argparse
ଆମଦାନୀ os
ଆମଦାନୀ hashlib

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.add_argument('--output', metavar='checksums.h', help='output file')
parser.add_argument('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

args = parser.parse_args()
filename = args.output
files = args.files

def create_table():
    a = []
    ପାଇଁ i ଭିତରେ range(256):
        k = i
        ପାଇଁ j ଭିତରେ range(8):
            ଯଦି k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    ଫେରନ୍ତୁ a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    ପାଇଁ k ଭିତରେ buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    ଫେରନ୍ତୁ crc

crc_table = create_table()

def sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) ଇଵ f:
        ପାଇଁ n ଭିତରେ iter(ଅନାମ : f.readinto(mv), 0):
            h.update(mv[:n])
    ଫେରନ୍ତୁ h.hexdigest()


with open(filename, 'w') ଇଵ outfile:
    outfile.ଲିଖ("#pragma once\n")
    outfile.ଲିଖ("#include <stdint.h>\n")
    outfile.ଲିଖ("namespace component_information {\n")
    ପାଇଁ filename ଭିତରେ files:
        file_crc = 0
        ପାଇଁ ଧାଡ଼ି ଭିତରେ open(filename, "rb"):
            file_crc = crc_update(ଧାଡ଼ି, crc_table, file_crc)
        file_sha256 = sha256sum(filename)

        basename = os.path.basename(filename)
        identifier = basename.split('.')[0]
        outfile.ଲିଖ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_crc))
        outfile.ଲିଖ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.ଲିଖ("}\n")
