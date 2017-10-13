/*
   Copyright (c) 2007 Tomas Pettersson <drpetter@gmail.com>

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

*/

#include "sfxr.h"

#include <dmsdk/sdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

float sfxr_frnd(float range)
{
	return (float)rnd(10000)/10000*range;
}

void SfxrResetParams()
{
	sfxr.wave_type=0;

	sfxr.p_base_freq=0.3f;
	sfxr.p_freq_limit=0.0f;
	sfxr.p_freq_ramp=0.0f;
	sfxr.p_freq_dramp=0.0f;
	sfxr.p_duty=0.0f;
	sfxr.p_duty_ramp=0.0f;

	sfxr.p_vib_strength=0.0f;
	sfxr.p_vib_speed=0.0f;
	sfxr.p_vib_delay=0.0f;

	sfxr.p_env_attack=0.0f;
	sfxr.p_env_sustain=0.3f;
	sfxr.p_env_decay=0.4f;
	sfxr.p_env_punch=0.0f;

	sfxr.filter_on=0;
	sfxr.p_lpf_resonance=0.0f;
	sfxr.p_lpf_freq=1.0f;
	sfxr.p_lpf_ramp=0.0f;
	sfxr.p_hpf_freq=0.0f;
	sfxr.p_hpf_ramp=0.0f;

	sfxr.p_pha_offset=0.0f;
	sfxr.p_pha_ramp=0.0f;

	sfxr.p_repeat_speed=0.0f;

	sfxr.p_arp_speed=0.0f;
	sfxr.p_arp_mod=0.0f;

	sfxr.master_vol=0.05f;
	sfxr.sound_vol=0.5f;

	sfxr.filesample=0.0f;

	sfxr.playing_sample=0;
	sfxr.samples_played=0;
}


void SfxrResetSample(int restart)
{
	if(!restart) {
		sfxr.phase=0;
	}
	sfxr.fperiod=100.0/(sfxr.p_base_freq*sfxr.p_base_freq+0.001);
	sfxr.period=(int)sfxr.fperiod;
	sfxr.fmaxperiod=100.0/(sfxr.p_freq_limit*sfxr.p_freq_limit+0.001);
	sfxr.fslide=1.0-pow((double)sfxr.p_freq_ramp, 3.0)*0.01;
	sfxr.fdslide=-pow((double)sfxr.p_freq_dramp, 3.0)*0.000001;
	sfxr.square_duty=0.5f-sfxr.p_duty*0.5f;
	sfxr.square_slide=-sfxr.p_duty_ramp*0.00005f;
	if(sfxr.p_arp_mod>=0.0f) {
		sfxr.arp_mod=1.0-pow((double)sfxr.p_arp_mod, 2.0)*0.9;
	}
	else {
		sfxr.arp_mod=1.0+pow((double)sfxr.p_arp_mod, 2.0)*10.0;
	}
	sfxr.arp_time=0;
	sfxr.arp_limit=(int)(pow(1.0f-sfxr.p_arp_speed, 2.0f)*20000+32);
	if(sfxr.p_arp_speed==1.0f) {
		sfxr.arp_limit=0;
	}
	if(!restart)
	{
		// reset filter
		sfxr.fltp=0.0f;
		sfxr.fltdp=0.0f;
		sfxr.fltw=pow(sfxr.p_lpf_freq, 3.0f)*0.1f;
		sfxr.fltw_d=1.0f+sfxr.p_lpf_ramp*0.0001f;
		sfxr.fltdmp=5.0f/(1.0f+pow(sfxr.p_lpf_resonance, 2.0f)*20.0f)*(0.01f+sfxr.fltw);
		if(sfxr.fltdmp>0.8f) sfxr.fltdmp=0.8f;
		sfxr.fltphp=0.0f;
		sfxr.flthp=pow(sfxr.p_hpf_freq, 2.0f)*0.1f;
		sfxr.flthp_d=1.0+sfxr.p_hpf_ramp*0.0003f;
		// reset vibrato
		sfxr.vib_phase=0.0f;
		sfxr.p_vib_speed=pow(sfxr.p_vib_speed, 2.0f)*0.01f;
		sfxr.vib_amp=sfxr.p_vib_strength*0.5f;
		// reset envelope
		sfxr.env_vol=0.0f;
		sfxr.env_stage=0;
		sfxr.env_time=0;
		sfxr.env_length[0]=(int)(sfxr.p_env_attack*sfxr.p_env_attack*100000.0f);
		sfxr.env_length[1]=(int)(sfxr.p_env_sustain*sfxr.p_env_sustain*100000.0f);
		sfxr.env_length[2]=(int)(sfxr.p_env_decay*sfxr.p_env_decay*100000.0f);

		sfxr.fphase=pow(sfxr.p_pha_offset, 2.0f)*1020.0f;
		if(sfxr.p_pha_offset<0.0f) sfxr.fphase=-sfxr.fphase;
		sfxr.fdphase=pow(sfxr.p_pha_ramp, 2.0f)*1.0f;
		if(sfxr.p_pha_ramp<0.0f) sfxr.fdphase=-sfxr.fdphase;
		sfxr.iphase=abs((int)sfxr.fphase);
		sfxr.ipp=0;
		for(int i=0;i<1024;i++) {
			sfxr.phaser_buffer[i]=0.0f;
		}

		for(int i=0;i<32;i++) {
			sfxr.noise_buffer[i]=sfxr_frnd(2.0f)-1.0f;
		}

		sfxr.rep_time=0;
		sfxr.rep_limit=(int)(pow(1.0f-sfxr.p_repeat_speed, 2.0f)*20000+32);
		if(sfxr.p_repeat_speed==0.0f) {
			sfxr.rep_limit=0;
		}
	}
}


