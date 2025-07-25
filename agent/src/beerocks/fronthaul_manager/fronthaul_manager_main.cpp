/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_manager/ap_manager.h"
#include "monitor/monitor.h"

#include <bcl/beerocks_cmdu_client_factory_factory.h>
#include <bcl/beerocks_event_loop_impl.h>
#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_os_utils.h>
#include <bcl/beerocks_timer_factory_impl.h>
#include <bcl/beerocks_timer_manager_impl.h>
#include <bcl/beerocks_version.h>
#include <mapf/common/utils.h>

#include <easylogging++.h>

// Do not use this macro anywhere else in ire process
// It should only be there in one place in each executable module
BEEROCKS_INIT_BEEROCKS_VERSION

static bool g_running = true;
static int s_signal   = 0;
static std::string fronthaul_iface;

// Pointer to logger instance
static std::shared_ptr<beerocks::logging> g_logger_ap_mananger;
static std::shared_ptr<beerocks::logging> g_logger_monitor;

static void handle_signal()
{
    if (!s_signal)
        return;

    switch (s_signal) {

    // Terminate
    case SIGTERM:
    case SIGINT:
        LOG(INFO) << "Caught signal '" << strsignal(s_signal) << "' Exiting...";
        g_running = false;
        break;

    // Roll log file
    case SIGUSR1: {
        LOG(INFO) << "LOG Roll Signal!";
        auto roll_file = [&](std::shared_ptr<beerocks::logging> &logger) {
            if (!logger) {
                LOG(ERROR) << "Invalid logger pointer!";
                return;
            }

            logger->apply_settings();
        };

        LOG(INFO) << "--- Start of file after roll ---";
        roll_file(g_logger_ap_mananger);
        roll_file(g_logger_monitor);
        break;
    }
    default:
        LOG(WARNING) << "Unhandled Signal: '" << strsignal(s_signal) << "' Ignoring...";
        break;
    }

    s_signal = 0;
}

static void init_signals()
{
    // Signal handler function
    auto signal_handler = [](int signum) { s_signal = signum; };

    struct sigaction sigterm_action;
    sigterm_action.sa_handler = signal_handler;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_flags = 0;
    sigaction(SIGTERM, &sigterm_action, NULL);

    struct sigaction sigint_action;
    sigint_action.sa_handler = signal_handler;
    sigemptyset(&sigint_action.sa_mask);
    sigint_action.sa_flags = 0;
    sigaction(SIGINT, &sigint_action, NULL);

    struct sigaction sigusr1_action;
    sigusr1_action.sa_handler = signal_handler;
    sigemptyset(&sigusr1_action.sa_mask);
    sigusr1_action.sa_flags = 0;
    sigaction(SIGUSR1, &sigusr1_action, NULL);
}

static bool parse_arguments(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "i:")) != -1) {
        switch (opt) {
        case 'i': {
            fronthaul_iface.assign(optarg);
            break;
        }
        case '?': {
            return false;
        }
        }
    }
    return true;
}

/**
 * @brief Removes the residue files from previous fronthaul process instance.
 * 
 * @param path Path to where the fronthaul residual file are located.
 * @param file_name Name of the file to be removed if exist.
 */
static void remove_residual_fronthaul_files(const std::string &path, const std::string &file_name)
{
    int pid_out = -1;
    // If the PID not provided by is-pid-running check.
    if (!beerocks::os_utils::read_pid_file(path, file_name, pid_out)) {
        // If the file doesn't exist or failed to read PID from it - do nothing.
        return;
    }

    // There is no error print to prevent false error prints in case of first boot.
    if (pid_out == -1) {
        return;
    }

    for (int index = 1; index <= beerocks::eBeeRocksIfaceIds::IFACE_TOTAL_VAPS; ++index) {
        // Removes a residual wpa_ctrl files of an old PIDs if exists.
        std::stringstream ss;
        ss << "wpa_ctrl_" << pid_out << "-" << index;
        beerocks::os_utils::remove_residual_files(std::string("/tmp/"), ss.str());
    }
}

