#include <chrono>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include <VoIPController.h>
#include <VoIPServerConfig.h>

#include <opusfile-reader.h>
#include <opusfile-writer.h>
#ifdef TGVOIPCALL_WAVE_OUTPUT
#include <wavefile-writer.h>
#endif

using namespace tgvoip;

struct Conf {
    static const int64_t EndpointId{1};
    static const Endpoint::Type EndpointType{Endpoint::UDP_RELAY};
    static const bool AllowP2p{false};
    static const int32_t ConnMaxLayer{92};
    static const int NetworkType{NET_TYPE_ETHERNET};
    std::string AddrIp;
    uint16_t AddrPort{0};
    std::string EncryptionKey;
    bool IsOutgoing{true};
    std::string Tag;
    std::string InputFileName;
    std::string OutputFileName;
    std::string ConfigFileName;
};

Conf parseArgs(int argc, const char* argv[]);
std::string readConfig(const std::string& ConfigFileName);
std::vector<Endpoint> createEndpoints(const Conf& conf);
void decodeHex(const std::string& hex, unsigned char* dest, size_t destLen);

int main(int argc, const char* argv[]) {
    try {
        const auto conf = parseArgs(argc, argv);

        OpusFileReader r;
        OpusFileWriter w;
#ifdef TGVOIPCALL_WAVE_OUTPUT
        WaveFileWriter waveW;
#endif

        if (!r.Open(conf.InputFileName)) {
            std::cerr << "Error opening " << conf.InputFileName << std::endl;
            return -2;
        }
        if (!w.Create(conf.OutputFileName)) {
            std::cerr << "Error creating " << conf.OutputFileName << std::endl;
            return -3;
        }
#ifdef TGVOIPCALL_WAVE_OUTPUT
        const auto wavOutputFileName{conf.OutputFileName + ".wav"};
        if (!waveW.Create(wavOutputFileName)) {
            std::cerr << "Error creating " << wavOutputFileName << std::endl;
            return -4;
        }
#endif

        const auto serverConfig = readConfig(conf.ConfigFileName);
        ServerConfig::GetSharedInstance()->Update(serverConfig);

        auto c = std::make_unique<VoIPController>();

        const auto endpoints = createEndpoints(conf);
        c->SetRemoteEndpoints(endpoints, Conf::AllowP2p, Conf::ConnMaxLayer);

        VoIPController::Config controllerConfig;
        controllerConfig.initTimeout = 5;
        controllerConfig.recvTimeout = 4;
        controllerConfig.dataSaving = DATA_SAVING_NEVER;
        controllerConfig.enableAEC = !ServerConfig::GetSharedInstance()->GetBoolean("use_system_aec", true);
        controllerConfig.enableNS = !ServerConfig::GetSharedInstance()->GetBoolean("use_system_ns", true);
        controllerConfig.enableAGC = false;
        controllerConfig.enableCallUpgrade = false;
        controllerConfig.enableVolumeControl = false;
        c->SetConfig(controllerConfig);

        unsigned char encKey[256];
        decodeHex(conf.EncryptionKey, encKey, 256);
        c->SetEncryptionKey((char*)(encKey), conf.IsOutgoing);

        const auto input = [&](int16_t* data, size_t len) {
            //std::cout << "input produce " << len << std::endl;
            r.Read(data, len);
        };
        const auto output = [&](int16_t* data, size_t len) {
            //std::cout << "output got " << len << std::endl;
            // TODO: don't write all zero points in data (produced by read on other end in case of eof)?
            w.Write(data, len);
#ifdef TGVOIPCALL_WAVE_OUTPUT
            waveW.Write(data, len);
#endif
        };
        c->SetAudioDataCallbacks(input, output);

        c->Start();
        c->Connect();

        int res{0};

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(333));
            if (c->GetConnectionState() == STATE_FAILED) {
                std::cerr << "Connection failed (timeout)" << std::endl;
                std::cerr << c->GetDebugLog() << std::endl;
                res = -1;
                break;
            }
            if (r.Eof()) {
                std::this_thread::sleep_for(std::chrono::seconds(3));
                break;
            }
        }

        c->Stop();

        if (res == 0) {
            std::cout << c->GetDebugLog() << std::endl;
        }

        // will stop encoder and decoder (and AudioDataCallbacks)
        c.reset(nullptr);

        if (!w.Commit()) {
            std::cerr << "Error commiting " << conf.OutputFileName << std::endl;
            res = -5;
        }
