#!/usr/bin/env python

ਆਯਾਤ argparse
ਆਯਾਤ json
ਆਯਾਤ lzma #to create .xz file
ਆਯਾਤ re

parser = argparse.ਦਲੀਲ_Parser(description="""Generate the COMPONENT_GENERAL json file""")
parser.ਜੋੜੋ_ਦਲੀਲ('filename', metavar='component_general.json', help='JSON output file')
parser.ਜੋੜੋ_ਦਲੀਲ('--compress', action='store_true', help='Add a compressed output file')
parser.ਜੋੜੋ_ਦਲੀਲ('--type', metavar='type', action="append", ਮੂਲ=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.ਜੋੜੋ_ਦਲੀਲ('--version-file', metavar='build_git_version.h', help='Git version file')

ਦਲੀਲਾਂ = parser.parse_ਦਲੀਲਾਂ()
filename = ਦਲੀਲਾਂ.filename
compress = ਦਲੀਲਾਂ.compress
ਸੰਸਕਰਣ_file = ਦਲੀਲਾਂ.ਸੰਸਕਰਣ_file

ਸੰਸਕਰਣ_dir = ''
ਜੇ ਸੰਸਕਰਣ_file ਹੈ ਨਹੀਂ ਕੁਝ_ਨਹੀਂ:
    ਲਈ ਸਤਰ ਵਿੱਚ open(ਸੰਸਕਰਣ_file, "r"):
        ਸੰਸਕਰਣ_ਖੋਜ = re.ਖੋਜ(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', ਸਤਰ)
        ਜੇ ਸੰਸਕਰਣ_ਖੋਜ:
            ਸੰਸਕਰਣ_dir = ਸੰਸਕਰਣ_ਖੋਜ.ਸਮੂਹ(1)
            ਰੋਕੋ


ਪਰਿਭਾਸ਼ਾ save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    ਨਾਲ lzma.open(xz_filename, 'wt', preset=9) ਇਵ f:
        ਨਾਲ open(filename, 'r') ਇਵ content_file:
            f.ਲਿਖ(content_file.ਪਠ())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
ਲਈ metadata_type_tuple ਵਿੱਚ ਦਲੀਲਾਂ.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.ਵੰਡੋ(',')
    file_ਚੱਕਰੀ_ਜਾਂਚ = 0
    ਲਈ ਸਤਰ ਵਿੱਚ open(metadata_file, "rb"):
        file_ਚੱਕਰੀ_ਜਾਂਚ = ਚੱਕਰੀ_ਜਾਂਚ_update(ਸਤਰ, ਚੱਕਰੀ_ਜਾਂਚ_ਸਾਰਣੀ, file_ਚੱਕਰੀ_ਜਾਂਚ)
    json_type = {
            'type': ਪੂਰਨ(type_id),
            'uri': uri.replace('{version}', ਸੰਸਕਰਣ_dir),
            'fileCrc': file_ਚੱਕਰੀ_ਜਾਂਚ,
            }
    ਜੇ ਲੰਬਾਈ(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', ਸੰਸਕਰਣ_dir)
    ਜੇ ਲੰਬਾਈ(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

ਨਾਲ open(filename, 'w') ਇਵ outfile:
    json.dump(component_general, outfile, ਛਾਂਟੋ_ਕੁੰਜੀਆਂ=ਸੱਚ)

ਜੇ compress:
    save_compressed(filename)
