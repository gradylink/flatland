#include "soundRegistry.hpp"
#include <stdexcept>

namespace flatland {

SoundId SoundRegistry::registerSound(const std::string &name, std::vector<uint8_t> oggBytes) {
  SoundId id = static_cast<SoundId>(entries.size());
  entries.push_back({name, std::move(oggBytes)});
  return id;
}

std::string SoundRegistry::soundEventName(SoundId sound) const {
  if (sound < 0 || static_cast<size_t>(sound) >= entries.size()) throw std::out_of_range("unknown SoundId");
  return "flatland:sound/" + entries[static_cast<size_t>(sound)].name;
}

void SoundRegistry::addResourcePackFiles(ZipWriter &zip) const {
  if (entries.empty()) return;

  std::string soundsJson = "{";
  for (size_t i = 0; i < entries.size(); i++) {
    const Entry &entry = entries[i];
    if (i > 0) soundsJson += ",";
    soundsJson += "\"sound/" + entry.name + "\":{\"sounds\":[\"flatland:sound/" + entry.name + "\"]}";

    std::string oggContents(entry.oggBytes.begin(), entry.oggBytes.end());
    zip.addFile("assets/flatland/sounds/sound/" + entry.name + ".ogg", oggContents);
  }
  soundsJson += "}";

  zip.addFile("assets/flatland/sounds.json", soundsJson);
}

} // namespace flatland
