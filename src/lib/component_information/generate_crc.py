#!/usr/bin/env python

ਆਯਾਤ argparse
ਆਯਾਤ os
ਆਯਾਤ hashlib

parser = argparse.ਦਲੀਲ_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.ਜੋੜੋ_ਦਲੀਲ('--output', metavar='checksums.h', help='output file')
parser.ਜੋੜੋ_ਦਲੀਲ('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

ਦਲੀਲਾਂ = parser.parse_ਦਲੀਲਾਂ()
filename = ਦਲੀਲਾਂ.output
files = ਦਲੀਲਾਂ.files

ਪਰਿਭਾਸ਼ਾ create_ਸਾਰਣੀ():
    a = []
    ਲਈ i ਵਿੱਚ ਸੀਮਾ(256):
        k = i
        ਲਈ j ਵਿੱਚ ਸੀਮਾ(8):
            ਜੇ k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    ਵਾਪਸ a

# MAVLink's CRC32
ਪਰਿਭਾਸ਼ਾ ਚੱਕਰੀ_ਜਾਂਚ_update(buf, ਚੱਕਰੀ_ਜਾਂਚ_ਸਾਰਣੀ, ਚੱਕਰੀ_ਜਾਂਚ):
    ਲਈ k ਵਿੱਚ buf:
        ਚੱਕਰੀ_ਜਾਂਚ = (ਚੱਕਰੀ_ਜਾਂਚ >> 8) ^ ਚੱਕਰੀ_ਜਾਂਚ_ਸਾਰਣੀ[(ਚੱਕਰੀ_ਜਾਂਚ & 0xff) ^ k]
    ਵਾਪਸ ਚੱਕਰੀ_ਜਾਂਚ

ਚੱਕਰੀ_ਜਾਂਚ_ਸਾਰਣੀ = create_ਸਾਰਣੀ()

ਪਰਿਭਾਸ਼ਾ sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    ਨਾਲ open(filename, 'rb', buffering=0) ਇਵ f:
        ਲਈ n ਵਿੱਚ iter(ਅਨਾਮ : f.readinto(mv), 0):
            h.update(mv[:n])
    ਵਾਪਸ h.hexdigest()


ਨਾਲ open(filename, 'w') ਇਵ outfile:
    outfile.ਲਿਖ("#pragma once\n")
    outfile.ਲਿਖ("#include <stdint.h>\n")
    outfile.ਲਿਖ("namespace component_information {\n")
    ਲਈ filename ਵਿੱਚ files:
        file_ਚੱਕਰੀ_ਜਾਂਚ = 0
        ਲਈ ਸਤਰ ਵਿੱਚ open(filename, "rb"):
            file_ਚੱਕਰੀ_ਜਾਂਚ = ਚੱਕਰੀ_ਜਾਂਚ_update(ਸਤਰ, ਚੱਕਰੀ_ਜਾਂਚ_ਸਾਰਣੀ, file_ਚੱਕਰੀ_ਜਾਂਚ)
        file_sha256 = sha256sum(filename)

        basename = os.ਰਸਤਾ.basename(filename)
        identifier = basename.ਵੰਡੋ('.')[0]
        outfile.ਲਿਖ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_ਚੱਕਰੀ_ਜਾਂਚ))
        outfile.ਲਿਖ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.ਲਿਖ("}\n")
