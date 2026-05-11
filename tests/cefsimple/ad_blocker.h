#ifndef AD_BLOCKER_H
#define AD_BLOCKER_H

#include <string>
#include <unordered_set>
#include <vector>

class AdBlocker {
public:
    AdBlocker();
    ~AdBlocker();
    
    void LoadFilters(const std::string& filepath);
    bool ShouldBlock(const std::string& url);
    static int GetBlockedCount();

private:
    std::unordered_set<std::string> exact_domains_;
    std::vector<std::string> patterns_;
    static int blocked_count_;
};

#endif // AD_BLOCKER_H