void SfxrSynthSample(int length, unsigned char* buffer, FILE* file)
{
	for(int i=0;i<length;i++)
	{
		if(!sfxr.playing_sample) {
			break;
		}

		sfxr.rep_time++;
		if(sfxr.rep_limit!=0 && sfxr.rep_time>=sfxr.rep_limit)
		{
			sfxr.rep_time=0;
			SfxrResetSample(1);
		}

		// frequency envelopes/arpeggios
		sfxr.arp_time++;
		if(sfxr.arp_limit!=0 && sfxr.arp_time>=sfxr.arp_limit)
		{
			sfxr.arp_limit=0;
			sfxr.fperiod*=sfxr.arp_mod;
		}
		sfxr.fslide+=sfxr.fdslide;
		sfxr.fperiod*=sfxr.fslide;
		if(sfxr.fperiod>sfxr.fmaxperiod)
		{
			sfxr.fperiod=sfxr.fmaxperiod;
			if(sfxr.p_freq_limit>0.0f) {
				sfxr.playing_sample=0;
			}
		}
		float rfperiod=sfxr.fperiod;
		if(sfxr.vib_amp>0.0f)
		{
			sfxr.vib_phase+=sfxr.p_vib_speed;
			rfperiod=sfxr.fperiod*(1.0+sin(sfxr.vib_phase)*sfxr.vib_amp);
		}
		sfxr.period=(int)rfperiod;
		if(sfxr.period<8) sfxr.period=8;
		sfxr.square_duty+=sfxr.square_slide;
		if(sfxr.square_duty<0.0f) sfxr.square_duty=0.0f;
		if(sfxr.square_duty>0.5f) sfxr.square_duty=0.5f;
		// volume envelope
		sfxr.env_time++;
		if(sfxr.env_time>sfxr.env_length[sfxr.env_stage])
		{
			sfxr.env_time=0;
			sfxr.env_stage++;
			if(sfxr.env_stage==3) {
				sfxr.playing_sample=0;
			}
		}
		if(sfxr.env_stage==0) {
			sfxr.env_vol=(float)sfxr.env_time/sfxr.env_length[0];
		}
		if(sfxr.env_stage==1) {
			sfxr.env_vol=1.0f+pow(1.0f-(float)sfxr.env_time/sfxr.env_length[1], 1.0f)*2.0f*sfxr.p_env_punch;
		}
		if(sfxr.env_stage==2) {
			sfxr.env_vol=1.0f-(float)sfxr.env_time/sfxr.env_length[2];
		}

		// phaser step
		sfxr.fphase+=sfxr.fdphase;
		sfxr.iphase=abs((int)sfxr.fphase);
		if(sfxr.iphase>1023) sfxr.iphase=1023;

		if(sfxr.flthp_d!=0.0f)
		{
			sfxr.flthp*=sfxr.flthp_d;
			if(sfxr.flthp<0.00001f) sfxr.flthp=0.00001f;
			if(sfxr.flthp>0.1f) sfxr.flthp=0.1f;
		}

		float ssample=0.0f;
		for(int si=0;si<8;si++) // 8x supersampling
		{
			float sample=0.0f;
			sfxr.phase++;
			if(sfxr.phase>=sfxr.period)
			{
//				phase=0;
				sfxr.phase%=sfxr.period;
				if(sfxr.wave_type==3) {
					for(int i=0;i<32;i++) {
						sfxr.noise_buffer[i]=sfxr_frnd(2.0f)-1.0f;
					}
				}
			}
			// base waveform
			float fp=(float)sfxr.phase/sfxr.period;
			switch(sfxr.wave_type)
			{
			case 0: // square
				if(fp<sfxr.square_duty) {
					sample=0.5f;
				}
				else {
					sample=-0.5f;
				}
				break;
			case 1: // sawtooth
				sample=1.0f-fp*2;
				break;
			case 2: // sine
				sample=(float)sin(fp*2*PI);
				break;
			case 3: // noise
				sample=sfxr.noise_buffer[sfxr.phase*32/sfxr.period];
				break;
			}
			// lp filter
			float pp=sfxr.fltp;
			sfxr.fltw*=sfxr.fltw_d;
			if(sfxr.fltw<0.0f) sfxr.fltw=0.0f;
			if(sfxr.fltw>0.1f) sfxr.fltw=0.1f;
			if(sfxr.p_lpf_freq!=1.0f)
			{
				sfxr.fltdp+=(sample-sfxr.fltp)*sfxr.fltw;
				sfxr.fltdp-=sfxr.fltdp*sfxr.fltdmp;
			}
			else
			{
				sfxr.fltp=sample;
				sfxr.fltdp=0.0f;
			}
			sfxr.fltp+=sfxr.fltdp;
			// hp filter
			sfxr.fltphp+=sfxr.fltp-pp;
			sfxr.fltphp-=sfxr.fltphp*sfxr.flthp;
			sample=sfxr.fltphp;
			// phaser
			sfxr.phaser_buffer[sfxr.ipp&1023]=sample;
			sample+=sfxr.phaser_buffer[(sfxr.ipp-sfxr.iphase+1024)&1023];
			sfxr.ipp=(sfxr.ipp+1)&1023;
			// final accumulation and envelope application
			ssample+=sample*sfxr.env_vol;
		}
		ssample=ssample/8*sfxr.master_vol;

		ssample*=2.0f*sfxr.sound_vol;

		if(buffer != NULL)
		{
			// 44100Hz 16 bits
			ssample*=4.0f; // arbitrary gain to get reasonable output volume...
			if(ssample>1.0f) ssample=1.0f;
			if(ssample<-1.0f) ssample=-1.0f;

			sfxr.filesample+=ssample;
			unsigned short isample=(unsigned short)(sfxr.filesample*32000);
			unsigned char c1 = (unsigned char)((isample & 0xFF00) >> 8);
			unsigned char c2 = (unsigned char)(isample & 0x00FF);
			buffer[sfxr.buffer_byteswritten++] = c2;
			buffer[sfxr.buffer_byteswritten++] = c1;
			sfxr.filesample=0.0f;
		}
		if(file!=NULL)
		{
			ssample*=4.0f; // arbitrary gain to get reasonable output volume...
			if(ssample>1.0f) ssample=1.0f;
			if(ssample<-1.0f) ssample=-1.0f;
			sfxr.filesample+=ssample;
			short isample=(short)(sfxr.filesample*32000);
			fwrite(&isample, 1, 2, file);
			sfxr.filesample=0.0f;
			sfxr.file_bytesswritten+=2;
		}

		sfxr.samples_played++;
	}
}

