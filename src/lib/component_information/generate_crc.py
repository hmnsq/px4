#!/usr/bin/env python

استورد argparse
استورد os
استورد hashlib

parser = argparse.دلیل_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.جوڑو_دلیل('--output', metavar='checksums.h', help='output file')
parser.جوڑو_دلیل('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

دلیلاں = parser.parse_دلیلاں()
filename = دلیلاں.output
files = دلیلاں.files

تعریف create_جدول():
    a = []
    لكل i في سلسلہ(256):
        k = i
        لكل j في سلسلہ(8):
            إذا k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    أرجع a

# MAVLink's CRC32
تعریف چکری_جانچ_update(buf, چکری_جانچ_جدول, چکری_جانچ):
    لكل k في buf:
        چکری_جانچ = (چکری_جانچ >> 8) ^ چکری_جانچ_جدول[(چکری_جانچ & 0xff) ^ k]
    أرجع چکری_جانچ

چکری_جانچ_جدول = create_جدول()

تعریف sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    نال open(filename, 'rb', buffering=0) كـ f:
        لكل n في iter(مجهول : f.readinto(mv), 0):
            h.update(mv[:n])
    أرجع h.hexdigest()


نال open(filename, 'w') كـ outfile:
    outfile.اكتب("#pragma once\n")
    outfile.اكتب("#include <stdint.h>\n")
    outfile.اكتب("namespace component_information {\n")
    لكل filename في files:
        file_چکری_جانچ = 0
        لكل سطر في open(filename, "rb"):
            file_چکری_جانچ = چکری_جانچ_update(سطر, چکری_جانچ_جدول, file_چکری_جانچ)
        file_sha256 = sha256sum(filename)

        basename = os.رستہ.basename(filename)
        identifier = basename.ونڈو('.')[0]
        outfile.اكتب("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_چکری_جانچ))
        outfile.اكتب("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.اكتب("}\n")
