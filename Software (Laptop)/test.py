from math import *

l1 = 180
l2 = 180

def calc_angles(x, y):
    d = sqrt(x*x + y*y)

    theta1 = acos((l1*l1 + d*d - l2*l2) / (2*l1*d))
    theta2 = acos((l2*l2 + l1 - d*d) / (2*l2*l1))

    J1 = theta1 + acos(x/d)
    J2 = theta2 - (3.14159/2)

    print(f'test: {(l1*l1 + d*d - l2*l2) / (2*l1*d)} theta1: {theta1}, theta2: {theta2}, J1: {J1}, J2: {J2}')


if __name__ == '__main__':
    calc_angles(150, 150)