void SfxrPlaySample(unsigned char* buffer, FILE* file) {
	SfxrResetSample(0);
	sfxr.samples_played=0;
	sfxr.playing_sample=1;
	while(sfxr.playing_sample) {
		SfxrSynthSample(256, buffer, file);
	}
}



int SfxrCalculateLength() {
	SfxrPlaySample(NULL, NULL);
	return sfxr.samples_played * 2;
}


struct SfxrSound SfxrToBuffer() {
	struct SfxrSound sound;
	sound.length = SfxrCalculateLength();
	sound.data = (unsigned char*) malloc(sound.length);

	sfxr.buffer_byteswritten=0;
	sfxr.file_bytesswritten=0;
	sfxr.filesample=0.0f;

	SfxrPlaySample(sound.data, NULL);
	return sound;
}


int SfxrToWAV(const char* filename)
{
	FILE* foutput=fopen(filename, "wb");
	if(!foutput) {
		return 0;
	}
	// write wav header
	unsigned int dword=0;
	unsigned short word=0;
	const int wav_bits=16;
	const int wav_freq=44100;

	fwrite("RIFF", 4, 1, foutput); // "RIFF"
	dword=0;
	fwrite(&dword, 1, 4, foutput); // remaining file size
	fwrite("WAVE", 4, 1, foutput); // "WAVE"

	fwrite("fmt ", 4, 1, foutput); // "fmt "
	dword=16;
	fwrite(&dword, 1, 4, foutput); // chunk size
	word=1;
	fwrite(&word, 1, 2, foutput); // compression code
	word=1;
	fwrite(&word, 1, 2, foutput); // channels
	dword=wav_freq;
	fwrite(&dword, 1, 4, foutput); // sample rate
	dword=wav_freq*wav_bits/8;
	fwrite(&dword, 1, 4, foutput); // bytes/sec
	word=wav_bits/8;
	fwrite(&word, 1, 2, foutput); // block align
	word=wav_bits;
	fwrite(&word, 1, 2, foutput); // bits per sample

	fwrite("data", 4, 1, foutput); // "data"
	dword=0;
	int foutstream_datasize=ftell(foutput);
	fwrite(&dword, 1, 4, foutput); // chunk size

	// write sample data
	sfxr.file_bytesswritten=0;
	sfxr.filesample=0.0f;
	SfxrPlaySample(NULL, foutput);

	// seek back to header and write size info
	fseek(foutput, 4, SEEK_SET);
	dword=0;
	dword=foutstream_datasize-4+sfxr.file_bytesswritten;
	fwrite(&dword, 1, 4, foutput); // remaining file size
	fseek(foutput, foutstream_datasize, SEEK_SET);
	dword=sfxr.file_bytesswritten;
	fwrite(&dword, 1, 4, foutput); // chunk size (data)
	fclose(foutput);

	return 1;
}


