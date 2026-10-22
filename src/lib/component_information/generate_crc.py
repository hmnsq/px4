#!/usr/bin/env python

దిగుమతి argparse
దిగుమతి os
దిగుమతి hashlib

parser = argparse.వాదం_Parser(description="""Generate the COMPONENT_INFORMATION checksums (CRC32) header file""")
parser.కూడు_వాదం('--output', metavar='checksums.h', help='output file')
parser.కూడు_వాదం('files', nargs='+', metavar='input_file.json', help='files to generate CRC from')

వాదనలు = parser.parse_వాదనలు()
filename = వాదనలు.output
files = వాదనలు.files

నిర్వచనం create_పట్టిక():
    a = []
    కొరకు i లో శ్రేణి(256):
        k = i
        కొరకు j లో శ్రేణి(8):
            ఒకవేళ k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    తిరిగి a

# MAVLink's CRC32
నిర్వచనం చక్రీయ_తనిఖీ_update(buf, చక్రీయ_తనిఖీ_పట్టిక, చక్రీయ_తనిఖీ):
    కొరకు k లో buf:
        చక్రీయ_తనిఖీ = (చక్రీయ_తనిఖీ >> 8) ^ చక్రీయ_తనిఖీ_పట్టిక[(చక్రీయ_తనిఖీ & 0xff) ^ k]
    తిరిగి చక్రీయ_తనిఖీ

చక్రీయ_తనిఖీ_పట్టిక = create_పట్టిక()

నిర్వచనం sha256sum(filename):
    h  = hashlib.sha256()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    తోడుగా open(filename, 'rb', buffering=0) ఇవ f:
        కొరకు n లో iter(అజ్ఞాత : f.readinto(mv), 0):
            h.update(mv[:n])
    తిరిగి h.hexdigest()


తోడుగా open(filename, 'w') ఇవ outfile:
    outfile.లిఖ("#pragma once\n")
    outfile.లిఖ("#include <stdint.h>\n")
    outfile.లిఖ("namespace component_information {\n")
    కొరకు filename లో files:
        file_చక్రీయ_తనిఖీ = 0
        కొరకు పంక్తి లో open(filename, "rb"):
            file_చక్రీయ_తనిఖీ = చక్రీయ_తనిఖీ_update(పంక్తి, చక్రీయ_తనిఖీ_పట్టిక, file_చక్రీయ_తనిఖీ)
        file_sha256 = sha256sum(filename)

        basename = os.మార్గం.basename(filename)
        identifier = basename.విభజించు('.')[0]
        outfile.లిఖ("static constexpr uint32_t {:}_crc = {:};\n".format(identifier, file_చక్రీయ_తనిఖీ))
        outfile.లిఖ("static constexpr const char *{:}_sha256 = \"{:}\";\n".format(identifier, file_sha256))


    outfile.లిఖ("}\n")
