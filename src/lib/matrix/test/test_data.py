#!/usr/bin/env python3

తః pylab దిగుమతి *
తః pprint దిగుమతి pprint
దిగుమతి scipy.linalg
దిగుమతి sys

# test cases, derived from doc/nasa_rotation_def.pdf

#pylint: disable=all

నిర్వచనం euler_గమ్యం_quat(phi, theta, psi):
    "Quaternion from (body 3(psi)-2(theta)-1(phi) euler angles"
    s1 = sin(psi/2)
    c1 = cos(psi/2)
    s2 = sin(theta/2)
    c2 = cos(theta/2)
    s3 = sin(phi/2)
    c3 = cos(phi/2)
    తిరిగి array([
        s1*s2*s3 + c1*c2*c3,
        -s1*s2*c3 + s3*c1*c2,
        s1*s3*c2 + s2*c1*c3,
        s1*c2*c3 - s2*s3*c1
    ])

నిర్వచనం euler_గమ్యం_dcm(phi, theta, psi):
    s1 = sin(psi)
    c1 = cos(psi)
    s2 = sin(theta)
    c2 = cos(theta)
    s3 = sin(phi)
    c3 = cos(phi)
    తిరిగి array([
        [c1*c2, c1*s2*s3 - s1*c3, c1*s2*c3 + s1*s3],
        [s1*c2, s1*s2*s3 + c1*c3, s1*s2*c3 - c1*s3],
        [-s2, c2*s3, c2*c3],
    ])

నిర్వచనం quat_prod(q, r):
    "Quaternion product"
    తిరిగి array([
        r[0]*q[0] - r[1]*q[1] - r[2]*q[2] - r[3]*q[3],
        r[0]*q[1] + r[1]*q[0] - r[2]*q[3] + r[3]*q[2],
        r[0]*q[2] + r[1]*q[3] + r[2]*q[0] - r[3]*q[1],
        r[0]*q[3] - r[1]*q[2] + r[2]*q[1] + r[3]*q[0]
    ])

నిర్వచనం dcm_గమ్యం_euler(dcm):
    తిరిగి array([
        arctan(dcm[2,1]/ dcm[2,2]),
        arctan(-dcm[2,0]/ (1 - dcm[2,0]**2)**0.5),
        arctan(dcm[1,0]/ dcm[0,0]),
    ])

నిర్వచనం dcm_from_quat(q):
    q1 = q[0]
    q2 = q[1]
    q3 = q[2]
    q4 = q[3]
    తిరిగి array([
        [q1*q1 + q2*q2 - q3*q3 - q4*q4, 2*(q2*q3 - q1*q4), 2*(q2*q4 + q1*q3)],
        [2*(q2*q3 + q1*q4), q1*q1 - q2*q2 + q3*q3 - q4*q4, 2*(q3*q4 - q1*q2)],
        [2*(q2*q4 - q1*q3), 2*(q3*q4 + q1*q2), q1*q1 - q2*q2 - q3*q3 + q4*q4]
    ])

నిర్వచనం quat_గమ్యం_euler(q):
    "Quaternion to (body 3(psi)-2(theta)-1(phi) euler angles"
    తిరిగి dcm_గమ్యం_euler(dcm_from_quat(q))

నిర్వచనం quat_గమ్యం_dcm(q):
    తిరిగి euler_గమ్యం_dcm(quat_గమ్యం_euler(q))

phi = 0.1
theta = 0.2
psi = 0.3
ముద్రించు('euler', phi, theta, psi)

q = euler_గమ్యం_quat(phi, theta, psi)
FLT_EPSILON = sys.float_info.epsilon
ప్రతిజ్ఞా(abs(norm(q) - 1) < FLT_EPSILON)
ప్రతిజ్ఞా(abs(norm(q) - 1) < FLT_EPSILON)
ప్రతిజ్ఞా(norm(array(quat_గమ్యం_euler(q)) - array([phi, theta, psi])) < FLT_EPSILON)
ముద్రించు('\nq:')
pprint(q)

dcm = euler_గమ్యం_dcm(phi, theta, psi)
ప్రతిజ్ఞా(norm(dcm[:,0]) == 1)
ప్రతిజ్ఞా(norm(dcm[:,1]) == 1)
ప్రతిజ్ఞా(norm(dcm[:,2]) == 1)
ప్రతిజ్ఞా(abs(dcm[:,0].dot(dcm[:,1])) < FLT_EPSILON)
ప్రతిజ్ఞా(abs(dcm[:,0].dot(dcm[:,2])) < FLT_EPSILON)
ముద్రించు('\ndcm:')
pprint(dcm)

ముద్రించు('\nq*q', quat_prod(q, q))

q2 = quat_prod(q, q)
pprint(q2)
ముద్రించు(norm(q2))

ముద్రించు('\nq3:')
q3 = array([1,2,3,4])
pprint(q3)
ముద్రించు('\nq3_norm:')
q3_norm =q3 / norm(q3)
pprint(q3_norm)

ముద్రించు('\ninverse')
A = array([[0,2,3], [4,5,6], [7,8,10]])
pprint(A)
pprint(inv(A))

ముద్రించు('\nmatrix exponential')
A = 0.01*array([[1.0,2.0,3.0], [4.0,5.0,6.0], [7.0,8.0,10.0]])
eA_check = scipy.linalg.expm(A)

pprint(eA_check)

eA_approx = eye(3)
k = 1.0
A_pow = A
కొరకు i లో శ్రేణి(1,3):
    k *= i
    # print(i, k, '\n', A_pow/k, '\n')
    eA_approx +=  A_pow/k
    A_pow = A_pow.dot(A)
ముద్రించు(eA_approx)

ముద్రించు('\nqr decomposition 4x4')
A = array([[20.0, -10.0, -13.0, 21.0], [ 17.0, 16.0, -18.0, -14], [0.7, -0.8, 0.9, -0.5], [-1.0, -1.1, -1.2, -1.3]])
b = array([[2.], [3.], [4.], [5.]])
x = scipy.linalg.lstsq(A,b)[0]
ముద్రించు('A:')
pprint(A)
ముద్రించు('b:')
pprint(b)
ముద్రించు('x:')
pprint(scipy.linalg.lstsq(A,b)[0])

ముద్రించు('\nqr decomposition 4x3')
A = array([[20.0, -10.0, -13.0], [ 17.0, 16.0, -18.0], [0.7, -0.8, 0.9], [-1.0, -1.1, -1.2]])
b = array([[2.], [3.], [4.], [5.]])
x = scipy.linalg.lstsq(A,b)[0]
ముద్రించు('A:')
pprint(A)
ముద్రించు('b:')
pprint(b)
ముద్రించు('x:')
pprint(x)