struct SfxrSound Pickup(int seed) {
	srand(seed);
	SfxrResetParams();

	sfxr.p_base_freq=0.4f+sfxr_frnd(0.5f);
	sfxr.p_env_attack=0.0f;
	sfxr.p_env_sustain=sfxr_frnd(0.1f);
	sfxr.p_env_decay=0.1f+sfxr_frnd(0.4f);
	sfxr.p_env_punch=0.3f+sfxr_frnd(0.3f);
	if(rnd(1))
	{
		sfxr.p_arp_speed=0.5f+sfxr_frnd(0.2f);
		sfxr.p_arp_mod=0.2f+sfxr_frnd(0.4f);
	}
	return SfxrToBuffer();
}

struct SfxrSound Laser(int seed) {
	srand(seed);
	SfxrResetParams();

	sfxr.wave_type=rnd(2);
	if(sfxr.wave_type==2 && rnd(1)) {
		sfxr.wave_type=rnd(1);
	}
	sfxr.p_base_freq=0.5f+sfxr_frnd(0.5f);
	sfxr.p_freq_limit=sfxr.p_base_freq-0.2f-sfxr_frnd(0.6f);
	if(sfxr.p_freq_limit<0.2f) sfxr.p_freq_limit=0.2f;
	sfxr.p_freq_ramp=-0.15f-sfxr_frnd(0.2f);
	if(rnd(2)==0)
	{
		sfxr.p_base_freq=0.3f+sfxr_frnd(0.6f);
		sfxr.p_freq_limit=sfxr_frnd(0.1f);
		sfxr.p_freq_ramp=-0.35f-sfxr_frnd(0.3f);
	}
	if(rnd(1))
	{
		sfxr.p_duty=sfxr_frnd(0.5f);
		sfxr.p_duty_ramp=sfxr_frnd(0.2f);
	}
	else
	{
		sfxr.p_duty=0.4f+sfxr_frnd(0.5f);
		sfxr.p_duty_ramp=-sfxr_frnd(0.7f);
	}
	sfxr.p_env_attack=0.0f;
	sfxr.p_env_sustain=0.1f+sfxr_frnd(0.2f);
	sfxr.p_env_decay=sfxr_frnd(0.4f);
	if(rnd(1)) {
		sfxr.p_env_punch=sfxr_frnd(0.3f);
	}
	if(rnd(2)==0)
	{
		sfxr.p_pha_offset=sfxr_frnd(0.2f);
		sfxr.p_pha_ramp=-sfxr_frnd(0.2f);
	}
	if(rnd(1)) {
		sfxr.p_hpf_freq=sfxr_frnd(0.3f);
	}
	return SfxrToBuffer();
}



