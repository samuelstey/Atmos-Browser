#include "ad_blocker.h"
#include <fstream>
#include <algorithm>
#include <windows.h>

int AdBlocker::blocked_count_ = 0;

AdBlocker::AdBlocker() {}
AdBlocker::~AdBlocker() {}

void AdBlocker::LoadFilters(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        OutputDebugStringA(("AdBlocker: FAILED to open " + filepath + "\n").c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == '#') continue;

        if (line.find("||") == 0 || (line.find('/') != std::string::npos && line.find("||") != 0)) {
            patterns_.push_back(line);
        } else {
            exact_domains_.insert(line);
        }
    }
    
    OutputDebugStringA(("AdBlocker: Loaded " + std::to_string(exact_domains_.size()) + 
                       " domains and " + std::to_string(patterns_.size()) + " patterns\n").c_str());
}

std::string ExtractDomain(const std::string& url) {
    size_t pos = url.find("://");
    if (pos != std::string::npos) pos += 3; else pos = 0;
    size_t end_pos = url.find('/', pos);
    if (end_pos == std::string::npos) end_pos = url.length();
    size_t port_pos = url.find(':', pos);
    if (port_pos != std::string::npos && port_pos < end_pos) end_pos = port_pos;
    return url.substr(pos, end_pos - pos);
}

bool DomainMatches(const std::string& domain, const std::string& blocked) {
    if (domain == blocked) return true;
    if (domain.length() > blocked.length()) {
        size_t pos = domain.rfind(blocked);
        if (pos != std::string::npos && pos + blocked.length() == domain.length()) {
            if (pos > 0 && domain[pos - 1] == '.') return true;
        }
    }
    return false;
}

bool AdBlocker::ShouldBlock(const std::string& url) {
    std::string domain = ExtractDomain(url);
    
    for (const auto& blocked : exact_domains_) {
        if (DomainMatches(domain, blocked)) {
            blocked_count_++;
            OutputDebugStringA(("AdBlocker BLOCKED domain: " + domain + " | URL: " + url + "\n").c_str());
            return true;
        }
    }

    for (const auto& pattern : patterns_) {
        std::string clean = pattern;
        if (clean.find("||") == 0) clean = clean.substr(2);
        if (!clean.empty() && clean.back() == '^') clean.pop_back();

        if (clean.length() > 3 && url.find(clean) != std::string::npos) {
            blocked_count_++;
            OutputDebugStringA(("AdBlocker BLOCKED pattern: " + clean + " | URL: " + url + "\n").c_str());
            return true;
        }
    }

    return false;
}

int AdBlocker::GetBlockedCount() {
    return blocked_count_;
}
