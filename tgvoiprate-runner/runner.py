import os
import platform
import shutil
import subprocess
import sys
import time

def rate_exec_name():
    if platform.system() == 'Windows':
        return 'tgvoiprate.exe'
    else:
        return 'tgvoiprate'

def rate_exec():
    if platform.system() == 'Windows':
        return rate_exec_name()
    else:
        return './' + rate_exec_name()

def do_rate(ref_file_name, test_file_name, logger=None):
    s = subprocess.run(
        [rate_exec(), ref_file_name, test_file_name],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        universal_newlines=True,
    )

    if logger:
        logger.write(s.stdout + '\n')
    else:
        print(s.stdout)

def do_run_allcalls(logger=None):
    for root, dirs, files in os.walk('../tgvoipcall-runner'):
        if 'samples' in dirs:
            dirs.remove('samples')

        dirs.sort()

        if 'config.json' in files:
            for file in files:
                if os.path.splitext(file)[1] == '.ogg':
                    test_file_name = os.path.join(root, file)
                    ref_file_name = os.path.join('../tgvoipcall-runner/samples/', file[11:])

                    if logger:
                        logger.write('Processing {0} ref={1} test={2}\n'.format(root, ref_file_name, test_file_name))
                    else:
                        print('Processing {0} ref={1} test={2}'.format(root, ref_file_name, test_file_name))

                    do_rate(ref_file_name, test_file_name, logger)

                    comment_file_name = test_file_name + '.txt'
                    if os.path.exists(comment_file_name):
                        with open(comment_file_name, 'r') as comment_file:
                            comment = comment_file.read()
                        if logger:
                            logger.write(comment + '\n')
                        else:
                            print(comment)

def do_run_allentrys():
    for root, dirs, files in os.walk('entrys'):
        dirs.sort()

        if rate_exec_name() in files:
            entry_name = os.path.basename(root)
            print('Processing {}'.format(entry_name))

            shutil.copy2(os.path.join(root, rate_exec_name()), './')

            start = time.perf_counter_ns()
            
            with open(entry_name + '.txt', 'w') as f:
                do_run_allcalls(f)
                f.write('elapsed in {} ms'.format((time.perf_counter_ns() - start) / 1000000))

            print('{} elapsed in {} ms'.format(entry_name, (time.perf_counter_ns() - start) / 1000000))

def main():
    testEntrys = (len(sys.argv) > 1) and (sys.argv[1] == 'entrys')

    if testEntrys:
        do_run_allentrys()
    else:
        do_run_allcalls()

if __name__ == '__main__':
    main()
