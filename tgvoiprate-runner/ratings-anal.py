import csv
import io
import os
import statistics

RatingsFileName = 'voip_contest1_ratings.csv'

# entrys 992, 998, 1000, 1001 got empty rates
NoValue = -3.0

SamplesPath = 'samples'
DistortedPath = 'distorted'

class TestCase(object):
    def __init__(self, original, call_entry, net):
        self.original = original
        self.call_entry = call_entry
        self.net = net
    
    def distorted(self):
        return 'entry{0}_{1}_{2}.dur.ogg'.format(self.call_entry, self.original, self.processed_net())

    def processed_net(self):
        if self.net == 'default':
            return ''
        return self.net.replace(' + ', ',')

class IndexedDelta(object):
    def __init__(self, index, correct, test):
        self.index = index
        self.correct = correct
        self.test = test
        self.delta = abs(correct - test)

def parse_ratings(filename):
    with open(filename, mode='rt', newline='') as csvfile:
        reader = csv.DictReader(csvfile, delimiter='\t')
        
        entry_names = list(filter(lambda it: it not in ['original', 'call_entry', 'net'], reader.fieldnames))

        test_cases = []
        entry_rates = dict()
        for entry_name in entry_names:
            entry_rates[entry_name] = []

        for row in reader:
            test_cases.append(TestCase(row['original'], row['call_entry'], row['net']))

            for entry_name in entry_names:
                try:
                    rate = float(row[entry_name])
                except ValueError:
                    #print('ValueError at line {0}, column {1}, value "{2}". Use {3} instead'.format(reader.line_num, entry_name, row[entry_name], NoValue))
                    rate = NoValue
                
                entry_rates[entry_name].append(rate)
    
    return (entry_names, test_cases, entry_rates)

def calc_rating(correct, test):
    sum = 0.0
    for x, y in zip(correct, test):
        sum = sum + pow(max(abs(x - y) - 0.3, 0.0), 2)
    return sum

def print_ratings(entry_names, entry_rates):
    entry_ratings = []
    for entry_name in entry_names:
        rating = calc_rating(entry_rates['correct_mean'], entry_rates[entry_name])
        entry_ratings.append((entry_name, rating))
    entry_ratings_byrating = sorted(entry_ratings, key=lambda it: it[1])
    for it in entry_ratings_byrating:
        print('rating {0:.2f} of entry {1}'.format(it[1], it[0]))

def print_stats(entry_names, entry_rates):
    entry_stats = []
    for entry_name in entry_names:
        ideltas = gen_ideltas(entry_rates['correct_mean'], entry_rates[entry_name])
        deltas = to_deltas(ideltas)
        entry_stats.append((entry_name, statistics.mean(deltas), statistics.variance(deltas)))
    
    entry_stats_bymean = sorted(entry_stats, key=lambda it: it[1])
    entry_stats_byvar = sorted(entry_stats, key=lambda it: it[2])
    
    print('entry_stats_bymean')
    for it in entry_stats_bymean:
        print('mean {1:.4f}, var {2:.4f} of {0}'.format(it[0], it[1], it[2]))
    print('entry_stats_byvar')
    for it in entry_stats_byvar:
        print('mean {1:.4f}, var {2:.4f} of {0}'.format(it[0], it[1], it[2]))

def gen_ideltas(correct, test):
    res = []
    for i, (x, y) in enumerate(zip(correct, test)):
        res.append(IndexedDelta(i, x, y))
    return res

def print_ideltas(ideltas, test_cases):
    for idelta in ideltas:
        print('delta {0:.4f} (test {1:.4f}, correct {2:.4f}) at #{3:3d} {4}'.format(idelta.delta, idelta.test, idelta.correct, idelta.index + 1, test_cases[idelta.index].distorted()))

def print_run(test_cases):
    for test_case in test_cases:
        original_filename = os.path.join(SamplesPath, test_case.original)
        distorted_filename = os.path.join(DistortedPath, test_case.distorted())
        print('./tgvoiprate {0} {1}'.format(original_filename, distorted_filename))

def to_deltas(ideltas):
    res = []
    for idelta in ideltas:
        res.append(idelta.delta)
    return res

def do_print():
    entry_names, test_cases, entry_rates = parse_ratings(RatingsFileName)

    print_ratings(entry_names, entry_rates)
    #print(calc_rating(entry_rates['correct_mean'], entry_rates['997']))
    #print(calc_rating(entry_rates['correct_mean'], entry_rates['993']))

    ideltas = gen_ideltas(entry_rates['correct_mean'], entry_rates['993'])
    sorted_ideltas = sorted(ideltas, key=lambda it: it.delta)
    print_ideltas(sorted_ideltas, test_cases)

    #print_stats(entry_names, entry_rates)

    #print_run(test_cases)

def do_run_alldistorted():
    import runner

    _, test_cases, entry_rates = parse_ratings(RatingsFileName)

    new_rates = []
    for test_case in test_cases:
        original_filename = os.path.join(SamplesPath, test_case.original)
        distorted_filename = os.path.join(DistortedPath, test_case.distorted())
        with io.StringIO() as s:
            runner.do_rate(original_filename, distorted_filename, s)
            new_rates.append(float(s.getvalue()))

    print(calc_rating(entry_rates['correct_mean'], new_rates))

if __name__ == '__main__':
    do_print()
    #do_run_alldistorted()
