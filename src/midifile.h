/* MIDI status commands most significant bit is 1 */
#ifndef NOTE_ON
#define NOTE_OFF         	0x80
#define NOTE_ON          	0x90
#endif
#define POLY_AFTERTOUCH  	0xa0
#define CONTROL_CHANGE    	0xb0
#define PROGRAM_CHNG     	0xc0
#define CHANNEL_AFTERTOUCH      0xd0
#define PITCH_WHEEL      	0xe0
#define SYSTEM_EXCLUSIVE      	0xf0
#define DELAY_PACKET	 	(1111)

/* 7 bit controllers */
#define DAMPER_PEDAL            0x40
#define PORTAMENTO	        0x41 	
#define SOSTENUTO	        0x42
#define SOFT_PEDAL	        0x43
#define GENERAL_4               0x44
#define	HOLD_2		        0x45
#define	GENERAL_5	        0x50
#define	GENERAL_6	        0x51
#define GENERAL_7	        0x52
#define GENERAL_8	        0x53
#define TREMOLO_DEPTH	        0x5c
#define CHORUS_DEPTH	        0x5d
#define	DETUNE		        0x5e
#define PHASER_DEPTH	        0x5f

/* parameter values */
#define DATA_INC	        0x60
#define DATA_DEC	        0x61

/* parameter selection */
#define NON_REG_LSB	        0x62
#define NON_REG_MSB	        0x63
#define REG_LSB		        0x64
#define REG_MSB		        0x65

/* Standard MIDI Files meta event definitions */
#define	META_EVENT		0xFF
#define	SEQUENCE_NUMBER 	0x00
#define	TEXT_EVENT		0x01
#define COPYRIGHT_NOTICE 	0x02
#define SEQUENCE_NAME    	0x03
#define INSTRUMENT_NAME 	0x04
#define LYRIC	        	0x05
#define MARKER			0x06
#define	CUE_POINT		0x07
#define CHANNEL_PREFIX		0x20
#define	END_OF_TRACK		0x2f
#define	SET_TEMPO		0x51
#define	SMPTE_OFFSET		0x54
#define	TIME_SIGNATURE		0x58
#define	KEY_SIGNATURE		0x59
#define	SEQUENCER_SPECIFIC	0x74

/* Manufacturer's ID number */
#define Seq_Circuits (0x01) /* Sequential Circuits Inc. */
#define Big_Briar    (0x02) /* Big Briar Inc.           */
#define Octave       (0x03) /* Octave/Plateau           */
#define Moog         (0x04) /* Moog Music               */
#define Passport     (0x05) /* Passport Designs         */
#define Lexicon      (0x06) /* Lexicon 			*/
#define Tempi        (0x20) /* Bon Tempi                */
#define Siel         (0x21) /* S.I.E.L.                 */
#define Kawai        (0x41) 
#define Roland       (0x42)
#define Korg         (0x42)
#define Yamaha       (0x43)

/* miscellaneous definitions */
#define MThd 0x4d546864
#define MTrk 0x4d54726b
#define lowerbyte(x) ((unsigned char)(x & 0xff))
#define upperbyte(x) ((unsigned char)((x & 0xff00)>>8))
