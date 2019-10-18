#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>

#include <VoIPController.h>
#include <VoIPServerConfig.h>

using namespace tgvoip;

struct Conf {
    static const int64_t EndpointId{0};
    static const bool AllowP2p{false};
    static const int32_t ConnMaxLayer{0};
    std::string AddrIp;
    uint16_t AddrPort{0};
    std::string EncriptionKey;
    bool IsOutgoing{false};
    std::string Tag;
    std::string InputFileName;
    std::string OutputFileName;
    std::string ConfigFileName;
};

Conf parseArgs(int argc, const char* argv[]);
std::string readConfig(std::string ConfigFileName);
//std::vector<Endpoint> createEndpoints(const Conf& conf);

int main(int argc, const char* argv[]) {
    try {
        const auto conf = parseArgs(argc, argv);

        const auto config = readConfig(conf.ConfigFileName);
        cout << config << std::endl;
        /*ServerConfig::GetSharedInstance()->Update(config);

        auto c = std::make_unique<VoIPController>();

        const auto input = [](int16_t*, size_t) {};
        const auto output = [](int16_t*, size_t) {};
        c->SetAudioDataCallbacks(input, output);
        
        c->SetEncryptionKey((char*)(conf.EncriptionKey.c_str()), conf.IsOutgoing);
    
        const auto endpoints = createEndpoints(conf);
        c->SetRemoteEndpoints(endpoints, conf.AllowP2p, conf.ConnMaxLayer);

        c->Start();
        c->Connect();



        c->Stop();

        cout << c->GetDebugLog();

        c.release();*/
    } catch (std::exception& e) {
        cerr << e.what();
        return -1;
    }
}

std::pair<std::string, uint16_t> parseAddr(std::string addr);

Conf parseArgs(int argc, const char* argv[]) {
    Conf res;

    const auto p = parseAddr(argv[1]);
    res.AddrIp = p.first;
    res.AddrPort = p.second;

    res.Tag = argv[2];

    for (int i = 3; (i + 2) <= argc; i += 2) {
        const std::string key{argv[i]};
        if (key == "-k") {
            res.EncriptionKey = argv[i + 1];
        } else if (key == "-i") {
            res.InputFileName = argv[i + 1];
        } else if (key == "-o") {
            res.OutputFileName = argv[i + 1];
        } else if (key == "-c") {
            res.ConfigFileName = argv[i + 1];
        } else if (key == "-r") {
            res.IsOutgoing = std::string{argv[i + 1]} == "caller";
        }
    }

    return res;
}

std::pair<std::string, uint16_t> parseAddr(std::string addr) {
    const auto p = addr.find(':');
    if (p == string::npos) {
        throw std::exception{"no port specified"};
    }
    const auto ip = addr.substr(0, p);
    const auto port = std::stoi(addr.substr(p + 1));
    return std::make_pair(ip, port);
}

std::string readConfig(std::string configFileName) {
    const std::ifstream config_file{configFileName};
    std::stringstream config;
    config << config_file.rdbuf();
    return config.str();
}

/*std::vector<Endpoint> createEndpoints(const Conf& conf) {
    std::vector<Endpoint> res;

    const Endpoint endpoint{conf.EndpointId, conf.AddrPort, IPv4Address{conf.AddrIp}, IPv6Address{},
            Endpoint::UDP_RELAY, (unsigned char*)(conf.Tag.c_str())};
    res.emplace_back(endpoint);

    return res;
}*/
