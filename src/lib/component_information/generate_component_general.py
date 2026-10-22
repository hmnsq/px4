#!/usr/bin/env python

import argparse
import json
import lzma #to create .xz file
import re

parser = argparse.ArgumentParser(description="""Generate the COMPONENT_GENERAL json file""")
parser.add_argument('filename', metavar='component_general.json', help='JSON output file')
parser.add_argument('--compress', action='store_true', help='Add a compressed output file')
parser.add_argument('--type', metavar='type', action="append", default=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.add_argument('--version-file', metavar='build_git_version.h', help='Git version file')

args = parser.parse_args()
filename = args.filename
compress = args.compress
version_file = args.version_file

version_dir = ''
もし version_file is not None:
    ために line in open(version_file, "r"):
        version_search = re.search(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', line)
        もし version_search:
            version_dir = version_search.group(1)
            break


def save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    with lzma.open(xz_filename, 'wt', preset=9) as f:
        with open(filename, 'r') as content_file:
            f.write(content_file.read())

component_general = {}
component_general['version'] = 1

def create_table():
    a = []
    ために i in range(256):
        k = i
        ために j in range(8):
            もし k & 1:
                k ^= 0x1db710640
            k >>= 1
        a.append(k)
    戻る a

# MAVLink's CRC32
def crc_update(buf, crc_table, crc):
    ために k in buf:
        crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ k]
    戻る crc

crc_table = create_table()

metadata_types = []
ために metadata_type_tuple in args.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.split(',')
    file_crc = 0
    ために line in open(metadata_file, "rb"):
        file_crc = crc_update(line, crc_table, file_crc)
    json_type = {
            'type': 整数(type_id),
            'uri': uri.replace('{version}', version_dir),
            'fileCrc': file_crc,
            }
    もし len(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', version_dir)
    もし len(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

with open(filename, 'w') as outfile:
    json.dump(component_general, outfile, sort_keys=True)

もし compress:
    save_compressed(filename)
