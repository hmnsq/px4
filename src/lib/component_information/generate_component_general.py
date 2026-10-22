#!/usr/bin/env python

దిగుమతి argparse
దిగుమతి json
దిగుమతి lzma #to create .xz file
దిగుమతి re

parser = argparse.వాదం_Parser(description="""Generate the COMPONENT_GENERAL json file""")
parser.కూడు_వాదం('filename', metavar='component_general.json', help='JSON output file')
parser.కూడు_వాదం('--compress', action='store_true', help='Add a compressed output file')
parser.కూడు_వాదం('--type', metavar='type', action="append", అప్రమేయ=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.కూడు_వాదం('--version-file', metavar='build_git_version.h', help='Git version file')

వాదనలు = parser.parse_వాదనలు()
filename = వాదనలు.filename
compress = వాదనలు.compress
సంస్కరణ_file = వాదనలు.సంస్కరణ_file

సంస్కరణ_dir = ''
ఒకవేళ సంస్కరణ_file అయిన కాదు ఏమీలేదు:
    కొరకు పంక్తి లో open(సంస్కరణ_file, "r"):
        సంస్కరణ_శోధన = re.శోధన(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', పంక్తి)
        ఒకవేళ సంస్కరణ_శోధన:
            సంస్కరణ_dir = సంస్కరణ_శోధన.సమూహం(1)
            ఆపు


నిర్వచనం save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    తోడుగా lzma.open(xz_filename, 'wt', preset=9) ఇవ f:
        తోడుగా open(filename, 'r') ఇవ content_file:
            f.లిఖ(content_file.పఠ())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
కొరకు metadata_type_tuple లో వాదనలు.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.విభజించు(',')
    file_చక్రీయ_తనిఖీ = 0
    కొరకు పంక్తి లో open(metadata_file, "rb"):
        file_చక్రీయ_తనిఖీ = చక్రీయ_తనిఖీ_update(పంక్తి, చక్రీయ_తనిఖీ_పట్టిక, file_చక్రీయ_తనిఖీ)
    json_type = {
            'type': పూర్ణాంకం(type_id),
            'uri': uri.replace('{version}', సంస్కరణ_dir),
            'fileCrc': file_చక్రీయ_తనిఖీ,
            }
    ఒకవేళ పొడవు(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', సంస్కరణ_dir)
    ఒకవేళ పొడవు(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

తోడుగా open(filename, 'w') ఇవ outfile:
    json.dump(component_general, outfile, క్రమీకరించు_కీలకాలు=నిజం)

ఒకవేళ compress:
    save_compressed(filename)
