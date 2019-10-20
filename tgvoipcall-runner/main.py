import json
import subprocess
import sys
import urllib.request

if len(sys.argv) < 4:
    print('main.py auth_token call caller|callee')
    exit(1)

auth_token = sys.argv[1]
call = sys.argv[2]
direction = sys.argv[3]

url = "https://api.contest.com/voip{0}/getConnection?call={1}".format(auth_token, call)
contents = urllib.request.urlopen(url).read()
#print(contents)

answer = json.loads(contents)
print(answer)

if not answer['ok']:
    exit(1)

with open('config.json', 'w') as f:
    f.write(json.dumps(answer['result']['config']))

endpoint = answer['result']['endpoints'][0]

callerSubProcess = subprocess.run(
    ['./tgvoipcall',
        '{0}:{1}'.format(endpoint['ip'], endpoint['port']),
        endpoint['peer_tags'][direction],
        '-k', answer['result']['encryption_key'],
        '-c', 'config.json',
        '-r', direction,
    ],
    universal_newlines=True,
)
print(callerSubProcess)
