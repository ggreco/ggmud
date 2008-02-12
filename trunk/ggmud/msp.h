#ifndef MSP_H
#define MSP_H

typedef struct msp_control
{
    // statistics
    long sounds_played;
    long musics_played;

    // sound related
    char actual_sound[256];
    int sound_volume;
    int sound_priority;
    int sound_repeats;
    int sound_continue;

    // music related
    char actual_music[256];
    int music_volume;
    int music_priority;
    int music_repeats;
    int music_continue;

    // base urls
    char default_sound_url[256];
    char default_music_url[256];
} msp_control;

extern msp_control *init_msp();

#endif
