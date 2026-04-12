/*
 * file: midump.c
 *     -- parse and display the contents of a MIDI file
 *
 * Copyright (c) 2025 by Kent Crispin and Songbird Software.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "midifile.h"

static FILE *fp;
static long  file_pos = 0;

static const char *note_names[12] = {
    "C", "C#", "D", "D#", "E", "F",
    "F#", "G", "G#", "A", "A#", "B"
};

/* indexed by sharps/flats + 7  (range 0..14, i.e. Cb..C#) */
static const char *major_keys[15] = {
    "Cb", "Gb", "Db", "Ab", "Eb", "Bb", "F",
    "C", "G", "D", "A", "E", "B", "F#", "C#"
};
static const char *minor_keys[15] = {
    "Ab", "Eb", "Bb", "F", "C", "G", "D",
    "A", "E", "B", "F#", "C#", "G#", "D#", "A#"
};

static int read_byte(void)
{
    int c = fgetc(fp);
    if (c == EOF) {
        fprintf(stderr, "midump: unexpected end of file\n");
        exit(1);
    }
    file_pos++;
    return c;
}

static unsigned short read_short(void)
{
    unsigned short v;
    v  = (unsigned short)read_byte() << 8;
    v |= (unsigned short)read_byte();
    return v;
}

static unsigned long read_long(void)
{
    unsigned long v;
    v  = (unsigned long)read_byte() << 24;
    v |= (unsigned long)read_byte() << 16;
    v |= (unsigned long)read_byte() << 8;
    v |= (unsigned long)read_byte();
    return v;
}

static unsigned long read_varlen(void)
{
    unsigned long v = 0;
    int c;
    do {
        c = read_byte();
        v = (v << 7) | (c & 0x7f);
    } while (c & 0x80);
    return v;
}

/* format a MIDI note number as a name+octave string, e.g. 60 -> "C4" */
static void fmt_note(int note, char *buf)
{
    sprintf(buf, "%s%d", note_names[note % 12], note / 12 - 1);
}

static void parse_meta(void)
{
    unsigned char buf[256];
    unsigned long i;
    int  type = read_byte();
    unsigned long len = read_varlen();

    for (i = 0; i < len && i < sizeof(buf) - 1; i++)
        buf[i] = (unsigned char)read_byte();
    for (; i < len; i++)
        read_byte();   /* skip bytes that overflow our buffer */
    buf[len < sizeof(buf) - 1 ? len : sizeof(buf) - 1] = '\0';

    switch (type) {
    case 0x00:
        printf("Sequence Number %d", (buf[0] << 8) | buf[1]);
        break;
    case 0x01: printf("Text \"%s\"",        buf); break;
    case 0x02: printf("Copyright \"%s\"",   buf); break;
    case 0x03: printf("Track Name \"%s\"",  buf); break;
    case 0x04: printf("Instrument \"%s\"",  buf); break;
    case 0x05: printf("Lyric \"%s\"",       buf); break;
    case 0x06: printf("Marker \"%s\"",      buf); break;
    case 0x07: printf("Cue Point \"%s\"",   buf); break;
    case 0x20:
        printf("Channel Prefix %d", buf[0]);
        break;
    case 0x2f:
        printf("End of Track");
        break;
    case 0x51: {
        unsigned long us = ((unsigned long)buf[0] << 16) |
                           ((unsigned long)buf[1] <<  8) |
                            (unsigned long)buf[2];
        printf("Tempo %lu us/beat (%.1f BPM)", us, 60000000.0 / us);
        break;
    }
    case 0x54:
        printf("SMPTE Offset %02d:%02d:%02d.%02d.%02d",
               buf[0], buf[1], buf[2], buf[3], buf[4]);
        break;
    case 0x58:
        printf("Time Sig %d/%d, %d clocks/click, %d 32nds/quarter",
               buf[0], 1 << buf[1], buf[2], buf[3]);
        break;
    case 0x59: {
        int sf = (signed char)buf[0];
        const char *key = buf[1] ? minor_keys[sf + 7] : major_keys[sf + 7];
        printf("Key Sig %s %s", key, buf[1] ? "minor" : "major");
        break;
    }
    case 0x7f:
        printf("Sequencer Specific (%lu bytes)", len);
        break;
    default:
        printf("Meta 0x%02x (%lu bytes)", type, len);
        break;
    }
}

