#!/usr/bin/env python3

سے pylab درآمد *
سے pprint درآمد pprint
درآمد scipy.linalg
درآمد sys

# test cases, derived from doc/nasa_rotation_def.pdf

#pylint: disable=all

تعریف euler_مقصد_quat(phi, theta, psi):
    "Quaternion from (body 3(psi)-2(theta)-1(phi) euler angles"
    s1 = sin(psi/2)
    c1 = cos(psi/2)
    s2 = sin(theta/2)
    c2 = cos(theta/2)
    s3 = sin(phi/2)
    c3 = cos(phi/2)
    واپس array([
        s1*s2*s3 + c1*c2*c3,
        -s1*s2*c3 + s3*c1*c2,
        s1*s3*c2 + s2*c1*c3,
        s1*c2*c3 - s2*s3*c1
    ])

تعریف euler_مقصد_dcm(phi, theta, psi):
    s1 = sin(psi)
    c1 = cos(psi)
    s2 = sin(theta)
    c2 = cos(theta)
    s3 = sin(phi)
    c3 = cos(phi)
    واپس array([
        [c1*c2, c1*s2*s3 - s1*c3, c1*s2*c3 + s1*s3],
        [s1*c2, s1*s2*s3 + c1*c3, s1*s2*c3 - c1*s3],
        [-s2, c2*s3, c2*c3],
    ])

تعریف quat_prod(q, r):
    "Quaternion product"
    واپس array([
        r[0]*q[0] - r[1]*q[1] - r[2]*q[2] - r[3]*q[3],
        r[0]*q[1] + r[1]*q[0] - r[2]*q[3] + r[3]*q[2],
        r[0]*q[2] + r[1]*q[3] + r[2]*q[0] - r[3]*q[1],
        r[0]*q[3] - r[1]*q[2] + r[2]*q[1] + r[3]*q[0]
    ])

تعریف dcm_مقصد_euler(dcm):
    واپس array([
        arctan(dcm[2,1]/ dcm[2,2]),
        arctan(-dcm[2,0]/ (1 - dcm[2,0]**2)**0.5),
        arctan(dcm[1,0]/ dcm[0,0]),
    ])

تعریف dcm_from_quat(q):
    q1 = q[0]
    q2 = q[1]
    q3 = q[2]
    q4 = q[3]
    واپس array([
        [q1*q1 + q2*q2 - q3*q3 - q4*q4, 2*(q2*q3 - q1*q4), 2*(q2*q4 + q1*q3)],
        [2*(q2*q3 + q1*q4), q1*q1 - q2*q2 + q3*q3 - q4*q4, 2*(q3*q4 - q1*q2)],
        [2*(q2*q4 - q1*q3), 2*(q3*q4 + q1*q2), q1*q1 - q2*q2 - q3*q3 + q4*q4]
    ])

تعریف quat_مقصد_euler(q):
    "Quaternion to (body 3(psi)-2(theta)-1(phi) euler angles"
    واپس dcm_مقصد_euler(dcm_from_quat(q))

تعریف quat_مقصد_dcm(q):
    واپس euler_مقصد_dcm(quat_مقصد_euler(q))

phi = 0.1
theta = 0.2
psi = 0.3
چھاپو('euler', phi, theta, psi)

q = euler_مقصد_quat(phi, theta, psi)
FLT_EPSILON = sys.float_info.epsilon
أكد(abs(norm(q) - 1) < FLT_EPSILON)
أكد(abs(norm(q) - 1) < FLT_EPSILON)
أكد(norm(array(quat_مقصد_euler(q)) - array([phi, theta, psi])) < FLT_EPSILON)
چھاپو('\nq:')
pprint(q)

dcm = euler_مقصد_dcm(phi, theta, psi)
أكد(norm(dcm[:,0]) == 1)
أكد(norm(dcm[:,1]) == 1)
أكد(norm(dcm[:,2]) == 1)
أكد(abs(dcm[:,0].dot(dcm[:,1])) < FLT_EPSILON)
أكد(abs(dcm[:,0].dot(dcm[:,2])) < FLT_EPSILON)
چھاپو('\ndcm:')
pprint(dcm)

چھاپو('\nq*q', quat_prod(q, q))

q2 = quat_prod(q, q)
pprint(q2)
چھاپو(norm(q2))

چھاپو('\nq3:')
q3 = array([1,2,3,4])
pprint(q3)
چھاپو('\nq3_norm:')
q3_norm =q3 / norm(q3)
pprint(q3_norm)

چھاپو('\ninverse')
A = array([[0,2,3], [4,5,6], [7,8,10]])
pprint(A)
pprint(inv(A))

چھاپو('\nmatrix exponential')
A = 0.01*array([[1.0,2.0,3.0], [4.0,5.0,6.0], [7.0,8.0,10.0]])
eA_check = scipy.linalg.expm(A)

pprint(eA_check)

eA_approx = eye(3)
k = 1.0
A_pow = A
برائے i میں سلسلہ(1,3):
    k *= i
    # print(i, k, '\n', A_pow/k, '\n')
    eA_approx +=  A_pow/k
    A_pow = A_pow.dot(A)
چھاپو(eA_approx)

چھاپو('\nqr decomposition 4x4')
A = array([[20.0, -10.0, -13.0, 21.0], [ 17.0, 16.0, -18.0, -14], [0.7, -0.8, 0.9, -0.5], [-1.0, -1.1, -1.2, -1.3]])
b = array([[2.], [3.], [4.], [5.]])
x = scipy.linalg.lstsq(A,b)[0]
چھاپو('A:')
pprint(A)
چھاپو('b:')
pprint(b)
چھاپو('x:')
pprint(scipy.linalg.lstsq(A,b)[0])

چھاپو('\nqr decomposition 4x3')
A = array([[20.0, -10.0, -13.0], [ 17.0, 16.0, -18.0], [0.7, -0.8, 0.9], [-1.0, -1.1, -1.2]])
b = array([[2.], [3.], [4.], [5.]])
x = scipy.linalg.lstsq(A,b)[0]
چھاپو('A:')
pprint(A)
چھاپو('b:')
pprint(b)
چھاپو('x:')
pprint(x)