struct SfxrSound Explosion(int seed) {
	srand(seed);
	SfxrResetParams();

	sfxr.wave_type=3;
	if(rnd(1))
	{
		sfxr.p_base_freq=0.1f+sfxr_frnd(0.4f);
		sfxr.p_freq_ramp=-0.1f+sfxr_frnd(0.4f);
	}
	else
	{
		sfxr.p_base_freq=0.2f+sfxr_frnd(0.7f);
		sfxr.p_freq_ramp=-0.2f-sfxr_frnd(0.2f);
	}
	sfxr.p_base_freq*=sfxr.p_base_freq;
	if(rnd(4)==0) {
		sfxr.p_freq_ramp=0.0f;
	}
	if(rnd(2)==0) {
		sfxr.p_repeat_speed=0.3f+sfxr_frnd(0.5f);
	}
	sfxr.p_env_attack=0.0f;
	sfxr.p_env_sustain=0.1f+sfxr_frnd(0.3f);
	sfxr.p_env_decay=sfxr_frnd(0.5f);
	if(rnd(1)==0)
	{
		sfxr.p_pha_offset=-0.3f+sfxr_frnd(0.9f);
		sfxr.p_pha_ramp=-sfxr_frnd(0.3f);
	}
	sfxr.p_env_punch=0.2f+sfxr_frnd(0.6f);
	if(rnd(1))
	{
		sfxr.p_vib_strength=sfxr_frnd(0.7f);
		sfxr.p_vib_speed=sfxr_frnd(0.6f);
	}
	if(rnd(2)==0)
	{
		sfxr.p_arp_speed=0.6f+sfxr_frnd(0.3f);
		sfxr.p_arp_mod=0.8f-sfxr_frnd(1.6f);
	}
	return SfxrToBuffer();
}