/**
 * @brief Create and initialize logging object and print the version.
 * 
 * @param file_name File name of the log file.
 * @param log_config Log file configuration.
 * @param argc Argument counter of the process.
 * @param argv Arguments values of the process.
 * @param logger_id The id of the logger (optional).
 * @return std::shared_ptr<beerocks::logging> shared_ptr to logging object on success, otherwise
 * nullptr.
 */
static std::shared_ptr<beerocks::logging>
init_logger(const std::string &file_name, const beerocks::config_file::SConfigLog &log_config,
            int argc, char **argv, const std::string &logger_id = std::string())
{
    auto logger = std::make_shared<beerocks::logging>(file_name, log_config, logger_id);
    if (!logger) {
        std::cout << "Failed to allocated logger to " << file_name;
        return nullptr;
    }
    logger->apply_settings();
    CLOG(INFO, logger->get_logger_id())
        << std::endl
        << "Running " << file_name << " Version " << BEEROCKS_VERSION << " Build date "
        << BEEROCKS_BUILD_DATE << std::endl
        << std::endl;
    beerocks::version::log_version(argc, argv, logger->get_logger_id());

    // Redirect stdout / stderr to file
    if (logger->get_log_files_enabled()) {
        beerocks::os_utils::redirect_console_std(log_config.files_path + file_name + "_std.log");
    }

    return logger;
}

