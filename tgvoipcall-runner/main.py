import datetime
import json
import os
import random
import subprocess
import sys
import urllib.request

def get_voip_api_answer(auth_token, call):
    url = "https://api.contest.com/voip{0}/getConnection?call={1}".format(auth_token, call)
    contents = urllib.request.urlopen(url).read()
    #print(contents)

    return json.loads(contents)

def start_process(api_answer, direction, infile_name, outfile_name, out_dir):
    endpoint = api_answer['result']['endpoints'][0]

    return subprocess.Popen(
        ['./tgvoipcall',
            '{0}:{1}'.format(endpoint['ip'], endpoint['port']),
            endpoint['peer_tags'][direction],
            '-k', api_answer['result']['encryption_key'],
            '-i', infile_name,
            '-o', os.path.join(out_dir, 'out_{0}_{1}'.format(direction, os.path.basename(outfile_name))),
            '-c', os.path.join(out_dir, 'config.json'),
            '-r', direction,
        ],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )

def do_call(auth_token, call, caller_file_name, callee_file_name, out_dir, logger):
    api_answer = get_voip_api_answer(auth_token, call)
    logger.write('api_answer:\n')
    logger.write(json.dumps(api_answer) + '\n')

    if not api_answer['ok']:
        logger.write('api not ok - exiting\n')
        return

    with open(os.path.join(out_dir, 'config.json'), 'w') as f:
        f.write(json.dumps(api_answer['result']['config']))

    callerSubProcess = start_process(api_answer, 'caller', caller_file_name, callee_file_name, out_dir)
    calleeSubProcess = start_process(api_answer, 'callee', callee_file_name, caller_file_name, out_dir)

    caller_outs, caller_errs = callerSubProcess.communicate()
    callee_outs, callee_errs = calleeSubProcess.communicate()

    logger.write('caller output:\n')
    logger.write(caller_outs + '\n')
    logger.write('caller error:\n')
    logger.write(caller_errs + '\n')
    logger.write('caller return code = ' + str(callerSubProcess.returncode) + '\n\n')
    logger.write('callee output:\n')
    logger.write(callee_outs + '\n')
    logger.write('callee error:\n')
    logger.write(callee_errs + '\n')
    logger.write('callee return code = ' + str(calleeSubProcess.returncode) + '\n\n')

if len(sys.argv) < 3:
    print('main.py auth_token repetitions [call]')
    exit(-1)

random.seed()

auth_token = sys.argv[1]
repetitions = int(sys.argv[2])
if len(sys.argv) > 3:
    call = sys.argv[3]
else:
    call = random.randint(1, 1000000)

