#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
    Copyright (c) 2022 PX4 Development Team
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.
    3. Neither the name PX4 nor the names of its contributors may be
    used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

File: derivation_utils.py
Description:
    Common functions used for the derivation of most estimators
"""

तः symforce आनयन symbolic इव sm
तः symforce आनयन geo
तः symforce आनयन typing इव T

आनयन re

# q: quaternion describing rotation from frame 1 to frame 2
# returns a rotation matrix derived form q which describes the same
# rotation
परिभाषा quat_गन्तव्यम्_rot(q):
    q0 = q[0]
    q1 = q[1]
    q2 = q[2]
    q3 = q[3]

    Rot = geo.M33([[q0**2 + q1**2 - q2**2 - q3**2, 2*(q1*q2 - q0*q3), 2*(q1*q3 + q0*q2)],
                  [2*(q1*q2 + q0*q3), q0**2 - q1**2 + q2**2 - q3**2, 2*(q2*q3 - q0*q1)],
                   [2*(q1*q3-q0*q2), 2*(q2*q3 + q0*q1), q0**2 - q1**2 - q2**2 + q3**2]])

    प्रत्यागम Rot

परिभाषा sign_no_zero(x) -> T.Scalar:
    """
    Returns -1 if x is negative, 1 if x is positive, and 1 if x is zero
    """
    प्रत्यागम 2 * sm.Min(sm.sign(x), 0) + 1

परिभाषा सङ्कलय_epsilon_sign(व्यञ्जकम्, var, eps):
    # Avoids a singularity at 0 while keeping the derivative correct
    प्रत्यागम व्यञ्जकम्.subs(var, var + eps * sign_no_zero(var))

परिभाषा generate_px4_function(function_name, output_नामानि):
    तः symforce.codegen आनयन Codegen, CppConfig
    आनयन os
    आनयन fileinput

    codegen = Codegen.function(
            function_name,
            output_नामानि=output_नामानि,
            config=CppConfig())
    metadata = codegen.generate_function(
            output_dir="generated",
            skip_directory_nesting=सत्य)

    मुद्रय("Files generated in {}:\n".format(metadata.output_dir))
    प्रति f अन्तः metadata.generated_files:
        मुद्रय("  |- {}".format(os.मार्गः.relpath(f, metadata.output_dir)))

    # Replace cstdlib and Eigen functions by PX4 equivalents
    सह fileinput.File_निवेशः(os.मार्गः.abspath(metadata.generated_files[0]), inplace=सत्य, backup='.bak') इव file:
        प्रति पङ्क्तिः अन्तः file:
            पङ्क्तिः = पङ्क्तिः.replace("std::max", "math::max")
            पङ्क्तिः = पङ्क्तिः.replace("std::min", "math::min")
            पङ्क्तिः = पङ्क्तिः.replace("Eigen", "matrix")
            पङ्क्तिः = पङ्क्तिः.replace("matrix/Dense", "matrix/math.hpp")

            # don't allow underscore + uppercase identifier naming (always reserved for any use)
            पङ्क्तिः = re.sub(r'_([A-Z])', अनामन् x: '_' + x.गणः(1).अधस्तनम्(), पङ्क्तिः)

            मुद्रय(पङ्क्तिः, अन्तः_2='')

परिभाषा generate_python_function(function_name, output_नामानि):
    तः symforce.codegen आनयन Codegen, PythonConfig
    codegen = Codegen.function(
            function_name,
            output_नामानि=output_नामानि,
            config=PythonConfig())

    metadata = codegen.generate_function(
            output_dir="generated",
            skip_directory_nesting=सत्य)
