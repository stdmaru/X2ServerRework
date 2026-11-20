#pragma once

// The original's KTDXPROFILE markers, as a step profiler (X2_DEBUG builds; X2_STEP_TRACE=<ms> turns it on).
// While CKTDXApp::FrameProcess runs a simulation step, every KTDXPROFILE()/KTDXPROFILE_BEGIN() scope it enters on the
// main thread is timed; a step slower than <ms> prints its call tree to stderr, the scopes over a tenth of the limit.
// Scopes outside a step (rendering, loader threads) are not recorded.

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace x2::ktdx::profile {

struct Node {
    const char* name;
    int depth;
    double startMs;
    double durationMs;  // < 0 while open
};

struct StepState {
    bool recording = false;
    int depth = 0;
    std::vector<Node> nodes;
    std::vector<std::size_t> open;  // indices of open nodes
};

inline double NowMs()
{
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

inline double ThresholdMs()
{
    static const double s_ms = [] {
        const char* value = std::getenv("X2_STEP_TRACE");
        return value ? std::atof(value) : 0.0;
    }();
    return s_ms;
}

inline StepState& State()
{
    thread_local StepState state;
    return state;
}

inline void Begin(const char* name)
{
    StepState& state = State();
    if (!state.recording || state.nodes.size() >= 50000) {
        return;
    }
    state.open.push_back(state.nodes.size());
    state.nodes.push_back({name, static_cast<int>(state.open.size()) - 1, NowMs(), -1.0});
}

inline void End()
{
    StepState& state = State();
    if (!state.recording || state.open.empty()) {
        return;
    }
    Node& node = state.nodes[state.open.back()];
    node.durationMs = NowMs() - node.startMs;
    state.open.pop_back();
}

struct Scope {
    explicit Scope(const char* name) { Begin(name); }
    ~Scope() { End(); }
    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;
};

/// Starts recording a step on this thread (no-op without X2_STEP_TRACE).
inline void StepBegin()
{
    StepState& state = State();
    state.recording = ThresholdMs() > 0.0;
    state.nodes.clear();
    state.open.clear();
}

/// Ends the step; when it took over the threshold, prints what it recorded.
inline void StepEnd(int step, double totalMs)
{
    StepState& state = State();
    if (!state.recording) {
        return;
    }
    state.recording = false;
    const double now = NowMs();
    while (!state.open.empty()) {  // a BEGIN without its END (an early return)
        Node& node = state.nodes[state.open.back()];
        node.durationMs = now - node.startMs;
        state.open.pop_back();
    }
    if (totalMs <= ThresholdMs()) {
        return;
    }
    const double minMs = ThresholdMs() / 10.0;
    std::fprintf(stderr, "step-profile steps=%d total=%.2f\n", step, totalMs);
    for (const Node& node : state.nodes) {
        if (node.durationMs >= minMs) {
            std::fprintf(stderr, "step-profile %*s%s %.2f\n", node.depth * 2, "", node.name, node.durationMs);
        }
    }
}

}  // namespace x2::ktdx::profile

#define KTDX_PROFILE_CONCAT2(a, b) a##b
#define KTDX_PROFILE_CONCAT(a, b) KTDX_PROFILE_CONCAT2(a, b)
#define KTDXPROFILE() ::x2::ktdx::profile::Scope KTDX_PROFILE_CONCAT(ktdxProfileScope, __LINE__)(__func__)
#define KTDXPROFILE2(name) ::x2::ktdx::profile::Scope KTDX_PROFILE_CONCAT(ktdxProfileScope, __LINE__)(name)
#define KTDXPROFILE_BEGIN(name) ::x2::ktdx::profile::Begin(name)
#define KTDXPROFILE_END() ::x2::ktdx::profile::End()
