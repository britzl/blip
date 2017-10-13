#ifndef SFXR_H
#define SFXR_H

#define rnd(n) (rand()%(n+1))

#define PI 3.14159265f

struct SfxrSound {
	unsigned char* data;
	int length;
};

struct Sfxr {
	int wave_type;

	float p_base_freq;
	float p_freq_limit;
	float p_freq_ramp;
	float p_freq_dramp;
	float p_duty;
	float p_duty_ramp;

	float p_vib_strength;
	float p_vib_speed;
	float p_vib_delay;

	float p_env_attack;
	float p_env_sustain;
	float p_env_decay;
	float p_env_punch;

	int filter_on;
	float p_lpf_resonance;
	float p_lpf_freq;
	float p_lpf_ramp;
	float p_hpf_freq;
	float p_hpf_ramp;

	float p_pha_offset;
	float p_pha_ramp;

	float p_repeat_speed;

	float p_arp_speed;
	float p_arp_mod;

	float master_vol;
	float sound_vol;

	float vib_phase;
	float vib_speed;
	float vib_amp;

	int env_stage;
	int env_time;
	int env_length[3];
	float env_vol;

	int iphase;
	float phaser_buffer[1024];
	int ipp;
	float noise_buffer[32];

	float square_duty;
	float square_slide;

	int rep_time;
	int rep_limit;
	int arp_time;
	int arp_limit;
	double arp_mod;

	double fslide;
	double fdslide;

	float fphase;
	float fdphase;

	float fltp;
	float fltdp;
	float fltdmp;
	float fltphp;
	float flthp;
	float flthp_d;
	float fltw;
	float fltw_d;

	int period;
	int phase;

	double fperiod;
	double fmaxperiod;

	int playing_sample;
	int samples_played;

	int buffer_byteswritten;
	int file_bytesswritten;
	float filesample;
};

static Sfxr sfxr;

struct SfxrSound Blip(int seed);
struct SfxrSound Jump(int seed);
struct SfxrSound Hurt(int seed);
struct SfxrSound Powerup(int seed);
struct SfxrSound Explosion(int seed);
struct SfxrSound Laser(int seed);
struct SfxrSound Pickup(int seed);

#endif
