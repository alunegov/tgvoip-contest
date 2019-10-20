#include <chrono>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include <VoIPController.h>
#include <VoIPServerConfig.h>

using namespace tgvoip;

struct Conf {
    static const int64_t EndpointId{2373083837059};
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

int main(int argc, const char* argv[]) {
    try {
        const auto conf = parseArgs(argc, argv);

        const auto serverConfig = readConfig(conf.ConfigFileName);
        ServerConfig::GetSharedInstance()->Update(serverConfig);

        auto c = std::make_unique<VoIPController>();

        const auto endpoints = createEndpoints(conf);
        c->SetRemoteEndpoints(endpoints, Conf::AllowP2p, Conf::ConnMaxLayer);

        VoIPController::Config controllerConfig;
        controllerConfig.dataSaving = DATA_SAVING_NEVER;
        controllerConfig.enableAEC = true;
        controllerConfig.enableNS = true;
        controllerConfig.enableAGC = true;
        controllerConfig.enableVolumeControl = true;
        controllerConfig.initTimeout = 7;
        controllerConfig.recvTimeout = 4;
        c->SetConfig(controllerConfig);

        c->SetEncryptionKey((char*)(conf.EncryptionKey.c_str()), conf.IsOutgoing);

        const auto input = [](int16_t* data, size_t len) {
            cout << "input return " << len << std::endl;
            data[0] = 13;
        };
        const auto output = [](int16_t* data, size_t len) {
            cout << "output got " << len << std::endl;
        };
        c->SetAudioDataCallbacks(input, output);

        VoIPController::Callbacks callbacks{};
        callbacks.connectionStateChanged = [](VoIPController* c_, int state) { cout  << "connectionStateChanged " << state << std::endl; };
        callbacks.signalBarCountChanged = [](VoIPController* c_, int barCount) { cout  << "signalBarCountChanged " << barCount << std::endl; };
        callbacks.groupCallKeySent = [](VoIPController* c_) { cout  << "groupCallKeySent" << std::endl; };
        callbacks.groupCallKeyReceived = [](VoIPController* c_, const unsigned char* callKey) { cout  << "groupCallKeyReceived " << callKey << std::endl; };
        callbacks.upgradeToGroupCallRequested = [](VoIPController* c_) { cout  << "upgradeToGroupCallRequested" << std::endl; };
        c->SetCallbacks(callbacks);

        const auto i = VoIPController::EnumerateAudioInputs();
        const auto o = VoIPController::EnumerateAudioOutputs();
        c->SetInputVolume(100);
        c->SetOutputVolume(100);
        c->SetCurrentAudioInput("default");
        c->SetCurrentAudioOutput("default");
        c->SetMicMute(false);

        c->SetNetworkType(Conf::NetworkType);
        c->SetNetworkType(NET_TYPE_OTHER_HIGH_SPEED);

        //cout << c->GetDebugString() << std::endl;

        c->Start();
        //cout << c->GetDebugString() << std::endl;
        c->Connect();
        //cout << c->GetDebugString() << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(10));
        cout << c->GetDebugString() << std::endl;

        //c->DebugCtl(3, 1);
        //std::this_thread::sleep_for(std::chrono::seconds(3));

        //c->DebugCtl(3, 0);
        //std::this_thread::sleep_for(std::chrono::seconds(3));

        c->Stop();
        //cout << c->GetDebugString() << std::endl;

        cout << c->GetDebugLog();

        c.reset(nullptr);
    } catch (std::exception& e) {
        cerr << e.what();
        return -1;
    }
}

std::pair<std::string, uint16_t> parseAddr(const std::string& addr);

Conf parseArgs(int argc, const char* argv[]) {
    Conf res;

    if (argc == 1) {
        res.AddrIp = "134.209.178.88";
        res.AddrPort = 556;
        res.Tag = "";
        res.EncryptionKey = "";
        res.ConfigFileName = "config.json";
        res.IsOutgoing = true;

        return res;
    }

    const auto p = parseAddr(argv[1]);
    res.AddrIp = p.first;
    res.AddrPort = p.second;
    cout << res.AddrIp << ":" << res.AddrPort << std::endl;

    res.Tag = argv[2];
    cout << res.Tag << std::endl;

    for (int i = 3; (i + 2) <= argc; i += 2) {
        const std::string key{argv[i]};
        if (key == "-k") {
            res.EncryptionKey = argv[i + 1];
            cout << res.EncryptionKey << std::endl;
        } else if (key == "-i") {
            res.InputFileName = argv[i + 1];
        } else if (key == "-o") {
            res.OutputFileName = argv[i + 1];
        } else if (key == "-c") {
            res.ConfigFileName = argv[i + 1];
            cout << res.ConfigFileName << std::endl;
        } else if (key == "-r") {
            res.IsOutgoing = std::string{argv[i + 1]} == "caller";
            cout << res.IsOutgoing << std::endl;
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

    const Endpoint endpoint{Conf::EndpointId, conf.AddrPort, IPv4Address{conf.AddrIp}, IPv6Address{},
        Conf::EndpointType, (unsigned char*)(conf.Tag.c_str())};
    res.emplace_back(endpoint);

    return res;
}
