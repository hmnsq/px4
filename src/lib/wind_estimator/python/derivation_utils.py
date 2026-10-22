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

ਤਃ symforce ਆਯਾਤ symbolic ਇਵ sm
ਤਃ symforce ਆਯਾਤ geo
ਤਃ symforce ਆਯਾਤ typing ਇਵ T

ਆਯਾਤ re

# q: quaternion describing rotation from frame 1 to frame 2
# returns a rotation matrix derived form q which describes the same
# rotation
ਪਰਿਭਾਸ਼ਾ quat_ਮੰਜ਼ਿਲ_rot(q):
    q0 = q[0]
    q1 = q[1]
    q2 = q[2]
    q3 = q[3]

    Rot = geo.M33([[q0**2 + q1**2 - q2**2 - q3**2, 2*(q1*q2 - q0*q3), 2*(q1*q3 + q0*q2)],
                  [2*(q1*q2 + q0*q3), q0**2 - q1**2 + q2**2 - q3**2, 2*(q2*q3 - q0*q1)],
                   [2*(q1*q3-q0*q2), 2*(q2*q3 + q0*q1), q0**2 - q1**2 - q2**2 + q3**2]])

    ਵਾਪਸ Rot

ਪਰਿਭਾਸ਼ਾ sign_no_zero(x) -> T.Scalar:
    """
    Returns -1 if x is negative, 1 if x is positive, and 1 if x is zero
    """
    ਵਾਪਸ 2 * sm.Min(sm.sign(x), 0) + 1

ਪਰਿਭਾਸ਼ਾ ਜੋੜੋ_epsilon_sign(ਵਿਅੰਜਕ, var, eps):
    # Avoids a singularity at 0 while keeping the derivative correct
    ਵਾਪਸ ਵਿਅੰਜਕ.subs(var, var + eps * sign_no_zero(var))

ਪਰਿਭਾਸ਼ਾ generate_px4_function(function_name, output_ਨਾਂ):
    ਤਃ symforce.codegen ਆਯਾਤ Codegen, CppConfig
    ਆਯਾਤ os
    ਆਯਾਤ fileinput

    codegen = Codegen.function(
            function_name,
            output_ਨਾਂ=output_ਨਾਂ,
            config=CppConfig())
    metadata = codegen.generate_function(
            output_dir="generated",
            skip_directory_nesting=ਸੱਚ)

    ਛਾਪੋ("Files generated in {}:\n".format(metadata.output_dir))
    ਲਈ f ਵਿੱਚ metadata.generated_files:
        ਛਾਪੋ("  |- {}".format(os.ਰਸਤਾ.relpath(f, metadata.output_dir)))

    # Replace cstdlib and Eigen functions by PX4 equivalents
    ਨਾਲ fileinput.File_ਨਿਵੇਸ਼(os.ਰਸਤਾ.abspath(metadata.generated_files[0]), inplace=ਸੱਚ, backup='.bak') ਇਵ file:
        ਲਈ ਸਤਰ ਵਿੱਚ file:
            ਸਤਰ = ਸਤਰ.replace("std::max", "math::max")
            ਸਤਰ = ਸਤਰ.replace("std::min", "math::min")
            ਸਤਰ = ਸਤਰ.replace("Eigen", "matrix")
            ਸਤਰ = ਸਤਰ.replace("matrix/Dense", "matrix/math.hpp")

            # don't allow underscore + uppercase identifier naming (always reserved for any use)
            ਸਤਰ = re.sub(r'_([A-Z])', ਅਨਾਮ x: '_' + x.ਸਮੂਹ(1).ਹੇਠਲਾ(), ਸਤਰ)

            ਛਾਪੋ(ਸਤਰ, ਸਿਰਾ='')

ਪਰਿਭਾਸ਼ਾ generate_python_function(function_name, output_ਨਾਂ):
    ਤਃ symforce.codegen ਆਯਾਤ Codegen, PythonConfig
    codegen = Codegen.function(
            function_name,
            output_ਨਾਂ=output_ਨਾਂ,
            config=PythonConfig())

    metadata = codegen.generate_function(
            output_dir="generated",
            skip_directory_nesting=ਸੱਚ)
