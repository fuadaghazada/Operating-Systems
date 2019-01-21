import os

# Simple test program for checking all cases
# To run: python test_case.py

def compare_files(file1, file2):
    command = 'cmp --silent ' + str(file1) + ' ' + str(file2) + '&& echo "Results are the same with expected" || echo "Results are wrong"'
    os.system(command)

def run_test(case, obj, min, max, bin_count, num_files, batch_size = ''):

    command = ''

    if (obj == 'p'):
        command += './syn_phistogram '
    else:
        command += './syn_thistogram '

    command += (str(min) + ' ' + str(max) + ' ' + str(bin_count) + ' ' + str(num_files) + ' ')

    if case >= 4:
        filename = 'test' + str(case) + '-'
    else:
        filename = 'test'

    if case is not 2:
        for i in range(1, num_files + 1):
            command += (filename + str(i) + ' ')
    else:
        command += filename + '2 '

    command += ('myout' + str(case) + ' ')

    if batch_size is not '':
        command += str(batch_size)

    os.system(command)
    compare_files('results/myout' + str(case), 'results/output' + str(case))


# Test
os.system('make clean')
os.system('make')


# Case - 1
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
print("\nCase: 1")
run_test(1, 'p', 0, 10, 5, 1)
print("---------------------------------------------------------------")
run_test(1, 't', 0, 10, 5, 1, 1)
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")

# Case - 2
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
print("\nCase: 2")
run_test(2, 'p', 0, 50, 10, 1)
print("---------------------------------------------------------------")
run_test(2, 't', 0, 50, 10, 2, 2)
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")

# Case - 3
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
print("\nCase: 3")
run_test(3, 'p', 0, 50, 10, 3)
print("---------------------------------------------------------------")
run_test(3, 't', 0, 50, 10, 3, 10)
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")

# Case - 4
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
print("\nCase: 4")
run_test(4, 'p', 0, 50, 10, 10)
print("---------------------------------------------------------------")
run_test(4, 't', 0, 50, 10, 10, 50)
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")

# Case - 5
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
print("\nCase: 5")
run_test(5, 'p', 0, 10000, 1000, 20)
print("---------------------------------------------------------------")
run_test(5, 't', 0, 10000, 1000, 20, 100)
print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")


os.system('make clean')