#ifdef TGVOIPCALL_WAVE_OUTPUT
        if (!waveW.Commit()) {
            std::cerr << "Error commiting " << wavOutputFileName << std::endl;
            res = -5;
        }
#endif

        // TODO: call dtors of r, w, waveW to catch their errors?

        return res;
    } catch (const std::exception& e) {
        std::cerr << e.what();
        return -13;
    }
}

std::pair<std::string, uint16_t> parseAddr(const std::string& addr);

Conf parseArgs(int argc, const char* argv[]) {
    Conf res;

    if (argc == 1) {
        std::cout << "Usage: tgvoipcall reflector:port tag_caller_hex -k encryption_key_hex -i /path/to/sound_A.opus -o /path/to/sound_output_B.opus -c /path/to/config.json -r caller|callee" << std::endl;
        std::cout << "Will use defaults for now" << std::endl;

        res.AddrIp = "134.209.178.88";
        res.AddrPort = 553;
        res.Tag = "304b57e5dcc8298dec3f13089006a8bb";
        res.EncryptionKey = "399a4ec265f2025e8a5c6e39b2c257e4e3ab87d54fadb83c16637c2a714097ede18c4a3034654d7598d246e980bea26516aa92c336c5d5e436bbb18933442169428ee294a70c37992fb7e94b1312da93760a527127f21535eb32e990cf1ec7962285b5f2483ed0f5da332dcf1ffec1c212ea1bece0ba124efac2a336a48aae36f22d542b38b5f6965950244db0011f8a72bbffe98381d0dba549a52a9f9b609c14ee86a4fd12facb65fa986d0f4a3e99d130be7f6494d92adc3a8244654a8e7aa89e1817746def0f1652c31fd264722ea0daf536fdad6cd63061903a7cb3e93780bf3273988f3f470fdb412f9a71e249335e35754337ca7a58fe0030633c32ea";
        res.InputFileName = "in.ogg";
        res.OutputFileName = "out.ogg";
        res.ConfigFileName = "config.json";
        res.IsOutgoing = true;

        return res;
    }

    const auto p = parseAddr(argv[1]);
    res.AddrIp = p.first;
    res.AddrPort = p.second;
    //cout << res.AddrIp << ":" << res.AddrPort << std::endl;

    res.Tag = argv[2];
    //cout << res.Tag << std::endl;

    for (int i = 3; (i + 2) <= argc; i += 2) {
        const std::string key{argv[i]};
        if (key == "-k") {
            res.EncryptionKey = argv[i + 1];
            //cout << res.EncryptionKey << std::endl;
        } else if (key == "-i") {
            res.InputFileName = argv[i + 1];
        } else if (key == "-o") {
            res.OutputFileName = argv[i + 1];
        } else if (key == "-c") {
            res.ConfigFileName = argv[i + 1];
            //cout << res.ConfigFileName << std::endl;
        } else if (key == "-r") {
            res.IsOutgoing = std::string{argv[i + 1]} == "caller";
            //cout << res.IsOutgoing << std::endl;
        }
    }

    return res;
}

std::pair<std::string, uint16_t> parseAddr(const std::string& addr) {
    const auto p = addr.find(':');
    if (p == string::npos) {
        return std::make_pair(addr, 0);
    }
    const auto ip = addr.substr(0, p);
    const auto port = std::stoi(addr.substr(p + 1));
    return std::make_pair(ip, port);
}

std::string readConfig(const std::string& configFileName) {
    const std::ifstream config_file{configFileName};
    std::stringstream config;
    config << config_file.rdbuf();
    return config.str();
}

std::vector<Endpoint> createEndpoints(const Conf& conf) {
    std::vector<Endpoint> res;

    unsigned char peerTag[16];
    decodeHex(conf.Tag, peerTag, 16);
    const Endpoint endpoint{Conf::EndpointId, conf.AddrPort, IPv4Address{conf.AddrIp}, IPv6Address{},
        Conf::EndpointType, peerTag};
    res.emplace_back(endpoint);

    return res;
}

void decodeHex(const std::string& hex, unsigned char* dest, size_t destLen) {
    for (size_t i = 0; i < destLen; i++) {
        const auto c = hex.substr(2 * i, 2);
        const auto b = std::stoul(c, nullptr, 16);
        dest[i] = (unsigned char)b;
    }
}
