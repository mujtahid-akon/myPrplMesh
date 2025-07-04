/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef AMBIORIX_IMPL
#define AMBIORIX_IMPL

// prplmesh
#include <bcl/beerocks_event_loop.h>
#include <easylogging++.h>

#include "ambiorix.h"
#include "ambiorix_runtime.h"

#include <amxb/amxb_register.h>
#include <amxd/amxd_action.h>
#include <amxd/amxd_object.h>
#include <amxd/amxd_object_event.h>
#include <amxd/amxd_transaction.h>

namespace beerocks {
namespace nbapi {

using actions_callback = amxd_status_t (*)(amxd_object_t *object, amxd_param_t *param,
                                           amxd_action_t reason, const amxc_var_t *const args,
                                           amxc_var_t *const retval, void *priv);

using events_callback = void (*)(const char *const sig_name, const amxc_var_t *const data,
                                 void *const priv);

using ambiorix_func_ptr = amxd_status_t (*)(amxd_object_t *object, amxd_function_t *func,
                                            amxc_var_t *args, amxc_var_t *ret);

typedef struct sActionsCallback {
    std::string action_name;
    actions_callback callback;
} sActionsCallback;

typedef struct sEvents {
    std::string name;
    events_callback callback;
} sEvents;

typedef struct sFunctions {
    std::string name;
    std::string path;
    ambiorix_func_ptr callback;
} sFunctions;

/**
 * @class AmbiorixImpl
 * @brief This class manages the ambiorixImpl instance.
 */
class AmbiorixImpl : public Ambiorix {

public:
    explicit AmbiorixImpl(std::shared_ptr<EventLoop> event_loop,
                          const std::vector<sActionsCallback> &on_action,
                          const std::vector<sEvents> &events,
                          const std::vector<sFunctions> &funcs_list);

    /**
     * @brief AmbiorixImpl destructor removes: bus connection, data model, parser and all data
     *        from the backend (UBus, PCB, etc.).
     */
    ~AmbiorixImpl() override;

    /**
     * @brief Initialize the ambiorix library: load all available backends, connect to all 
     *        available bus, load data model, register data model in the bus.
     *        
     * @param datamodel_path Path to the data model definition ODL file.
     * @return True on success and false otherwise.
     */
    bool init(const std::string &datamodel_path);

    bool set(const std::string &relative_path, const std::string &parameter,
             const std::string &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const int8_t &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const int16_t &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const int32_t &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const int64_t &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const uint8_t &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const uint16_t &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const uint32_t &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const uint64_t &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const bool &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const double &value) override;
    bool set(const std::string &relative_path, const std::string &parameter,
             const sMacAddr &value) override;

    bool read_param(const std::string &obj_path, const std::string &param_name,
                    int8_t *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    int16_t *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    int32_t *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    int64_t *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    uint8_t *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    uint16_t *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    uint32_t *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    uint64_t *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    double *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    bool *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    std::string *param_val) override;
    bool read_param(const std::string &obj_path, const std::string &param_name,
                    sMacAddr *param_val) override;

    std::string add_instance(const std::string &relative_path) override;

    bool remove_instance(const std::string &relative_path, uint32_t index) override;

    uint32_t get_instance_index(const std::string &specific_path, const std::string &key) override;

    std::string get_datamodel_time_format() override;

    /**
     * @brief Set current data and time in RFC 3339 format.
     *
     * @param path_to_object Path to NBAPI object which has parameter object.
     * @param param parameter name which is default as TimeStamp.
     * @return True if date and time successfully set, false otherwise.
     */
    bool set_current_time(const std::string &path_to_object,
                          const std::string &param = "TimeStamp") override;

    /**
     * @brief Set field 'TimeStamp'  in RFC 3339 format.
     *
     * @param path_to_object Path to NBAPI object which has parameter object 'TimeStamp'
     * @param param time_stamp specific time_stamp.
     * @return True if date and time successfully set, false otherwise.
     */
    bool set_time(const std::string &path_to_object, const std::string &time_stamp) override;

    bool remove_all_instances(const std::string &relative_path) override;

    bool add_optional_subobject(const std::string &path_to_obj,
                                const std::string &subobject_name) override;

    bool remove_optional_subobject(const std::string &path_to_obj,
                                   const std::string &subobject_name) override;

    /**
     * @brief Reads and return from Data Model value of uint64 parameter for given object.
     *
     * @param[in] obj_path Path to object.
     * @param[in] param_name Name of the parameter.
     * @param[out] param_val Value of parameter.
     * @return True on success, false otherwise.
    */

    /**
     * @brief Load and parse data model from the ODL file.
     *
     * @param datamodel_path Path to the data model definition ODL file.
     * @return True on success and false otherwise.
     */
    bool load_datamodel(const std::string &datamodel_path);

private:
    // Methods

    /**
     * @brief Prepare transaction to the ubus
     *
     * @param relative_path Path to the object in datamodel (ex: "Device.WiFi.DataElements.Network.ID").
     * @param transaction Variable for transaction structure which contains fields
     *                    needed for transaction.
     * @return Pointer on the object on success and nullptr otherwise.
     */
    amxd_object_t *prepare_transaction(const std::string &relative_path, amxd_trans_t &transaction);

    /**
     * @brief Apply transaction
     *
     * @param transaction Variable for transaction structure which contains fields
     *                    needed for transaction.
     * @return True on success and false otherwise.
     */
    bool apply_transaction(amxd_trans_t &transaction);

    /**
     * @brief Initialize event handlers for Ambiorix fd in the event loop.
     *
     * @return True on success and false otherwise.
     */
    bool init_event_loop();

    /**
     * @brief Initialize event handlers for the ambiorix signals fd in the event loop.
     *
     * @return True on success and false otherwise.
     */
    bool init_signal_loop();

    /**
     * @brief Remove event handlers for Ambiorix fd from the event loop.
     *
     * @return True on success and false otherwise.
     */
    bool remove_event_loop();

    /**
     * @brief Remove event handlers for the ambiorix signals fd from the event loop.
     *
     * @return True on success and false otherwise.
     */
    bool remove_signal_loop();

    /**
     * @brief Connect to the bus and register datamodel.
     *
     * @return True on success and false otherwise.
     */
    bool connect_and_register();

    /**
     * @brief Find object by relative path.
     *
     * @param relative_path Path to the object in datamodel (ex: "Device.WiFi.DataElements.Network.ID").
     * @return Pointer on the object on success and nullptr otherwise.
     */
    amxd_object_t *find_object(const std::string &relative_path);

    // Variables
    std::vector<amxb_bus_ctx_t *> m_bus_ctx_vect;
    std::shared_ptr<EventLoop> m_event_loop;
    //std::unordered_map<std::string, actions_callback> m_on_action_handlers;
    std::vector<sActionsCallback> m_on_action_handlers;
    std::vector<sEvents> m_events_list;
    std::vector<sFunctions> m_func_list;
};

} // namespace nbapi
} // namespace beerocks

#endif
