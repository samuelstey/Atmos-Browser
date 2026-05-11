// Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/cefclient/browser/client_browser.h"

#include "include/base/cef_logging.h"
#include "include/cef_command_line.h"
#include "include/cef_crash_util.h"
#include "include/cef_file_util.h"
#include "include/cef_path_util.h"
#include "tests/cefclient/browser/client_prefs.h"
#include "tests/cefclient/browser/default_client_handler.h"
#include "tests/cefclient/browser/main_context.h"
#include "tests/cefclient/browser/root_window_manager.h"
#include "tests/shared/common/client_switches.h"

namespace client::browser {

namespace {

class ClientBrowserDelegate : public ClientAppBrowser::Delegate {
public:
  ClientBrowserDelegate() = default;

  ClientBrowserDelegate(const ClientBrowserDelegate &) = delete;
  ClientBrowserDelegate &operator=(const ClientBrowserDelegate &) = delete;

  void OnRegisterCustomPreferences(
      CefRefPtr<client::ClientAppBrowser> app, cef_preferences_type_t type,
      CefRawPtr<CefPreferenceRegistrar> registrar) override {
    if (type == CEF_PREFERENCES_TYPE_GLOBAL) {
      prefs::RegisterGlobalPreferences(registrar);
    }
  }

  void OnContextInitialized(CefRefPtr<ClientAppBrowser> app) override {
    if (CefCrashReportingEnabled()) {
      CefSetCrashKeyValue("testkey_small1", "value1_small_browser");
      CefSetCrashKeyValue("testkey_small2", "value2_small_browser");
      CefSetCrashKeyValue("testkey_medium1", "value1_medium_browser");
      CefSetCrashKeyValue("testkey_medium2", "value2_medium_browser");
      CefSetCrashKeyValue("testkey_large1", "value1_large_browser");
      CefSetCrashKeyValue("testkey_large2", "value2_large_browser");
    }

    const std::string &crl_sets_path =
        CefCommandLine::GetGlobalCommandLine()->GetSwitchValue(
            switches::kCRLSetsPath);
    if (!crl_sets_path.empty()) {
      CefLoadCRLSetsFile(crl_sets_path);
    }
  }

  void OnBeforeCommandLineProcessing(
      CefRefPtr<ClientAppBrowser> app,
      CefRefPtr<CefCommandLine> command_line) override {

    // Ativa a interface moderna do Chrome
    command_line->AppendSwitch("enable-chrome-runtime");

    CefString dir_path;
    // PK_DIR_EXE pega o caminho da pasta onde o executável está
    if (CefGetPath(PK_DIR_EXE, dir_path)) {
      // Monta o caminho final: pasta_do_exe + \ublock
      std::string extension_path = dir_path.ToString() + "\\ublock";
      command_line->AppendSwitchWithValue("load-extension", extension_path);
    }

    if (client::MainContext::Get()->TouchEventsEnabled()) {
      command_line->AppendSwitchWithValue("touch-events", "enabled");
    }
  }

  bool
  OnAlreadyRunningAppRelaunch(CefRefPtr<ClientAppBrowser> app,
                              CefRefPtr<CefCommandLine> command_line,
                              const CefString &current_directory) override {
    static const char *kIgnoredSwitches[] = {
        switches::kMultiThreadedMessageLoop,
        switches::kOffScreenRenderingEnabled,
        switches::kUseViews,
    };
    for (auto &kIgnoredSwitche : kIgnoredSwitches) {
      if (command_line->HasSwitch(kIgnoredSwitche)) {
        LOG(WARNING) << "The --" << kIgnoredSwitche
                     << " command-line switch is ignored on app relaunch.";
      }
    }

    auto config = std::make_unique<RootWindowConfig>(command_line->Copy());
    MainContext::Get()->GetRootWindowManager()->CreateRootWindow(
        std::move(config));

    return true;
  }

  CefRefPtr<CefClient>
  GetDefaultClient(CefRefPtr<ClientAppBrowser> app) override {
    LOG(INFO) << "Creating a chrome browser with the default client";
    return new DefaultClientHandler();
  }

private:
  IMPLEMENT_REFCOUNTING(ClientBrowserDelegate);
};

} // namespace

void CreateDelegates(ClientAppBrowser::DelegateSet &delegates) {
  delegates.insert(new ClientBrowserDelegate);
}

} // namespace client::browser