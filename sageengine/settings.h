#ifndef INCLUDED_SETTINGS
#define INCLUDED_SETTINGS

#include <string>

class Settings
{
public:
    Settings();
    virtual ~Settings() {}

    bool Load(const std::string& filename);
    bool Save(const std::string& filename);

    bool FullscreenGraphics() const     { return fullscreen_graphics_; }
    bool SoundEnabled() const           { return sound_enabled_; }
    bool MusicEnabled() const           { return music_enabled_; }
    unsigned long SoundVolume() const   { return sound_volume_; }
    unsigned long MusicVolume() const   { return music_volume_; }

    void FullscreenGraphics(bool fullscreen_graphics)   { fullscreen_graphics_ = fullscreen_graphics; OnFullscreenGraphicsSettingChanged(); }
    void SoundEnabled(bool sound_enabled)               { sound_enabled_ = sound_enabled; OnSoundEnabledSettingChanged(); }
    void MusicEnabled(bool music_enabled)               { music_enabled_ = music_enabled; OnMusicEnabledSettingChanged(); }
    void SoundVolume(unsigned long sound_volume)        { sound_volume_ = sound_volume; OnSoundVolumeSettingChanged(); }
    void MusicVolume(unsigned long music_volume)        { music_volume_ = music_volume; OnMusicVolumeSettingChanged(); }

protected:
    virtual void OnFullscreenGraphicsSettingChanged() = 0;
    virtual void OnSoundEnabledSettingChanged() = 0;
    virtual void OnMusicEnabledSettingChanged() = 0;
    virtual void OnSoundVolumeSettingChanged() = 0;
    virtual void OnMusicVolumeSettingChanged() = 0;

private:
    bool fullscreen_graphics_;
    bool sound_enabled_;
    bool music_enabled_;
    unsigned long sound_volume_;
    unsigned long music_volume_;
};

#endif  // INCLUDED_SETTINGS
