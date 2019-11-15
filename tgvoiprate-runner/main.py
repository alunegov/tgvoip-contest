import os
import shutil
import subprocess
import sys
import time

def do_rate(ref_file_name, test_file_name, logger=None):
    s = subprocess.run(
        ['./tgvoiprate', ref_file_name, test_file_name],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        universal_newlines=True,
    )

    if logger:
        logger.write(s.stdout + '\n')
    else:
        print(s.stdout)

def do_run(logger=None):
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

if len(sys.argv) > 1:
    testEntrys = sys.argv[1] == 'entrys'
else:
    testEntrys = false

if testEntrys:
    for root, dirs, files in os.walk('entrys'):
        dirs.sort()

        if 'tgvoiprate' in files:
            entry_name = os.path.basename(root)
            print('Processing {}'.format(entry_name))

            shutil.copy2(os.path.join(root, 'tgvoiprate'), './')

            start = time.perf_counter_ns()
            
            with open(entry_name + '.txt', 'w') as f:
                do_run(f)
                f.write('elapsed in {} ms'.format((time.perf_counter_ns() - start) / 1000000))

            print('{} elapsed in {} ms'.format(entry_name, (time.perf_counter_ns() - start) / 1000000))
else:
    do_run()