#include <storm/log/FileLogger.hpp>
#include <storm/log/LogHandler.hpp>

#include "App.hpp"
#include "Log.hpp"

#include <storm/main/Main.hpp>

int main(const int argc, const char **argv) {
    using namespace storm;

    // STORMKIT_EXPECTS(std::filesystem::exists("./stormkit/ModelLoading"));

    // log::LogHandler::setupLogger<log::FileLogger>("log.txt");
    log::LogHandler::setupDefaultLogger();

    try {
        auto app = App {};
        app.run(argc, argv);
    } catch (std::runtime_error &err) {
        flog("Unhandled exception ({})", err.what());
    } catch (...) { flog("Unhandled exception ({})"); }

    return EXIT_SUCCESS;
}
