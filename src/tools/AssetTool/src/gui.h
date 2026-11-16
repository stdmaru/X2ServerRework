// The x2_assets window (decision T50, docs/tools/client-data.md#managing-client_data): client_data's files by folder or
// by archive with their state, an inspector, and "Sync data structure".
#pragma once

#include <filesystem>

namespace x2::assets {

// Opens the window on `clientData` (empty: the last one opened, or a folder picker). Returns the process exit code.
int RunWindow(const std::filesystem::path& clientData);

} // namespace x2::assets
