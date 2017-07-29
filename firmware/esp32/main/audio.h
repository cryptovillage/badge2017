typedef enum {
	AUDIO_STATE_IDLE,
	AUDIO_STATE_YM2612,
	AUDIO_STATE_PLAYING_MP3_SDCARD,
	AUDIO_STATE_PLAYING_MP3_INTERNET
} audio_state_t;

typedef enum {
	AUDIO_REQUEST_STOP,
	AUDIO_REQUEST_PLAY_VGM_FILESYSTEM,
	AUDIO_REQUEST_PLAY_MP3_FILESYSTEM,
	AUDIO_REQUEST_PLAY_MP3_INTERNET
} audio_request_type_t;

typedef struct {
	audio_request_type_t type;
	char stream_name[250];
} audio_request_t;

#define AUDIO_REQUEST_QUEUE_LENGTH 1

extern void audio_init();
extern void audio_stop();
extern void audio_play_vgm(const char *filename);
extern void audio_play_mp3_file(const char *filename);
extern void audio_stream_mp3_url(const char *url);
