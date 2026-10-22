#!/usr/bin/env python

இறக்குமதி argparse
இறக்குமதி json
இறக்குமதி lzma #to create .xz file
இறக்குமதி re

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_GENERAL json file""")
parser.add_argument('filename', metavar='component_general.json', help='JSON output file')
parser.add_argument('--compress', action='store_true', help='Add a compressed output file')
parser.add_argument('--type', metavar='type', action="append", இயல்பு=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.add_argument('--version-file', metavar='build_git_version.h', help='Git version file')

args = parser.parse_args()
filename = args.filename
compress = args.compress
version_file = args.version_file

version_dir = ''
எனில் version_file is இல்லை None:
    க்கு வரி உள் open(version_file, "r"):
        version_தேடல் = re.தேடல்(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', வரி)
        எனில் version_தேடல்:
            version_dir = version_தேடல்.group(1)
            நிறுத்து


def save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    with lzma.open(xz_filename, 'wt', preset=9) இவ f:
        with open(filename, 'r') இவ content_file:
            f.லிக(content_file.பட())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
க்கு metadata_type_tuple உள் args.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.split(',')
    file_crc = 0
    க்கு வரி உள் open(metadata_file, "rb"):
        file_crc = crc_update(வரி, crc_table, file_crc)
    json_type = {
            'type': முழு(type_id),
            'uri': uri.replace('{version}', version_dir),
            'fileCrc': file_crc,
            }
    எனில் len(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', version_dir)
    எனில் len(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

with open(filename, 'w') இவ outfile:
    json.dump(component_general, outfile, sort_சாவிகள்=உண்மை)

எனில் compress:
    save_compressed(filename)
