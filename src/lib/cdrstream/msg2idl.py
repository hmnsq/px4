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

استورد argparse
استورد builtins
استورد pathlib
استورد sys

من rosidl_adapter.پیغام استورد convert_پیغام_منزل_idl

إذا __name__ == '__main__':
    parser = argparse.دلیل_Parser(
        description=f'Convert px4 .msg files to .idl')
    parser.جوڑو_دلیل(
        'interface_files', nargs='+',
        help='The interface files to convert')
    دلیلاں = parser.parse_دلیلاں(sys.argv[1:])

    # convert_msg_to_idl prints a Reading/Writing line per file. Filter those
    # via builtins.print instead of redirecting stdout, which the empy template
    # engine relies on and breaks if replaced. Errors still surface on stderr.
    real_print = builtins.چھاپو

    تعریف quiet_print(*pargs, **kwargs):
        إذا pargs و isinstance(pargs[0], str) و \
                pargs[0].startswith(('Reading input file:', 'Writing output file:')):
            أرجع
        real_print(*pargs, **kwargs)

    builtins.چھاپو = quiet_print
    حاول:
        لكل interface_file في دلیلاں.interface_files:
            interface_file = pathlib.رستہ_2(interface_file)
            package_dir = interface_file.ماپا.absolute()

            convert_پیغام_منزل_idl(
                package_dir, 'px4_msgs',
                interface_file.absolute().relative_منزل(package_dir),
                interface_file.ماپا)
    أخيرا:
        builtins.چھاپو = real_print
