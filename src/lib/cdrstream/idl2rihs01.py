#!/usr/bin/env python3

############################################################################
#
#   Copyright (C) 2023 PX4 Development Team. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name PX4 nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
############################################################################

درآمد argparse
درآمد pathlib
درآمد sys
درآمد os
درآمد tempfile
درآمد json

کوشش:
    سے rosidl_generator_type_description درآمد generate_type_hash
استثنا ImportError:
    # modifying sys.path and importing the Python package with the same
    # name as this script does not work on Windows
    rosidl_generator_type_description_جڑ = os.راستہ.dirname(os.راستہ.dirname(__file__))
    rosidl_generator_type_description_module = os.راستہ.join(
        rosidl_generator_type_description_جڑ, 'rosidl_generator_type_description', '__init__.py')
    اگر نہیں os.راستہ.exists(rosidl_generator_type_description_module):
        اٹھاؤ
    سے importlib.machinery درآمد ماخذ_File_Loader

    loader = ماخذ_File_Loader('rosidl_generator_type_description', rosidl_generator_type_description_module)
    rosidl_generator_type_description = loader.بھرو_module()
    generate_type_hash = rosidl_generator_type_description.generate_type_hash

اگر __name__ == '__main__':
    parser = argparse.دلیل_Parser(
        description=f'Convert px4 .idl files to rihs01')
    parser.جوڑو_دلیل(
        'interface_files', nargs='+',
        help='The interface files to convert')
    parser.جوڑو_دلیل(
        '--output-dir', '-o',
         help='The directory to save converted files (default: current directory)')
    دلائل = parser.parse_دلائل(sys.argv[1:])

    # So for some odd reason rosidl doesn't do proper cli arguments but believes
    # that some magically crafted json is better

    idl_files = []

    type_hash_arguments = {}
    type_hash_arguments['package_name'] = "px4_msgs"
    type_hash_arguments['output_dir'] = دلائل.output_dir
    type_hash_arguments["idl_tuples"] = idl_files

    برائے interface_file میں دلائل.interface_files:
        # So file path need to be magically encoded with a : to let the parser do its thing

        interface_file = str(pathlib.راستہ_2(interface_file)).replace("px4/msg", "px4:msg")
        idl_files.append(interface_file)

    json_file = tempfile.NamedTemporaryFile(منوال="w+")
    json.dump(type_hash_arguments, json_file)
    json_file.flush()

    generate_type_hash(json_file.name)
