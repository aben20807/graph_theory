# HW3

## Description
+ implement Hungarian algorithm

## Usage
```bash
$ gcc hungarian_n_imp.c
$ ./a.out < test1.in
```
```bash
$ gcc hungarian_n_m.c
$ ./a.out < test2.in
```

## Input
+ 2 ≦ n,m ≦ 10000, n, m are integers.
+ weights are all non-negative integers, smaller than 10^16 (long long).

## Output
+ the maximum weight matching

## Sample input and output
+ `hungarian_n_imp.c`:
```
5 5
4 1 6 2 3
5 0 3 7 6
2 3 4 5 8
3 4 6 3 4
4 6 5 8 6
```
```
31
```

+ `hungarian_n_m.c`:
```
4 5
4 1 6 2 3
5 0 3 7 6
2 3 4 5 8
3 4 6 3 4
```
```
25
```
