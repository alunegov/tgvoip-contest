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
    uint16_t AddrPort;
    std::string EncriptionKey;
    bool IsOutgoing;
    std::string Tag;
    std::string InputFileName;
    std::string OutputFileName;
    std::string ConfigFileName;
};


Conf parseConf(int argc, const char* argv[]) {
    Conf res;
    const auto addr = argv[1];
    const auto tag = argv[2];
    const auto encryption_key = argv[3];  // -k
    const auto input_file_name = argv[4];  // -i
    const auto output_file_name = argv[5];  // -o
    const auto config_file_name = argv[6];  // -c
    const auto dir = argv[7];  // -r

    const std::string addr_ip{""};
    const uint16_t addr_port{0};
    const bool dir_is_outgoing{false};
    return res;
}

int main(int argc, const char* argv[]) {
    try {
        const auto conf = parseConf(argc, argv);

        std::ifstream config_file{conf.ConfigFileName};
        std::stringstream config;
        config << config_file.rdbuf();

        ServerConfig::GetSharedInstance()->Update(config.str());

        auto c = std::make_unique<VoIPController>();

        const auto input = [](int16_t*, size_t) {};
        const auto output = [](int16_t*, size_t) {};
        c->SetAudioDataCallbacks(input, output);
        
        c->SetEncryptionKey((char*)(conf.EncriptionKey.c_str()), conf.IsOutgoing);
    
        std::vector<Endpoint> endpoints;

        const Endpoint endpoint{conf.EndpointId, conf.AddrPort, IPv4Address{conf.AddrIp}, IPv6Address{},
                Endpoint::UDP_RELAY, (unsigned char*)(conf.Tag.c_str())};
        endpoints.emplace_back(endpoint);

        c->SetRemoteEndpoints(endpoints, conf.AllowP2p, conf.ConnMaxLayer);

        c->Start();
        c->Connect();



        c->Stop();

        cout << c->GetDebugLog();

        c.release();
    } catch (std::exception& e) {
        cerr << e.what();
        return -1;
    }
}