struct SfxrSound Powerup(int seed) {
	srand(seed);
	SfxrResetParams();

	if(rnd(1)) {
		sfxr.wave_type=1;
	}
	else {
		sfxr.p_duty=sfxr_frnd(0.6f);
	}
	if(rnd(1))
	{
		sfxr.p_base_freq=0.2f+sfxr_frnd(0.3f);
		sfxr.p_freq_ramp=0.1f+sfxr_frnd(0.4f);
		sfxr.p_repeat_speed=0.4f+sfxr_frnd(0.4f);
	}
	else
	{
		sfxr.p_base_freq=0.2f+sfxr_frnd(0.3f);
		sfxr.p_freq_ramp=0.05f+sfxr_frnd(0.2f);
		if(rnd(1))
		{
			sfxr.p_vib_strength=sfxr_frnd(0.7f);
			sfxr.p_vib_speed=sfxr_frnd(0.6f);
		}
	}
	sfxr.p_env_attack=0.0f;
	sfxr.p_env_sustain=sfxr_frnd(0.4f);
	sfxr.p_env_decay=0.1f+sfxr_frnd(0.4f);
	return SfxrToBuffer();
}


struct SfxrSound Hurt(int seed) {
	srand(seed);
	SfxrResetParams();

	sfxr.wave_type=rnd(2);
	if(sfxr.wave_type==2) {
		sfxr.wave_type=3;
	}
	if(sfxr.wave_type==0) {
		sfxr.p_duty=sfxr_frnd(0.6f);
	}
	sfxr.p_base_freq=0.2f+sfxr_frnd(0.6f);
	sfxr.p_freq_ramp=-0.3f-sfxr_frnd(0.4f);
	sfxr.p_env_attack=0.0f;
	sfxr.p_env_sustain=sfxr_frnd(0.1f);
	sfxr.p_env_decay=0.1f+sfxr_frnd(0.2f);
	if(rnd(1)) {
		sfxr.p_hpf_freq=sfxr_frnd(0.3f);
	}
	return SfxrToBuffer();
}


struct SfxrSound Jump(int seed) {
	srand(seed);
	SfxrResetParams();

	sfxr.wave_type=0;
	sfxr.p_duty=sfxr_frnd(0.6f);
	sfxr.p_base_freq=0.3f+sfxr_frnd(0.3f);
	sfxr.p_freq_ramp=0.1f+sfxr_frnd(0.2f);
	sfxr.p_env_attack=0.0f;
	sfxr.p_env_sustain=0.1f+sfxr_frnd(0.3f);
	sfxr.p_env_decay=0.1f+sfxr_frnd(0.2f);
	if(rnd(1)) {
		sfxr.p_hpf_freq=sfxr_frnd(0.3f);
	}
	if(rnd(1)) {
		sfxr.p_lpf_freq=1.0f-sfxr_frnd(0.6f);
	}
	return SfxrToBuffer();
}


struct SfxrSound Blip(int seed) {
	srand(seed);
	SfxrResetParams();

	sfxr.wave_type=rnd(1);
	if(sfxr.wave_type==0) {
		sfxr.p_duty=sfxr_frnd(0.6f);
	}
	sfxr.p_base_freq=0.2f+sfxr_frnd(0.4f);
	sfxr.p_env_attack=0.0f;
	sfxr.p_env_sustain=0.1f+sfxr_frnd(0.1f);
	sfxr.p_env_decay=sfxr_frnd(0.2f);
	sfxr.p_hpf_freq=0.1f;
	return SfxrToBuffer();
}

/*int main(int argc, char *argv[]) {
	Laser(100);
}*/
