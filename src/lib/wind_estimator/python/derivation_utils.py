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

ತಃ symforce ಆಮದು symbolic ಇವ sm
ತಃ symforce ಆಮದು geo
ತಃ symforce ಆಮದು typing ಇವ T

ಆಮದು re

# q: quaternion describing rotation from frame 1 to frame 2
# returns a rotation matrix derived form q which describes the same
# rotation
ವ್ಯಾಖ್ಯಾನ quat_ಗಮ್ಯ_rot(q):
    q0 = q[0]
    q1 = q[1]
    q2 = q[2]
    q3 = q[3]

    Rot = geo.M33([[q0**2 + q1**2 - q2**2 - q3**2, 2*(q1*q2 - q0*q3), 2*(q1*q3 + q0*q2)],
                  [2*(q1*q2 + q0*q3), q0**2 - q1**2 + q2**2 - q3**2, 2*(q2*q3 - q0*q1)],
                   [2*(q1*q3-q0*q2), 2*(q2*q3 + q0*q1), q0**2 - q1**2 - q2**2 + q3**2]])

    ಹಿಂತಿರುಗಿ Rot

ವ್ಯಾಖ್ಯಾನ sign_no_zero(x) -> T.Scalar:
    """
    Returns -1 if x is negative, 1 if x is positive, and 1 if x is zero
    """
    ಹಿಂತಿರುಗಿ 2 * sm.Min(sm.sign(x), 0) + 1

ವ್ಯಾಖ್ಯಾನ ಕೂಡಿಸು_epsilon_sign(ಅಭಿವ್ಯಕ್ತಿ, var, eps):
    # Avoids a singularity at 0 while keeping the derivative correct
    ಹಿಂತಿರುಗಿ ಅಭಿವ್ಯಕ್ತಿ.subs(var, var + eps * sign_no_zero(var))

ವ್ಯಾಖ್ಯಾನ generate_px4_function(function_name, output_ಹೆಸರುಗಳು):
    ತಃ symforce.codegen ಆಮದು Codegen, CppConfig
    ಆಮದು os
    ಆಮದು fileinput

    codegen = Codegen.function(
            function_name,
            output_ಹೆಸರುಗಳು=output_ಹೆಸರುಗಳು,
            config=CppConfig())
    metadata = codegen.generate_function(
            output_dir="generated",
            skip_directory_nesting=ನಿಜ)

    ಮುದ್ರಿಸು("Files generated in {}:\n".format(metadata.output_dir))
    ಗಾಗಿ f ಒಳಗೆ metadata.generated_files:
        ಮುದ್ರಿಸು("  |- {}".format(os.ಮಾರ್ಗ.relpath(f, metadata.output_dir)))

    # Replace cstdlib and Eigen functions by PX4 equivalents
    ಜೊತೆ fileinput.File_ಒಳಹರಿವು(os.ಮಾರ್ಗ.abspath(metadata.generated_files[0]), inplace=ನಿಜ, backup='.bak') ಇವ file:
        ಗಾಗಿ ಸಾಲು ಒಳಗೆ file:
            ಸಾಲು = ಸಾಲು.replace("std::max", "math::max")
            ಸಾಲು = ಸಾಲು.replace("std::min", "math::min")
            ಸಾಲು = ಸಾಲು.replace("Eigen", "matrix")
            ಸಾಲು = ಸಾಲು.replace("matrix/Dense", "matrix/math.hpp")

            # don't allow underscore + uppercase identifier naming (always reserved for any use)
            ಸಾಲು = re.sub(r'_([A-Z])', ಅನಾಮಿಕ x: '_' + x.ಗುಂಪು(1).ಅಧೋ(), ಸಾಲು)

            ಮುದ್ರಿಸು(ಸಾಲು, ಕೊನೆ='')

ವ್ಯಾಖ್ಯಾನ generate_python_function(function_name, output_ಹೆಸರುಗಳು):
    ತಃ symforce.codegen ಆಮದು Codegen, PythonConfig
    codegen = Codegen.function(
            function_name,
            output_ಹೆಸರುಗಳು=output_ಹೆಸರುಗಳು,
            config=PythonConfig())

    metadata = codegen.generate_function(
            output_dir="generated",
            skip_directory_nesting=ನಿಜ)
