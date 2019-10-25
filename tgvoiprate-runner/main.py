import os
import subprocess

def do_rate(ref_file_name, test_file_name):
    s = subprocess.run(
        ['./tgvoiprate', ref_file_name, test_file_name],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        universal_newlines=True,
    )

    print(s.stdout)

for root, dirs, files in os.walk('../tgvoipcall-runner'):
    if 'samples' in dirs:
        dirs.remove('samples')
    dirs.sort()    
    if 'config.json' in files:
        for file in files:
            if os.path.splitext(file)[1] == '.ogg':
                test_file_name = os.path.join(root, file)
                ref_file_name = os.path.join('../tgvoipcall-runner/samples/', file[11:])
                print('Processing {0} ref={1} test={2}'.format(root, ref_file_name, test_file_name))
                do_rate(ref_file_name, test_file_name)
