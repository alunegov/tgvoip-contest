import datetime
import json
import os
import subprocess
import sys
import urllib.request

def get_voip_api_answer(auth_token, call):
    url = "https://api.contest.com/voip{0}/getConnection?call={1}".format(auth_token, call)
    contents = urllib.request.urlopen(url).read()
    #print(contents)

    return json.loads(contents)

def start_process(api_answer, direction, file_name, out_dir):
    endpoint = api_answer['result']['endpoints'][0]

    return subprocess.Popen(
        ['./tgvoipcall',
            '{0}:{1}'.format(endpoint['ip'], endpoint['port']),
            endpoint['peer_tags'][direction],
            '-k', api_answer['result']['encryption_key'],
            '-i', file_name,
            '-o', os.path.join(out_dir, 'out_{0}_{1}'.format(direction, file_name)),
            '-c', os.path.join(out_dir, 'config.json'),
            '-r', direction,
        ],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        universal_newlines=True,
    )

def do_call(auth_token, call, caller_file_name, callee_file_name, out_dir, logger):
    api_answer = get_voip_api_answer(auth_token, call)
    logger.write(json.dumps(api_answer) + '\n')
    if not api_answer['ok']:
        logger.write('api not ok\n')
        return

    with open(os.path.join(out_dir, 'config.json'), 'w') as f:
        f.write(json.dumps(api_answer['result']['config']))

    callerSubProcess = start_process(api_answer, 'caller', caller_file_name, out_dir)
    calleeSubProcess = start_process(api_answer, 'callee', callee_file_name, out_dir)

    caller_outs, _ = callerSubProcess.communicate()
    callee_outs, _ = calleeSubProcess.communicate()

    logger.write(caller_outs + '\n')
    #logger.write(callerSubProcess.returncode)
    logger.write(callee_outs + '\n')
    #logger.write(calleeSubProcess.returncode)

if len(sys.argv) < 2:
    print('main.py auth_token [call]')
    exit(1)

auth_token = sys.argv[1]
if len(sys.argv) > 2:
    call = sys.argv[2]
else:
    call = 'random'

samples = [5, 15, 60]

while True:
    out_dir = datetime.datetime.now().strftime('%Y%m%d%H%M%S%f')
    os.mkdir(out_dir)
    with open(os.path.join(out_dir, 'call_log.txt'), 'w') as f:
        do_call(auth_token, call, 'caller.ogg', 'callee.ogg', out_dir, f)
