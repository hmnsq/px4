#!/usr/bin/env python

आनयन argparse
आनयन json
आनयन lzma #to create .xz file
आनयन re

parser = argparse.तर्कः_Parser(description="""Generate the COMPONENT_GENERAL json file""")
parser.सङ्कलय_तर्कः('filename', metavar='component_general.json', help='JSON output file')
parser.सङ्कलय_तर्कः('--compress', action='store_true', help='Add a compressed output file')
parser.सङ्कलय_तर्कः('--type', metavar='type', action="append", स्वभाव=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.सङ्कलय_तर्कः('--version-file', metavar='build_git_version.h', help='Git version file')

तर्काः = parser.parse_तर्काः()
filename = तर्काः.filename
compress = तर्काः.compress
संस्करणम्_file = तर्काः.संस्करणम्_file

संस्करणम्_dir = ''
यदि संस्करणम्_file अस्ति न किमपि_न:
    प्रति पङ्क्तिः अन्तः open(संस्करणम्_file, "r"):
        संस्करणम्_अन्वेषणम् = re.अन्वेषणम्(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', पङ्क्तिः)
        यदि संस्करणम्_अन्वेषणम्:
            संस्करणम्_dir = संस्करणम्_अन्वेषणम्.गणः(1)
            भङ्ग


परिभाषा save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    सह lzma.open(xz_filename, 'wt', preset=9) इव f:
        सह open(filename, 'r') इव content_file:
            f.लिख(content_file.पठ())

component_general = {}
component_general['version'] = 1

परिभाषा create_सारणी():
    a = []
    प्रति i अन्तः परिसरः(256):
        k = i
        प्रति j अन्तः परिसरः(8):
            यदि k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    प्रत्यागम a

# MAVLink's CRC32
परिभाषा crc_update(buf, crc_सारणी, crc):
    प्रति k अन्तः buf:
        crc = (crc >> 8) ^ crc_सारणी[(crc & 0xff) ^ k]
    प्रत्यागम crc

crc_सारणी = create_सारणी()

metadata_types = []
प्रति metadata_type_tuple अन्तः तर्काः.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.विभज(',')
    file_crc = 0
    प्रति पङ्क्तिः अन्तः open(metadata_file, "rb"):
        file_crc = crc_update(पङ्क्तिः, crc_सारणी, file_crc)
    json_type = {
            'type': पूर्णाङ्क(type_id),
            'uri': uri.replace('{version}', संस्करणम्_dir),
            'fileCrc': file_crc,
            }
    यदि दीर्घता(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', संस्करणम्_dir)
    यदि दीर्घता(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

सह open(filename, 'w') इव outfile:
    json.dump(component_general, outfile, क्रमय_कुञ्चिकाः=सत्य)

यदि compress:
    save_compressed(filename)