int main(int argc, char *argv[])
{
    std::cout << "Beerocks Fronthaul Process Start" << std::endl;

    init_signals();

#ifndef USE_PRPLMESH_WHM
    // Close all inhereted files descriptors.
    int openFds = sysconf(_SC_OPEN_MAX);
    for (int fd = 0; fd < openFds; fd++) {
        // Don't close stdin/stderr/stdout
        if ((fd != STDOUT_FILENO) && (fd != STDIN_FILENO) && (fd != STDERR_FILENO)) {
            close(fd);
        }
    }
#endif

    // Check for version query first, handle and exit if requested.
    std::string module_description;
    if (beerocks::version::handle_version_query(argc, argv, module_description)) {
        return 0;
    }

    //get command line options
    if (!parse_arguments(argc, argv)) {
        std::cout << "Usage: " << argv[0] << " -i <fronthaul iface>" << std::endl;
        return 1;
    }

    // read slave config file
    std::string slave_config_file_path =
        CONF_FILES_WRITABLE_PATH + std::string(BEEROCKS_AGENT) +
        ".conf"; //search first in platform-specific default directory
    beerocks::config_file::sConfigSlave beerocks_slave_conf;
    if (!beerocks::config_file::read_slave_config_file(slave_config_file_path,
                                                       beerocks_slave_conf)) {
        slave_config_file_path = mapf::utils::get_install_path() + "config/" +
                                 std::string(BEEROCKS_AGENT) +
                                 ".conf"; // if not found, search in beerocks path
        if (!beerocks::config_file::read_slave_config_file(slave_config_file_path,
                                                           beerocks_slave_conf)) {
            std::cout << "config file '" << slave_config_file_path << "' args error." << std::endl;
            return 1;
        }
    }

    // Init logger ap_manager
    std::string base_ap_manager_name = std::string(BEEROCKS_AP_MANAGER) + "_" + fronthaul_iface;
    g_logger_ap_mananger = init_logger(base_ap_manager_name, beerocks_slave_conf.sLog, argc, argv);
    if (!g_logger_ap_mananger) {
        return 1;
    }

    // Init logger monitor
    std::string base_monitor_name = std::string(BEEROCKS_MONITOR) + "_" + fronthaul_iface;
    g_logger_monitor =
        init_logger(base_monitor_name, beerocks_slave_conf.sLog, argc, argv, BEEROCKS_MONITOR);
    if (!g_logger_monitor) {
        return 1;
    }

    // Kill running fronthaul and write pid file
    std::string base_fronthaul_name = std::string(BEEROCKS_FRONTHAUL) + "_" + fronthaul_iface;
    beerocks::os_utils::kill_pid(beerocks_slave_conf.temp_path + "pid/", base_fronthaul_name);

    // Remove any residual fronthaul files not cleared by previous instance
    remove_residual_fronthaul_files(beerocks_slave_conf.temp_path, base_fronthaul_name);

    beerocks::os_utils::write_pid_file(beerocks_slave_conf.temp_path, base_fronthaul_name);
    std::string pid_file_path =
        beerocks_slave_conf.temp_path + "pid/" + base_fronthaul_name; // For file touching

    // Create application event loop to wait for blocking I/O operations.
    auto event_loop = std::make_shared<beerocks::EventLoopImpl>();
    LOG_IF(!event_loop, FATAL) << "Unable to create event loop!";

    // Create timer factory to create instances of timers.
    auto timer_factory = std::make_shared<beerocks::TimerFactoryImpl>();
    LOG_IF(!timer_factory, FATAL) << "Unable to create timer factory!";

    // Create timer manager to help using application timers.
    auto timer_manager = std::make_shared<beerocks::TimerManagerImpl>(timer_factory, event_loop);
    LOG_IF(!timer_manager, FATAL) << "Unable to create timer manager!";

    // Get Agent UDS file
    std::string agent_uds_path = beerocks_slave_conf.temp_path + std::string(BEEROCKS_AGENT_UDS);

    // Create CMDU client factory to create CMDU clients connected to CMDU server running in
    // slave when requested
    std::shared_ptr<beerocks::CmduClientFactory> slave_cmdu_client_factory =
        beerocks::create_cmdu_client_factory(agent_uds_path, event_loop);
    LOG_IF(!slave_cmdu_client_factory, FATAL) << "Unable to create CMDU client factory!";

    // Create ap_manager
    son::ApManager ap_manager(fronthaul_iface, *g_logger_ap_mananger, slave_cmdu_client_factory,
                              timer_manager, event_loop);

    if (!ap_manager.start()) {
        // if is a fatal error except for when the user explicitly terminate the process
        if (s_signal == SIGTERM)
            return 0;
        LOG(FATAL) << "Unable to start AP manager!";
    }

    // Create Monitor
    son::Monitor monitor(fronthaul_iface, beerocks_slave_conf, *g_logger_monitor);

    bool monitor_is_running = false;

    auto touch_time_stamp_timeout = std::chrono::steady_clock::now();
    while (g_running) {

        // Handle signals
        if (s_signal) {
            handle_signal();
            continue;
        }

        if (std::chrono::steady_clock::now() > touch_time_stamp_timeout) {
            beerocks::os_utils::touch_pid_file(pid_file_path);
            touch_time_stamp_timeout = std::chrono::steady_clock::now() +
                                       std::chrono::seconds(beerocks::TOUCH_PID_TIMEOUT_SECONDS);
        }

        // Run application event loop and break on error.
        if (event_loop->run() < 0) {
            LOG(ERROR) << "Event loop failure!";
            break;
        }

        // After the ap_manager finishes the attach process, start the monitor. There is no
        // point in starting it before.
        // If the fronthaul is defined as ZWDFS, do not start the monitor since a ZWDFS interface
        // shall only be used for ZWDFS purpose, and shall not monitor anything by definition.
        if (ap_manager.is_operational()) {
            if (monitor_is_running || ap_manager.zwdfs_ap()) {
                if (!monitor.is_running()) {
                    LOG(INFO) << "Restarting monitor thread";
                    monitor_is_running = false;
                }
                continue;
            } else if (monitor.start()) {
                monitor_is_running = true;
            } else {
                CLOG(ERROR, g_logger_monitor->get_logger_id()) << "Unable to start monitor!";
                break;
            }
        }
    }

    if (monitor_is_running) {
        monitor.stop();
    }

    ap_manager.stop();

    return 0;
}
