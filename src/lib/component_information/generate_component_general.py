#!/usr/bin/env python

आयात argparse
आयात json
आयात lzma #to create .xz file
आयात re

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_GENERAL json file""")
parser.add_argument('filename', metavar='component_general.json', help='JSON output file')
parser.add_argument('--compress', action='store_true', help='Add a compressed output file')
parser.add_argument('--type', metavar='type', action="append", पूर्वनिर्धारित=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.add_argument('--version-file', metavar='build_git_version.h', help='Git version file')

args = parser.parse_args()
filename = args.filename
compress = args.compress
version_file = args.version_file

version_dir = ''
जर version_file is नाही None:
    साठी ओळ मध्ये open(version_file, "r"):
        version_शोध = re.शोध(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', ओळ)
        जर version_शोध:
            version_dir = version_शोध.group(1)
            थांबा


def save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    with lzma.open(xz_filename, 'wt', preset=9) इव f:
        with open(filename, 'r') इव content_file:
            f.लिख(content_file.पठ())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
साठी metadata_type_tuple मध्ये args.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.split(',')
    file_crc = 0
    साठी ओळ मध्ये open(metadata_file, "rb"):
        file_crc = crc_update(ओळ, crc_table, file_crc)
    json_type = {
            'type': पूर्णांक(type_id),
            'uri': uri.replace('{version}', version_dir),
            'fileCrc': file_crc,
            }
    जर len(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', version_dir)
    जर len(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

with open(filename, 'w') इव outfile:
    json.dump(component_general, outfile, sort_किल्ल्या=सत्य)

जर compress:
    save_compressed(filename)
