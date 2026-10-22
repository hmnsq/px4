#!/usr/bin/env python

درآمد argparse
درآمد os
درآمد hashlib

parser = argparse.دلیل_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.جوڑو_دلیل('--output', metavar='checksums.h', help='output file')
parser.جوڑو_دلیل('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

دلائل = parser.parse_دلائل()
filename = دلائل.output
files = دلائل.files

تعریف create_جدول():
    a = []
    برائے i میں سلسلہ(256):
        k = i
        برائے j میں سلسلہ(8):
            اگر k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    واپس a

# MAVLink's CRC32
تعریف چکری_جانچ_update(buf, چکری_جانچ_جدول, چکری_جانچ):
    برائے k میں buf:
        چکری_جانچ = (چکری_جانچ >> 8) ^ چکری_جانچ_جدول[(چکری_جانچ & 0xff) ^ k]
    واپس چکری_جانچ

چکری_جانچ_جدول = create_جدول()

تعریف sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    ساتھ open(filename, 'rb', buffering=0) بطور f:
        برائے n میں iter(بےنام : f.readinto(mv), 0):
            h.update(mv[:n])
    واپس h.hexdigest()


ساتھ open(filename, 'w') بطور outfile:
    outfile.اكتب("#pragma once\n")
    outfile.اكتب("#include <stdint.h>\n")
    outfile.اكتب("namespace component_information {\n")
    برائے filename میں files:
        file_چکری_جانچ = 0
        برائے سطر میں open(filename, "rb"):
            file_چکری_جانچ = چکری_جانچ_update(سطر, چکری_جانچ_جدول, file_چکری_جانچ)
        file_sha256 = sha256sum(filename)

        basename = os.راستہ.basename(filename)
        identifier = basename.بانٹو('.')[0]
        outfile.اكتب("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_چکری_جانچ))
        outfile.اكتب("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.اكتب("}\n")