static void parse_track(int track_num)
{
    unsigned long len;
    long track_end;
    unsigned long abs_tick = 0;
    int running_status = 0;
    int c0, c1, c2, c3;

    c0 = read_byte(); c1 = read_byte();
    c2 = read_byte(); c3 = read_byte();
    if (c0 != 'M' || c1 != 'T' || c2 != 'r' || c3 != 'k') {
        fprintf(stderr, "midump: expected MTrk, got %c%c%c%c\n",
                c0, c1, c2, c3);
        exit(1);
    }

    len = read_long();
    track_end = file_pos + (long)len;

    printf("\nTrack %d (%lu bytes):\n", track_num, len);
    printf("  %8s  %8s  event\n", "tick", "delta");

    while (file_pos < track_end) {
        unsigned long delta;
        int status, chan;
        char nbuf[8];

        delta = read_varlen();
        abs_tick += delta;

        status = read_byte();
        if (status & 0x80) {
            /* genuine status byte; update running status for channel messages */
            if ((status & 0xf0) != 0xf0)
                running_status = status;
        } else {
            /* data byte with running status: put it back and use last status */
            ungetc(status, fp);
            file_pos--;
            status = running_status;
        }

        printf("  %8lu  %8lu  ", abs_tick, delta);

        chan = status & 0x0f;

        switch (status & 0xf0) {
        case 0x80: {
            int note = read_byte();
            int vel  = read_byte();
            fmt_note(note, nbuf);
            printf("Note Off  ch=%d note=%3d (%-4s) vel=%d",
                   chan, note, nbuf, vel);
            break;
        }
        case 0x90: {
            int note = read_byte();
            int vel  = read_byte();
            fmt_note(note, nbuf);
            if (vel == 0)
                printf("Note Off  ch=%d note=%3d (%-4s) vel=0 [Note On / running]",
                       chan, note, nbuf);
            else
                printf("Note On   ch=%d note=%3d (%-4s) vel=%d",
                       chan, note, nbuf, vel);
            break;
        }
        case 0xa0: {
            int note     = read_byte();
            int pressure = read_byte();
            fmt_note(note, nbuf);
            printf("Aftertouch ch=%d note=%3d (%-4s) pressure=%d",
                   chan, note, nbuf, pressure);
            break;
        }
        case 0xb0: {
            int ctrl = read_byte();
            int val  = read_byte();
            printf("Control   ch=%d ctrl=%3d val=%d", chan, ctrl, val);
            break;
        }
        case 0xc0: {
            int prog = read_byte();
            printf("Program   ch=%d prog=%d", chan, prog);
            break;
        }
        case 0xd0: {
            int pressure = read_byte();
            printf("Chan Pressure ch=%d pressure=%d", chan, pressure);
            break;
        }
        case 0xe0: {
            int lo   = read_byte();
            int hi   = read_byte();
            int bend = ((hi << 7) | lo) - 8192;
            printf("Pitch Bend ch=%d bend=%d", chan, bend);
            break;
        }
        case 0xf0:
            switch (status) {
            case 0xff:
                parse_meta();
                break;
            case 0xf0:
            case 0xf7: {
                unsigned long slen = read_varlen();
                unsigned long i;
                printf("SysEx 0x%02x (%lu bytes)", status, slen);
                for (i = 0; i < slen; i++)
                    read_byte();
                break;
            }
            default:
                printf("Unknown status 0x%02x", status);
                goto done;
            }
            break;
        default:
            printf("Unknown status 0x%02x", status);
            goto done;
        }
        printf("\n");
    }
done:

    /* consume any leftover bytes in the track */
    while (file_pos < track_end) {
        fgetc(fp);
        file_pos++;
    }
}

int main(int argc, char *argv[])
{
    unsigned short format, ntracks, division;
    unsigned long hlen;
    int i;
    int c0, c1, c2, c3;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file.mid\n", argv[0]);
        return 1;
    }
    if (!(fp = fopen(argv[1], "rb"))) {
        fprintf(stderr, "midump: can't open %s\n", argv[1]);
        return 1;
    }

    c0 = read_byte(); c1 = read_byte();
    c2 = read_byte(); c3 = read_byte();
    if (c0 != 'M' || c1 != 'T' || c2 != 'h' || c3 != 'd') {
        fprintf(stderr, "midump: not a MIDI file\n");
        return 1;
    }

    hlen     = read_long();
    format   = read_short();
    ntracks  = read_short();
    division = read_short();

    printf("MIDI File: %s\n", argv[1]);
    printf("Format: %d  Tracks: %d  ", format, ntracks);
    if (division & 0x8000) {
        int fps = -(signed char)(division >> 8);
        int tpf =  division & 0xff;
        printf("Division: %d fps, %d ticks/frame\n", fps, tpf);
    } else {
        printf("Division: %d ticks/beat\n", division);
    }

    /* skip any extra bytes in the header chunk beyond the standard 6 */
    while (file_pos < (long)(8 + hlen)) {
        fgetc(fp);
        file_pos++;
    }

    for (i = 0; i < ntracks; i++)
        parse_track(i);

    fclose(fp);
    return 0;
}
