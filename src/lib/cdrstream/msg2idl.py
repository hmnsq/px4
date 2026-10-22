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

ਆਯਾਤ argparse
ਆਯਾਤ builtins
ਆਯਾਤ pathlib
ਆਯਾਤ sys

ਤਃ rosidl_adapter.ਸੁਨੇਹਾ_2 ਆਯਾਤ convert_ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_idl

ਜੇ __name__ == '__main__':
    parser = argparse.ਦਲੀਲ_Parser(
        description=f'Convert px4 .msg files to .idl')
    parser.ਜੋੜੋ_ਦਲੀਲ(
        'interface_files', nargs='+',
        help='The interface files to convert')
    ਦਲੀਲਾਂ = parser.parse_ਦਲੀਲਾਂ(sys.argv[1:])

    # convert_msg_to_idl prints a Reading/Writing line per file. Filter those
    # via builtins.print instead of redirecting stdout, which the empy template
    # engine relies on and breaks if replaced. Errors still surface on stderr.
    real_print = builtins.ਛਾਪੋ

    ਪਰਿਭਾਸ਼ਾ quiet_print(*pargs, **kwargs):
        ਜੇ pargs ਅਤੇ isinstance(pargs[0], str) ਅਤੇ \
                pargs[0].startswith(('Reading input file:', 'Writing output file:')):
            ਵਾਪਸ
        real_print(*pargs, **kwargs)

    builtins.ਛਾਪੋ = quiet_print
    ਕੋਸ਼ਿਸ਼:
        ਲਈ interface_file ਵਿੱਚ ਦਲੀਲਾਂ.interface_files:
            interface_file = pathlib.ਰਸਤਾ_2(interface_file)
            package_dir = interface_file.ਮਾਪਾ.absolute()

            convert_ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_idl(
                package_dir, 'px4_msgs',
                interface_file.absolute().relative_ਮੰਜ਼ਿਲ(package_dir),
                interface_file.ਮਾਪਾ)
    ਅਖੀਰ:
        builtins.ਛਾਪੋ = real_print
