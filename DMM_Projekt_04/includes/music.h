#ifndef MUSIC_H_
#define MUSIC_H_

typedef struct  
{
	uint16_t tone;
	uint16_t duration;
} MUSIC_Note;

#define MUSIC_END { 0, 0 }
#define MUSIC_IS_END(_note_)  (!((_note_).tone) && !((_note_).duration))

#define MUSIC_A4 440
#define MUSIC_B4 494
#define MUSIC_C4 262
#define MUSIC_D4 293
#define MUSIC_E4 330
#define MUSIC_F4 349
#define MUSIC_G4 392

#define MUSIC_A5 880
#define MUSIC_B5 988
#define MUSIC_C5 523
#define MUSIC_D5 587
#define MUSIC_E5 659
#define MUSIC_F5 698
#define MUSIC_G5 784

#define MUSIC_1 1000
#define MUSIC_2 (MUSIC_1/2)
#define MUSIC_4 (MUSIC_1/4)
#define MUSIC_8 (MUSIC_1/8)
#define MUSIC_16 (MUSIC_1/16)
#define MUSIC_32 (MUSIC_1/32)

	
typedef const MUSIC_Note* MUSIC_Track;
extern const MUSIC_Note MUSIC_Tetris[];

void Music_PlayTrack (MUSIC_Track track);


#endif /* MUSIC_H_ */