samples_5 = [
	'samples/sample05_0bb3646f15e8dc61f525f40f2884de57.ogg',
	'samples/sample05_7f3d7554d1fe70872389e84ebe802984.ogg',
	'samples/sample05_14ae7b1886265e54e7f2c83d67eb802e.ogg',
	'samples/sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg',
	'samples/sample05_93fd2fb8e32e04fff51eaa1677a471c8.ogg',
	'samples/sample05_44823b5704b026f2930ad862576bef3c.ogg',
	'samples/sample05_b4e08e2fae45c5991b82b80755d042a5.ogg',
	'samples/sample05_e181863bce6738bace6841b174713716.ogg',
	'samples/sample05_f8498e0018ea93b1158ec6fec09b23e5.ogg',
	'samples/sample05_ff63f34c691af48ef285649054ab4906.ogg',
	'samples/sample06_43af06b41db225c41a659da60408148a.ogg',
	'samples/sample06_6691afc81fd72df69da5b1a7a508b30e.ogg',
	'samples/sample06_08332cdbd86d4f09d30cd81c4f436081.ogg',
	'samples/sample06_afb5f1ecdd37621d1be77b20691fefd8.ogg',
	'samples/sample06_b2f157ef91eaef1e2778a9b37326e3ef.ogg',
	'samples/sample06_b05e9d0ca9fa03bc46191299c1bae645.ogg',
	'samples/sample06_fb64e39c9934c818b378a0532c38f50f.ogg',
	'samples/sample07_5574802a9f1816ded504abaccbd6ea79.ogg',
]
samples_15 = [
	'samples/sample14_7e30ddf39168a4ea0579f35d3baac0d9.ogg',
	'samples/sample14_1610bcfe3d4a5409ca90463ea8c0ef8f.ogg',
	'samples/sample14_9406179a57e6d882cba5a5c23c7e7e4f.ogg',
	'samples/sample14_9688780a39194d29f54f79bb9a6a9910.ogg',
	'samples/sample15_1a7df29173d06cd4119ea338d1e8e05c.ogg',
	'samples/sample15_03c54b861f72cce82609dd3acbaa85fb.ogg',
	'samples/sample15_4a30a6c03e108b963d0afe692558e3ec.ogg',
	'samples/sample15_8082d542b11fb2be2869f8f45b292373.ogg',
	'samples/sample15_64900b3ffd4aa70f5e5d9641952094e8.ogg',
	'samples/sample15_75836e80be4f3370e27e3f17bbce3433.ogg',
	'samples/sample15_a5c5e22bcb1d4585beba504d73b6cc99.ogg',
	'samples/sample15_c38cd5c611532af5e79ed0958c415880.ogg',
	'samples/sample15_d4cdbff1b70c60a2fd8fc54f26f55c23.ogg',
	'samples/sample16_2ee9c6bcc64a0b6566f8e9ec99b20ada.ogg',
	'samples/sample16_5d5ba5774b0b215e83f8099e87cbe7e2.ogg',
	'samples/sample16_28b7af27b8464f83f547e385893de318.ogg',
	'samples/sample16_450f0cecc3c7003c6fbc3a10f4712aa4.ogg',
	'samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg',
	'samples/sample17_350b04a3821a66a8bcd78a15588c8191.ogg',
	'samples/sample17_fd738975ea9a518e48680e3c33ee4c05.ogg',
]
samples_60 = [
	'samples/sample60_0a0e77fef9c11ec30dfdf1461d4eefe2.ogg',
	'samples/sample60_4c9973a21cbfa19ba71506093e90f3a7.ogg',
	'samples/sample60_93c07c0d1b3843e80ee44dce188bf2d6.ogg',
	'samples/sample60_790f3769e5288ad46c0688791d559bca.ogg',
	'samples/sample60_6067be4f4008b500aff2b3b682e7131b.ogg',
	'samples/sample60_8929a3305be7905248b12700091361c5.ogg',
	'samples/sample60_35939af851861d564d63bd374ffd1560.ogg',
	'samples/sample60_a9ec34d1a70c593ca1f0036dcf0da7ca.ogg',
	'samples/sample60_bf6bb62a74e210c44b6a256417a8193a.ogg',
	'samples/sample60_c70f8ed50f21ebc03588c87900db520d.ogg',
	'samples/sample61_69ba168f9b52cba70e1ce8046c0285f1.ogg',
	'samples/sample61_d4d716ffffc14ac5aea11d6ee1eccb2b.ogg',
	'samples/sample62_747b02d1c0aebd7eebe02b9c0b7a71ba.ogg',
	'samples/sample62_f875782337e56d53cd42c59d7cb044fc.ogg',
	'samples/sample63_8299254189168351907467c8b2ae7d33.ogg',
]

for i in range(repetitions):
    out_dir = datetime.datetime.now().strftime('%Y%m%d%H%M%S%f')
    os.mkdir(out_dir)

    duration = random.choice([5, 15, 60])
    if duration == 5:
        samples_ = samples_5
    elif duration == 15:
        samples_ = samples_15
    elif duration == 60:
        samples_ = samples_60
    else:
        print('unsupp duration {0}'.format(duration))
        exit(-2)

    callerSound = random.choice(samples_)
    calleeSound = random.choice(samples_)

    print('{0} dir={1} caller={2} callee={3}'.format(i, out_dir, callerSound, calleeSound))

    with open(os.path.join(out_dir, 'call_log.txt'), 'w') as f:
        do_call(auth_token, call, callerSound, calleeSound, out_dir, f)
