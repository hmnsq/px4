#!/usr/bin/env python

درآمد argparse
درآمد json
درآمد lzma #to create .xz file
درآمد re

parser = argparse.دلیل_Parser(description="""Generate the COMPONENT_GENERAL json file""")
parser.جوڑو_دلیل('filename', metavar='component_general.json', help='JSON output file')
parser.جوڑو_دلیل('--compress', action='store_true', help='Add a compressed output file')
parser.جوڑو_دلیل('--type', metavar='type', action="append", طےشدہ=[],
        help='Metadata type (<type>,<metadata file>,<uri>,<fallback uri>,[translation uri])')
parser.جوڑو_دلیل('--version-file', metavar='build_git_version.h', help='Git version file')

دلائل = parser.parse_دلائل()
filename = دلائل.filename
compress = دلائل.compress
نسخہ_file = دلائل.نسخہ_file

نسخہ_dir = ''
اگر نسخہ_file ہے نہیں کچھ_نہیں:
    برائے سطر میں open(نسخہ_file, "r"):
        نسخہ_تلاش = re.تلاش(r'PX4_GIT_TAG_OR_BRANCH_NAME\s+"(.+)"', سطر)
        اگر نسخہ_تلاش:
            نسخہ_dir = نسخہ_تلاش.گروہ(1)
            توڑو


تعریف save_compressed(filename):
    #create lzma compressed version
    xz_filename=filename+'.xz'
    ساتھ lzma.open(xz_filename, 'wt', preset=9) بطور f:
        ساتھ open(filename, 'r') بطور content_file:
            f.اكتب(content_file.اقرأ())

component_general = {}
component_general['version'] = 1

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

metadata_types = []
برائے metadata_type_tuple میں دلائل.type:
    type_id, metadata_file, uri, fallback_uri, translation_uri = metadata_type_tuple.بانٹو(',')
    file_چکری_جانچ = 0
    برائے سطر میں open(metadata_file, "rb"):
        file_چکری_جانچ = چکری_جانچ_update(سطر, چکری_جانچ_جدول, file_چکری_جانچ)
    json_type = {
            'type': صحیح(type_id),
            'uri': uri.replace('{version}', نسخہ_dir),
            'fileCrc': file_چکری_جانچ,
            }
    اگر لمبائی(fallback_uri) > 0:
        json_type['uriFallback'] = fallback_uri.replace('{version}', نسخہ_dir)
    اگر لمبائی(translation_uri) > 0:
        json_type['translationUri'] = translation_uri
    metadata_types.append(json_type)
component_general['metadataTypes'] = metadata_types

ساتھ open(filename, 'w') بطور outfile:
    json.dump(component_general, outfile, ترتیب_دو_کلیدیں=سچ)

اگر compress:
    save_compressed(filename)
